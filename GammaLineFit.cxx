#include "GammaLineFit.h"
#include "TFile.h"
#include "BAT/BCSummaryTool.h"
#include <string.h>


//ToDo: refactor functions FitLinear, Fit Guass
namespace std {

    void GammaLineFit::SetHist(TH1D *hist) {
        if (fHistSet) {
            cerr << "warning: overwritting histogram" << endl;
            fHist->Delete();
        }
        fHist = (TH1D *) hist->Clone();
        fHistSet = true;
    }

    void GammaLineFit::SetRes(TF1 *resCurve) {
        if (fResSet) {
            cerr << "warning: overwritting resolution curve" << endl;
            fRes->Delete();
        }
        fRes = (TF1 *) resCurve->Clone();
        fResSet = true;
    }

    void GammaLineFit::SetRes(double constRes) {
        TF1 *res = new TF1("res", "[0]", 0, 10000);
        SetRes(res);
        fRes->SetParameter(0, constRes);
        delete res;
    };


    bool GammaLineFit::Fit( bool ifpriors, string priors_file, string log_dir, string ds,TString name,  bool ifgammas){
        m_priors_file = priors_file; // provided pre-computed priors on slope and offset

        //! check if all ingredient available
        if(! fHistSet) {
            cerr << "error: no histogram set" << endl;
            return false;
        }
        if(! fResSet) {
            cerr << "error: no resolution set" << endl;
            return false;
        }
        if(! fRangeSet) {
            cerr << "error: no range set" << endl;
            return false;
        }

        char prior_file[500];
        sprintf(prior_file, "%s/priors_lib_%s_%.0f", log_dir.c_str(), ds.c_str(), fPeakPos[0]);
        cout << prior_file << endl;


        //! open bc log file
        BCLog::SetLogLevel(BCLog::error);
        BCLog::OpenLog(Form("%s.txt",name.Data()));
        BCAux::SetStyle();

        //! reset fit
        std::pair <double,double>  range_scale;
        std::pair <double,double>  range_offset;
        std::pair <double,double>  range_intens;

        PrepareSettings(ifpriors, range_scale, range_offset, range_intens);

        int fit_it = 0;
        while(fit_it<fNfits){
            cout << "************************************" << endl <<"Fit number: " << fit_it << endl;

            //! perform fit
            fHistFitter->Fit(); //calling the method of the BAT class BCHistogramFitter

            //! Check posteriors; updates the flags (private members of the class) if something is not ok
            EvaluateFit(name);
            if(m_adjust) // true if one of the posterior close to its edges
                AdjustSettings(fit_it, range_scale, range_offset, range_intens);
            else if(!m_wide_post) // it the posterior is not wide enough
                // (non-reproducible weird behaviour, just repeat the fit with current settings)
                continue;
            else // everythin is ok
                break;
            fit_it+=1;
        }

        if(fit_it==fNfits){ // reached maximum number of the fit iterations; and still didn't converge properly
            range_offset.first = m_f - 2 * m_ft_err;
            range_offset.second = m_f +2 * m_ft_err;

            range_scale.first = m_s - 2*m_sloper_err;
            range_scale.second = m_s + 2*m_sloper_err ;
            EmergencyFit();
            ifemergfit = true; // if one doesn't estimate quantiles while writing to the output log
        }

        // ToDo: exclude also pseudo data from the last two cond
        if(!ifpriors && !ifemergfit) {
            std::ofstream o_priors;
            o_priors.open(prior_file, std::ios_base::app);
            o_priors << fPeakPos[0] << "\t" << "offset " << range_offset.first << "\t" << range_offset.second << "\t"
                     << "slope " << range_scale.first << "\t" << range_scale.second << endl;
        }
        if(!ifemergfit) {
            if (!fFitSet) fCanvas2 = new TCanvas();
            fCanvas2->Clear();
            fCanvas2->cd();

            cout << "Saving output fit in png: " << Form("%s.png", name.Data())<< endl;
            fHistFitter->DrawFit("HIST", false); // draw without(?) a legend
            fCanvas2->Print(Form("%s.png", name.Data()));
            
            //!Save bat output to the txt for future plotting w/Py
            //SaveFit(name);
        }

        fFitSet = true;
        return true;
    }


    void GammaLineFit::AdjustSettings(  int it, std::pair <double,double>&  range_scale,
                                        std::pair <double,double>  &range_offset,
                                        std::pair <double,double>&  range_intens){

        //! Estimate bin width for posterior of each of the param
        double b_w = fabs((range_offset.second - range_offset.first))/m_post_bing; //ofset
        double s_w = fabs((range_scale.second - range_scale.first))/m_post_bing; //slope
        double i_w = fabs((range_intens.second - range_intens.first))/m_post_bing; // signal intensity

        const int side_bins = 1000 * (it + 1); // there was a reason why I did it
        if (!m_left_slope) {
            cout << "Adjust slope left" << endl;
            range_scale.first -= s_w * side_bins;
        }
        if (!m_right_slope) {
            cout << "Adjust slope right " << endl;
            range_scale.second += s_w * side_bins;
        }
        if (!m_left_off) {
            cout << "Adjust offset left" << endl;
            range_offset.first -= b_w * side_bins;
        }
        if (!m_right_off) {
            cout << "Adjust offset right" << endl;
            range_offset.second += b_w * side_bins;
        }
        if (!m_right_intens) {
            cout << "Adjust intens right" << endl;
            range_intens.second += i_w * side_bins;
        }

        // strength for "intnsty0" = intensity of the peak with 0 idx in the list (~ main peak of interest)
        fHistFitter->GetParameter(Form("intnsty%d",0))->SetLimits(range_intens.first, range_intens.second );

        fHistFitter->GetParameter("bkgrnd")->SetLimits(range_offset.first, range_offset.second);
        fHistFitter->GetParameter("slpe")->SetLimits(range_scale.first, range_scale.second);
    }


    void GammaLineFit::PrepareSettings( bool ifpriors, // if to use precomputed priors for offset and slope
                                        std::pair <double,double>&  range_scale,
                                        std::pair <double,double>&  range_offset,
                                        std::pair <double,double>&  range_intens){

        // function to estimate starting ranges for the parameters

        //! ToDo: check if u need the part below
        //! create fit histo
        fRangeBins = {fHist->GetXaxis()->FindBin(fRange.first),
                      fHist->GetXaxis()->FindBin(fRange.second)};
        fNBins = fRangeBins.second - fRangeBins.first;
        fFitHist = new TH1D("fitHist", "", fNBins, fRange.first, fRange.second);
        for (int i = 0; i < fNBins; i++)
            fFitHist->SetBinContent(i + 1, fHist->GetBinContent(fRangeBins.first + i));


        //! evaluate limit for fit (room for improvement...)
        double maxBkg = 0;
        double bkgBins = 0;
        bool isPeak = false;
        for (int i = 1; i <= fNBins; i++) {
            // check if current bin correspond to one of the peaks from the spectrum
            for (int j = 0; j < fNPeaks; j++) {
                if (fabs(fFitHist->GetBinCenter(i) - fPeakPos.at(j))
                    < 1.5 * fRes->Eval(fPeakPos.at(j))) { isPeak = true; }
            }
            // if not increment number of the bkg events by bin content
            if (!isPeak) {
                maxBkg += fFitHist->GetBinContent(i);
                bkgBins += 1;
            }
            // reset the flag
            isPeak = false;
        }

        maxBkg = maxBkg / bkgBins; // avg number of bkg events per bkg bin

        vector<double> bkgInpeak; // number of the bkg events in each peak
        vector<double> maxPeakCounts; // number of signal counts in each peak
        bkgInpeak.resize(fNPeaks, 0);
        maxPeakCounts.resize(fNPeaks, 0);
        for (int i = 1; i <= fNBins; i++) {
            for (int j = 0; j < fNPeaks; j++) {
                if (fabs(fFitHist->GetBinCenter(i) - fPeakPos.at(j))
                    < 1.5*fRes->Eval(fPeakPos.at(j))) {
                    maxPeakCounts.at(j) += (fFitHist->GetBinContent(i) - maxBkg);
                    bkgInpeak.at(j) += maxBkg;
                }
            }
            for (int i = 0; i < fNPeaks; i++) { // to account for the overfluct of counts in peak
                if (maxPeakCounts[i] < 1) maxPeakCounts[i] = 1;
            }
        }

        //! For bat adding a peak if only its more than a sigma higher than the bkg at the peak region
        if (fPeakPos.size() > 1) {
            vector<double> tmp_pos;
            vector<double> tmp_h;
            vector<double> tmp_b;
            tmp_pos.push_back(fPeakPos[0]);
            tmp_h.push_back(maxPeakCounts[0]);
            tmp_b.push_back(bkgInpeak[0]);
                //! Start from 1 later
                for (int i = 1; i < fPeakPos.size(); i++) {
                    cout << "checking peak at " << fPeakPos[i] << endl;
                    cout << "height " << maxPeakCounts[i] << " bkg fluct  " << bkgInpeak[i] << endl;
                    if (maxPeakCounts[i] > 3.5 * sqrt(bkgInpeak[i])) {
                        tmp_pos.push_back(fPeakPos[i]);
                        tmp_h.push_back(maxPeakCounts[i]);
                        tmp_b.push_back(bkgInpeak[i]);
                    }
                }
            fPeakPos = tmp_pos;
            maxPeakCounts = tmp_h;
            bkgInpeak=tmp_b;
        }
        fmaxpeakcounts = maxPeakCounts;
        fbkgpeak = bkgInpeak;

        fNPeaks = fPeakPos.size();
        fFitFunction = new GammaFitFunction("fit", fNPeaks, fRange, fPeakPos[0]); // it needs the last param for the linear function


        //! If specified use provided priors, else make a minuit fit and compute priors
        bool ifPriorsOn = GetPriors(range_scale, range_offset);
        if(!ifpriors || !ifPriorsOn)
            ComputePriors(maxPeakCounts, bkgInpeak, maxBkg, range_scale,
                          range_offset);

        // create fitter obj and set estimated priors
        delete fHistFitter;
        fHistFitter = new BCHistogramFitter(fFitHist, fFitFunction);

        fHistFitter->GetParameter("peakpos")->Fix(fPeakPos.at(0)); // for parametrization of the linear bkg
        fHistFitter->GetParameter("bkgrnd")->SetLimits(range_offset.first, range_offset.second);
        fHistFitter->GetParameter("slpe")->SetLimits(range_scale.first, range_scale.second);
        fHistFitter->GetParameter("bkgrnd")->SetNbins(5000);
        fHistFitter->GetParameter("slpe")->SetNbins(5000);
        // compute signal strength prior for each of the peak and set the params for the BCHistogramFitter itself

        range_intens.first = 0;
        for(int i=0; i<(int) fPeakPos.size(); i++) {
            double sig_strength = maxPeakCounts.at(i) + 5 * sqrt(maxPeakCounts.at(i)) + 5 * sqrt(bkgInpeak.at(i)) + 10;
            double bkg_fluct = 5 * sqrt(bkgInpeak.at(i));
            if (sig_strength < bkg_fluct)
                sig_strength = bkg_fluct;
            if(i==0)
                range_intens.second = sig_strength;

            fHistFitter->GetParameter(Form("intnsty%d",i))->SetLimits(0, sig_strength );
            fHistFitter->GetParameter(Form("mean%d",i))->Fix(fPeakPos.at(i));
            fHistFitter->GetParameter(Form("fwhm%d",i))->Fix(fRes->Eval(fPeakPos.at(i)));
            fHistFitter->GetParameter(Form("intnsty%d",i))->SetNbins(5000);
        }
        //! set options for MC
        fHistFitter->SetOptimizationMethod(BCModel::kOptMinuit);
        fHistFitter->MCMCSetPrecision(fPrecision);
    }


    bool GammaLineFit::GetPriors(std::pair <double,double> &range_scale,
                                 std::pair <double,double>  &range_offset){

        std::ifstream file(m_priors_file.c_str());
        if(!file.good()){
            cout << m_priors_file<<" priors file doesn't exist " << endl;
            return false;
        }
        std::string str;
        char str_par1 [100];
        char str_par2 [100];
        float en=0;
        cout << m_priors_file << endl;
        std::getline(file, str);
        cout << "str " << str << endl;
        float sl1 =0.;
        float sl2 =0.;

        float off1=0.;
        float off2 =0.;
        sscanf (str.c_str(),"  %f %s %f  %f %s %f %f", &en, str_par1, &off1, &off2, str_par2,&sl1, &sl2 );
        range_offset.first = off1;
        range_offset.second = off2;

        range_scale.first = sl1;
        range_scale.second = sl2;

        cout << en <<"\t"<<str_par1 << "\t" << range_offset.first <<"\t"
             << range_offset.second <<"\t"<<
             str_par2<<"\t"<< range_scale.first <<"\t"<< range_scale.second << endl;
    }


    void GammaLineFit::ComputePriors( std::vector<double> maxpeakheight,
                    std::vector<double> peakbkg, double bkg, std::pair <double,double>&  range_scale,
                    std::pair <double,double>&  range_offset){

        EstimateLinFit(maxpeakheight, peakbkg, bkg); // updates private variables m_f, m_s ~ best fit values from minuit
        m_scale_slope_l = 3.;
        m_scale_slope_r = 3.;
        m_scale_bkg_l = 5;
        m_scale_bkg_r =5;

        range_offset.first = m_f - m_scale_bkg_l * m_ft_err;
        range_offset.second = m_f + m_scale_bkg_r * m_ft_err;

        range_scale.first = m_s - m_scale_slope_l*m_sloper_err;
        range_scale.second = m_s + m_scale_slope_r*m_sloper_err ;
    }

    bool GammaLineFit::EstimateLinFit( std::vector<double> maxpeakheight,
                                       std::vector<double> peakbkg, double bkg) {
        // here is preliminary estimation of the params for the model is performed


        // for the preliminary fit only the highest peak among those exceeding 3sigma of the bkg fluct is included
        // params of the highest peak
        double peak_h = 0;
        double peak_pos;
        for(int i =0; i<fPeakPos.size(); i++){
            if (maxpeakheight[i] > 3*sqrt(peakbkg[i])){
                if(peak_h < maxpeakheight[i]){
                    peak_h = maxpeakheight[i];
                    peak_pos = fPeakPos[i];
                }
            }
        }

        double x1 = fRange.first; //- 10;
        double x2 = fRange.second; //+ 10;

        cout << "*****************************************" << endl <<
             "Preliminary Mintuit fit"<< endl <<"*****************************************" << endl;

        if(peak_h> 0)
            return FitGauss(x1, x2, peak_h,peak_pos, bkg);
        else
            return FitLinear(x1 ,x2,bkg); // only range and the avg bkg level
    }



    void GammaLineFit::EvaluateFit(TString name)
    {   //! If correctly estimated values for the left and right edges of the posteriors of the paramteres (slope, offset)
        m_left_slope = true;
        m_right_slope= true;

        m_left_off = true;
        m_right_off= true;

        //! If one of the posteriors is too wide
        m_wide_post = true;

        //! If right edge of the posterior for intensity is correctly estimated
        m_right_intens = true;

        //! If ranges must be adgusted
        m_adjust = false;


        int nparams = 3;
        vector<string> marhists(nparams);
        marhists[0] = "intnsty0";
        marhists[1] = "slpe";
        marhists[2] = "bkgrnd";


        const int side_bins = m_post_bing*0.1; //! We will check 10% of bins from left and right.
        //! If they are empty - > posterior estimated properly

        //! Add a check if mar hist exists
        for (int npar  = 0; npar<nparams;npar++) {
            //if( npar ==1) continue;

            //! Getting posterior marginalized distribution for one of the params and converting it to TH1D
            BCH1D *mar = fHistFitter->GetMarginalized(Form(marhists[npar].c_str()));
            TH1D *h_mar = mar->GetHistogram();

            //int nentries = m_post_bing+1;
            //for (int i = nentries - side_bins; i < nentries; i++) {

            //! Checking if the 10% bins from the right edge of the posterior are empty
            for (int i =  m_post_bing - side_bins; i < m_post_bing; i++) {
                if (h_mar->GetBinContent(i) > 0) {
                    switch(npar) {
                        case 0: m_right_intens = false; //! intensity is too close to the right
                        case 1: m_right_slope = false;
                        default: m_right_off = false; //! Right edge of the posterior for offset is not computed well
                    }
                    m_adjust = true; //
                    return;
                }
            }

            //! The left edge is checked only for slope and offset (it's ok if the intensity of the line is close to 0)
            if(npar>0){
                for (int i = 0; i < side_bins; i++) {
//                  //        0 - intens, 1 -s, 2 - off
                    if (h_mar->GetBinContent(i) > 0) {
                        switch(npar) {
                            case 1: m_left_slope = false;
                            default: m_left_off = false; //! Right edge of the posterior for offset is not computed well
                        }
                         m_adjust = true;
                         return;
                    }
                }
            }

            //! Check if number with non-0 entries are too few (happen sometimes, not entirely sure why)
            //! Non reproducable behavior, typicall fixed with another run of the fit
            //! In this case the flag m_wide_post is set to false, meaning that no need to adjust the params, and just rerun the fit
            int n_nonepmty_bins = 0.;
            for (int i = side_bins; i < m_post_bing-side_bins; i++) {
                if (h_mar->GetBinContent(i) > 0)
                    n_nonepmty_bins += 1;
            }
            if (n_nonepmty_bins < 10) {
                m_wide_post = false;
                m_adjust = true;
                cout << "One bin prob" << endl;
            }
        }
        return;
    }


    bool GammaLineFit::FitGauss(double x1, double x2, double maxpeakheight,
                                double peakpos, double maxbkg) {

        double fwhm = fRes->Eval(peakpos);

        // fit separately bkg before peak, peak, and bkg after the peak
        TF1* g1 = new TF1("l1","[0]+[1]*(x-[2])",x1,peakpos-2*fwhm);
        TF1* g2 = new TF1("l2","[0]+[1]*(x-[2])",peakpos+2*fwhm,x2);
        TF1* g3 = new TF1("m3","gaus",peakpos-2*fwhm, peakpos+2*fwhm);

        // The total is the sum of the three, each has 3 parameters
        TF1* total = new TF1("mstotal","[0]+[1]*(x-[2])+gaus(3)", x1, x2);

        // set starting params

        g1->SetParameters(maxbkg, 0);
        g2->SetParameters(maxbkg, 0);
        g1->FixParameter(2, fPeakPos[0]);
        g2->FixParameter(2, fPeakPos[0]);

        g3->SetParameters(float(maxpeakheight/sqrt(2*3.1)), peakpos, fwhm/2.55 );
        g3->SetParLimits(0, 1, maxpeakheight);
        g3->FixParameter(1, peakpos);
        g3->FixParameter(2, fwhm/2.35);

        // fit
        fHist->Fit(g1,"ER");
        fHist->Fit(g2,"ER+");
        fHist->Fit(g3,"ER+");

        // create starting params for the total func
        Double_t par[6];
        for (int i =0; i<2; i++) { // offset and slope
            par[i] = (g1->GetParameter(i) + g2->GetParameter(i)) / 2;
        }
        par[2] = fPeakPos[0];

        g3->GetParameters(&par[3]);

        // Use the parameters on the sum
        total->SetParameters(par);
        total->FixParameter(2, fPeakPos[0]);

        total->FixParameter(4, peakpos);
        total->FixParameter(5,fwhm/2.35);
        fHist->Fit(total,"LR");
        cout << "chi2/ndf" << total->GetChisquare()/total->GetNDF()    << endl;


        m_s = total->GetParameter(1);
        m_f = total->GetParameter(0);

        m_sloper_err = total->GetParError(1);
        m_ft_err = sqrt(total->GetParError(0)*total->GetParError(0));

//        m_fit_log << fPeakPos[0] << "\t" << m_s << "+/-" << m_sloper_err << "\t" <<
//                  total->GetParameter(0) << "+/-" <<total->GetParError(0) << endl;
        return true;

    }

    bool GammaLineFit::FitLinear(double x1, double x2, double maxbkg) {
        TF1 *f2 = new TF1("linfit", "[0]+[1]*(x-[2])", x1, x2);
        f2->SetParameter(0, maxbkg);
        f2->FixParameter(2, fPeakPos[0]);
        fHist->Fit("linfit", "FLR"); // F - minuit;

        m_s = f2->GetParameter(1); // slope paramter
        m_f = f2->GetParameter(0);//offset param
        m_sloper_err = f2->GetParError(1); // slope error from the fit
        m_ft_err = sqrt(f2->GetParError(0)*f2->GetParError(0));// offset error from the fit
       //m_fit_log << fPeakPos[0] << "\t" << m_s << "+/-" << m_sloper_err << "\t" <<
        //          f2->GetParameter(0) << "+/-" << f2->GetParError(0) << endl;
        return true;
    }


    void GammaLineFit::EmergencyFit() {

        fHistFitter->GetParameter("bkgrnd")->Fix(m_f);
        fHistFitter->GetParameter("slpe")->Fix(m_s);
        fHistFitter->Fit();

    }

    void GammaLineFit::SaveFit(TString name){

        // 
        ofstream outfile;
        char fname [500];
        sprintf(fname, "%s_errorband_hist.txt", std::string(name).c_str());
        outfile.open(fname);
        outfile << "entries" << fHistFitter->GetHistogram()->GetEntries() << endl;

        for (int i=0; i<fHistFitter->GetHistogram()->GetEntries(); i++){
            outfile << fHistFitter->GetHistogram()->GetBinCenter(i) << "\t";
        }

       outfile << endl;
        for (int i=0; i<fHistFitter->GetHistogram()->GetEntries(); i++){
            outfile << fHistFitter->GetHistogram()->GetBinContent(i) << "\t";
        }

        outfile << endl;
        outfile  << "fitf" << endl;

        if(fHistFitter->GetFitFunctionGraph(fHistFitter->GetBestFitParameters())){
            for(int i =0; i<100; i++){
                double a = 0;
                double b = 0;
                fHistFitter->GetFitFunctionGraph(fHistFitter->GetBestFitParameters())->GetPoint(i, a, b);
                outfile << a << "\t";
            }

            outfile << endl;
            for(int i =0; i<100; i++){
                double a = 0;
                double b = 0;
                fHistFitter->GetFitFunctionGraph(fHistFitter->GetBestFitParameters())->GetPoint(i, a, b);
                outfile << b << "\t";
            }
        }

        outfile << endl;
        if(fHistFitter->GetErrorBand()){
            outfile  << "band" << endl;
            for(int i =0; i<fHistFitter->GetErrorBand()->GetN(); i++){
                double a = 0;
                double b = 0;
                fHistFitter->GetErrorBand() ->GetPoint(i, a, b);
                outfile << a << "\t";
            }

            outfile << endl;
            for(int i =0; i<fHistFitter->GetErrorBand()->GetN(); i++){
                double a = 0;
                double b = 0;
                fHistFitter->GetErrorBand()->GetPoint(i, a, b);
                outfile << b << "\t";
            }
        }else
            std::cout << "band doesn't exist"<< std::endl;
    }



    void GammaLineFit::WriteToCommonLog(TString name, TString logFileName, double exposure,
                                        bool ifgammas, TString ds )
    {
        if(! fFitSet) {
            cout << "no fit performed, nothing to write to log" << endl;
            return;
        }

        ofstream xlog;
        xlog.open(logFileName.Data(), ios::app);     
        xlog << "*****************************************************" << endl
             << name.Data()
             << endl;

        xlog << "-----------------------------------------------------" << endl
             << "fit range: "
             << setw(17) << setprecision(1) << fixed << fRange.first   << " - "
             << setw( 6) << setprecision(1) << fixed << fRange.second  << " keV"
             << endl
             << "binning:   "
             << setw(26) << setprecision(1) << fixed << fFitHist->GetBinWidth(0) << " keV"
             << endl
             << "p-val:     "
             << setw(26) << setprecision(2) << fixed << fHistFitter->GetPValue()
             << endl;
        
        double low, high, quantile90, mode;
        BCH1D *mar;
        double of = 0.;

        if(!ifemergfit) {
            mar = fHistFitter->GetMarginalized("slpe");
            mar->GetSmallestInterval(low, high);
            quantile90 = mar->GetQuantile(0.9);
            mode = mar->GetMode();
        }else{
            mode = m_s;
            low = m_s - m_sloper_err;
            high = m_s + m_sloper_err;
        }
        xlog << " slpe :    "
             << setw(10) << setprecision(2) << fixed << mode << " + "
             << setw(5) << setprecision(2) << fixed << (high - mode) << " - "
             << setw(5) << setprecision(2) << fixed << (mode - low) << " cts (68%)"
             << endl;


        if(!ifemergfit) {

            BCH1D *mar_bkg = fHistFitter->GetMarginalized(Form("bkgrnd"));
            mode = mar_bkg->GetMode();
            mar_bkg->GetSmallestInterval(low, high);


        }else{
            mode = m_f;
            low = m_f-m_ft_err;
            high = m_f+m_ft_err;
        }

        xlog << " bkg :    "
             << setw(10) << setprecision(2) << fixed << mode << " + "
             << setw(5) << setprecision(2) << fixed << (high - mode) << " - "
             << setw(5) << setprecision(2) << fixed << (mode - low) << " cts (68%)"
             << endl;
       
        for(int i=0;i<fNPeaks;i++) {

            if(ifgammas){
                xlog << "-----------------------------------------------------" << endl
                     << "peakPos:   "
                     << setw(7)  << setprecision(2) << fixed << fPeakPos.at(i) << " -> "
                     << setw(7)  << setprecision(2) << fixed
                     << Form("%.2f",fHistFitter->GetBestFitParameter(3*i+0))  << " +- "
                     << setw(4)  << setprecision(2) << fixed
                     << Form("%.2f",fHistFitter->GetBestFitParameterError(3*i+0)) << " keV"
                     << endl
                     << "FWHM:      "
                     << setw(7)  << setprecision(2) << fixed << fRes->Eval(fPeakPos.at(i)) << " -> "
                     << setw(7)  << setprecision(2) << fixed
                     << Form("%.2f",fHistFitter->GetBestFitParameter(3*i+1))  << " +- "
                     << setw(4)  << setprecision(2) << fixed
                     << Form("%.2f",fHistFitter->GetBestFitParameterError(3*i+1)) << " keV"
                     << endl;
            }
                //Since peak pos and fwhm are fixed
            else{
                xlog << "-----------------------------------------------------" << endl
                     << "peakPos:   "
                     << setw(7)  << setprecision(2) << fixed << fPeakPos.at(i)
                     << endl
                     << "FWHM:      "
                     << setw(7)  << setprecision(2) << fixed << fRes->Eval(fPeakPos.at(i))
                     << endl;
            }

            mar = fHistFitter->GetMarginalized(Form("intnsty%d",i));
            mar->GetSmallestInterval(low,high);
            quantile90 = mar->GetQuantile(0.9);
            mode = mar->GetMode();
            double stdev = mar->GetSTD();




            xlog << "mode:      "
                 << setw(10) << setprecision(2) << fixed << mode << " + "
                 << setw(5)  << setprecision(2) << fixed << (high-mode) << " - "
                 << setw(5)  << setprecision(2) << fixed << (mode-low)  << " cts (68%)"
                 << "std: " << stdev
                 << endl
                 << "90\% quant:"
                 << setw(27) << setprecision(2) << fixed << quantile90 << " cts "
                 << endl;

            xlog << "-----------------------------------------------------" << endl
                 << "counts:";
            xlog << setw(10) << setprecision(2) << fixed
                 << mode << " + "
                 << setw(5) << setprecision(2) << fixed
                 << (high-mode) << " - "
                 << setw(5) << setprecision(2) << fixed
                 << mode-low  << " cts (68%)"
                 << endl;
            fHistFitter->GetMarginalized(Form("intnsty%d",i))->GetSmallestInterval(low,high, 0.95);
            xlog << "counts:" << setw(10) << setprecision(2) << fixed
                 << mode << " + "
                 << setw(5) << setprecision(2) << fixed
                 << (high-mode) << " - "
                 << setw(5) << setprecision(2) << fixed
                 << (mode-low)  << " cts (95%)"
                 << endl;

            fHistFitter->GetMarginalized(Form("intnsty%d",i))->GetSmallestInterval(low,high, 0.997);
            xlog << "count rate:" <<  setw(10) << setprecision(2) << fixed
                 << mode << " + "
                 << setw(5) << setprecision(2) << fixed
                 << (high-mode) << " - "
                 << setw(5) << setprecision(2) << fixed
                 << setw(5) << setprecision(2) << fixed
                 << (mode-low)  << " cts (99.7%)"
                 << endl;

            xlog << "< "
                 << setw(5) << setprecision(2) << fixed
                 << quantile90 << " cts (90%)"
                 << endl;

        }
        xlog << "*****************************************************" << endl << endl;
        xlog.close();
    }

}