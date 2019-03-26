#include "GammaLineFit.h"
#include "TFile.h"
#include <chrono>
#include <thread>
namespace std {

    void GammaLineFit::SetHist(TH1D* hist)
    {
        if(fHistSet) {
            cerr << "warning: overwritting histogram" << endl;
            fHist->Delete();
        }
        fHist = (TH1D*) hist->Clone();
        fHistSet = true;
    }

    void GammaLineFit::SetRes(TF1* resCurve)
    {
        if(fResSet) {
            cerr << "warning: overwritting resolution curve" << endl;
            fRes->Delete();
        }
        fRes = (TF1*) resCurve->Clone();
        fResSet = true;
    }

    void GammaLineFit::SetRes(double constRes)
    {
        TF1* res = new TF1("res","[0]",0,10000);
        SetRes(res);
        fRes->SetParameter(0, constRes);
        delete res;
    };

    bool GammaLineFit::FitLinear(double x1, double x2, double &f, double &s, double maxbkg) {
        std::cout << "FitLinear" << std::endl;
  cout << maxbkg << endl;
        TF1 *f2 = new TF1("linfit", "[0]+[1]*x", x1, x2);
        
//        f2->SetParLimits(0, 0.8*maxbkg,  maxbkg+4*(fRange.second - fRange.first));
//
f2->SetParameter(0, maxbkg);
//        f2->SetParLimits(0, 0.8*maxbkg
        fHist->Fit("linfit", "FLR");
cout << "chi2/ndf" << f2->GetChisquare()/f2->GetNDF()    << endl;
double ch =  f2->GetChisquare()/f2->GetNDF();
        f = f2->GetParameter(0);
        s = f2->GetParameter(1);
if(ch < 0.7||ch >1.1){
f = maxbkg;
s=0;}
        return true;
    }

    bool GammaLineFit::FitGauss(double x1, double x2, double &f, double &s, double maxpeakheight,
                                double peakpos, double maxbkg) {
        std::cout << "FitGauss" << std::endl;

        double fwhm = fRes->Eval(peakpos);
        TF1* g1 = new TF1("l1","pol1",x1,peakpos-2*fwhm);
        TF1* g2 = new TF1("l2","pol1",peakpos+2*fwhm,x2);

        TF1* g3 = new TF1("m3","gaus",peakpos-2*fwhm, peakpos+2*fwhm);
        // The total is the sum of the three, each has 3 parameters
        TF1* total = new TF1("mstotal","pol1(0)+gaus(2)", x1, x2);

        g1->SetParLimits(0, 0, maxbkg);
        g2->SetParLimits(0, 0, maxbkg);
        g3->SetParLimits(0, 0, maxpeakheight);
        g3->FixParameter(1, peakpos);
        g3->FixParameter(2, fwhm/2.55);

        fHist->Fit(g1,"R");
        fHist->Fit(g2,"R+");
        fHist->Fit(g3,"R+");

        Double_t par[5];
        for (int i =0; i<2; i++) {
            par[i] = (g1->GetParameter(i) + g2->GetParameter(i)) / 2;
        }
        g2->GetParameters(&par[2]);

        // Use the parameters on the sum
        total->SetParameters(par);
        total->FixParameter(3, fPeakPos.at(0));
        total->FixParameter(4, fRes->Eval(fPeakPos.at(0))/2.55);
        fHist->Fit(total,"R");
cout << "chi2/ndf" << total->GetChisquare()/total->GetNDF()    << endl;

        f = total->GetParameter(0);
        s = total->GetParameter(1);
        return true;

    }

    bool GammaLineFit::FitOnePeak(double x1, double x2, double &f, double &s, double maxpeakheight,
                                  double peakpos, double peakbkg, double bkg){
        std::cout << "FitOnePeak" << std::endl;

        if(maxpeakheight < 2*sqrt(peakbkg)){
            return FitLinear(x1, x2, f, s, bkg);
        }
        else return FitGauss(x1,x2, f, s, maxpeakheight, peakpos, bkg);
    }

    bool GammaLineFit::FitTwoGauss(double x1, double x2, double &f, double &s,
                                   std::vector<double> maxpeakheight, std::vector<double> peakpos, double maxbkg){
        std::cout << "FitTwoGauss" << std::endl;

        if (peakpos[0]>peakpos[1]){
            double tmp = peakpos[0];
            peakpos[0] = peakpos[1];
            peakpos[1] = tmp;

            tmp = maxpeakheight[0];
            maxpeakheight[0] = maxpeakheight[1];
            maxpeakheight[1] = tmp;
        }
        double fwhm = fRes->Eval(peakpos[0]);
        TF1* g1 = new TF1("l1","pol1",x1, peakpos[0]-2*fwhm);
        TF1* g2 = new TF1("l2","pol1",peakpos[1]+2*fwhm,x2);
        TF1* g3 = new TF1("m3","gaus",peakpos[0]-2*fwhm,peakpos[0]+2*fwhm);
        TF1* g4 = new TF1("m4","gaus",peakpos[1]-2*fwhm,peakpos[1]+2*fwhm);

        TF1* total = new TF1("mstotal","pol1(0)+gaus(2)+gaus(5)", x1, x2 );

        g1->SetParLimits(0, 0, maxbkg);
        g2->SetParLimits(0, 0, maxbkg);

        g3->SetParLimits(0, 0, maxpeakheight[0]);
        g3->FixParameter(1, peakpos[0]);
        g3->FixParameter(2, fwhm/2.55);

        g4->SetParLimits(0, 0, maxpeakheight[1]);
        g4->FixParameter(1, peakpos[1]);
        g4->FixParameter(2, fRes->Eval(peakpos[1])/2.55);



        fHist->Fit(g3,"R");
        fHist->Fit(g4,"R+");
        int npols = 0;
        if (peakpos[0]-2*fwhm > x1) {
            fHist->Fit(g1, "R+");
            npols+=1;
        }
        if (peakpos[1]+2*fwhm < x2) {
            fHist->Fit(g2, "R+");
            npols+=1;
        }

        // Get the parameters from the fit
        Double_t par[8];
        for (int i =0; i<2; i++) {
            par[i] = (g1->GetParameter(i) + g2->GetParameter(i)) / npols;
        }
        g3->GetParameters(&par[2]);
        g4->GetParameters(&par[5]);

        // Use the parameters on the sum
        total->SetParameters(par);
        //g3->SetParLimits(0, 0, maxpeakheight[0]);
        total->FixParameter(3, peakpos[0]);
        total->FixParameter(4, fwhm/2.55);

        total->FixParameter(6, peakpos[1]);
        total->FixParameter(7, fRes->Eval(peakpos[1])/2.55);
        fHist->Fit(total,"R");
cout << "chi2/ndf" << total->GetChisquare()/total->GetNDF()    << endl;

        f = total->GetParameter(0);
        s = total->GetParameter(1);
        return true;
    }

    bool GammaLineFit::FitTwoPeak(double x1, double x2, double &f, double &s,
                                  std::vector<double> maxpeakheight, std::vector<double> peakpos,
                                  std::vector<double> maxbkg, double bkg){
        std::cout << "FitTwoPeak" << std::endl;
        double fwhm = fRes->Eval(peakpos[0]);
        if( maxpeakheight[0] < 2*sqrt(maxbkg[0]))
            return FitOnePeak(x1, x2, f, s, maxpeakheight[1], peakpos[1], maxbkg[1], bkg);
        else if ( maxpeakheight[1] < 2*sqrt(maxbkg[1]))
            return FitOnePeak(x1, x2, f, s, maxpeakheight[0], peakpos[0], maxbkg[0], bkg);

        else
            return FitTwoGauss(x1, x2, f, s, maxpeakheight, peakpos, bkg);
    }

    bool GammaLineFit::FitTreePeak(double x1, double x2, double &f, double &s,
                                   std::vector<double> maxpeakheight,
                                   std::vector<double> peakpos,  std::vector<double> maxbkg, double bkg){
        std::cout << "FitTreePeak" << std::endl;
        double fwhm = fRes->Eval(peakpos[0]);

        if(maxpeakheight[1] < 2*sqrt(maxbkg[1]) || maxpeakheight[2] < 2*sqrt(maxbkg[1]))
            return FitTwoGauss(x1, x2, f, s, maxpeakheight, peakpos, bkg);
        else
            return FitTwoGauss(x1, peakpos[2]-2*fwhm, f, s, maxpeakheight, peakpos, bkg);
    }

    bool GammaLineFit::EstimateLinFit(double &f, double &s, std::vector<double> maxpeakheight,
                                      std::vector<double> peakbkg, double bkg){
  cout << "bkg" << bkg<<endl;
            
  switch (fPeakPos.size()) {
            case 1: return FitOnePeak(fRange.first, fRange.second, f, s, maxpeakheight[0], fPeakPos[0],
                                      peakbkg[0], bkg);
            case 2: return FitTwoPeak(fRange.first, fRange.second, f, s, maxpeakheight, fPeakPos, peakbkg, bkg);
            case 3: return FitTreePeak(fRange.first, fRange.second, f, s, maxpeakheight, fPeakPos, peakbkg, bkg);

        }
    }


    bool GammaLineFit::ResetFit(TString name, bool ifgammas)
    {

        //! create fit function
        if(fFitSet)  delete fFitFunction;
        // Just move this line after you remove tiny dicks in the collection of peaks


        //! create fit histo
        if(fFitSet) fFitHist->Delete();
        fRangeBins = { fHist->GetXaxis()->FindBin(fRange.first),
                       fHist->GetXaxis()->FindBin(fRange.second) };
        fNBins     = fRangeBins.second - fRangeBins.first;
        fFitHist   = new TH1D( "fitHist","", fNBins, fRange.first, fRange.second );
        for (int i=0;i<fNBins;i++)
            fFitHist->SetBinContent(i+1,fHist->GetBinContent(fRangeBins.first+i));


        if(!fFitSet) fCanvas1 = new TCanvas();
        fCanvas1->Clear();
        fCanvas1->cd();
        fFitHist->Draw();


        //! evaluate limit for fit (room for improvement...)
        double maxBkg    = 0;
        double bkgBins   = 0;
        bool   isPeak = false;
        double max_bin= 0.;
        double min_bin= 1e8;

        for (int i=1;i<=fNBins;i++) {
            for(int j=0; j<fNPeaks; j++) {
                if(fabs(fFitHist->GetBinCenter(i)-fPeakPos.at(j))
                   < 1.5*fRes->Eval(fPeakPos.at(j))) { isPeak = true; }
            }
            if(!isPeak) {
                maxBkg += fFitHist->GetBinContent(i);
                bkgBins   += 1;
            }
            isPeak = false;
        }
        maxBkg = maxBkg/bkgBins;
        vector<double> bkgInpeak;
        vector<double> maxPeakCounts;
        bkgInpeak.resize(fNPeaks,0);
        maxPeakCounts.resize(fNPeaks,0);
        for (int i=1;i<=fNBins;i++) {
            for(int j=0; j<fNPeaks; j++) {
                if(fabs(fFitHist->GetBinCenter(i)-fPeakPos.at(j))
                   < fRes->Eval(fPeakPos.at(j))) {
                    double add = fFitHist->GetBinContent(i)-maxBkg;
                    maxPeakCounts.at(j) += (fFitHist->GetBinContent(i)-maxBkg);
                    bkgInpeak.at(j) +=maxBkg;
                }
            }
        }
//        maxBkg = maxBkg/fFitHist->GetBinWidth(0);



        for(int i=0; i<fNPeaks; i++) {
            std::cout << "peak " << i << "\t"<< fPeakPos[i] << " max peak height " << maxPeakCounts.at(i)
                      << "\t" << "max bkg " << bkgInpeak.at(i) << std::endl;
            if(maxPeakCounts[i] < 1) maxPeakCounts[i] =1;
        }


        if (fPeakPos.size()>1){
            vector<double> tmp_pos;
            vector<double> tmp_h;

            tmp_pos.push_back(fPeakPos[0]);
            tmp_h.push_back(maxPeakCounts[0]);

            for (int i =1; i<fPeakPos.size(); i++){
                if(maxPeakCounts[i] > sqrt(bkgInpeak[i])){
                    tmp_pos.push_back(fPeakPos[i]);
                    tmp_h.push_back(maxPeakCounts[i]);

                }
            }
            fPeakPos = tmp_pos;
            maxPeakCounts=tmp_h;
        }
        std::cout << fNPeaks << std::endl;
        fNPeaks=fPeakPos.size();
        std::cout << fNPeaks << std::endl;
        std::cout << "sizes" << maxPeakCounts.size() << fPeakPos.size() << std::endl;

        fFitFunction = new GammaFitFunction("fit",fNPeaks,fRange,fLinBkg);
        std::cout << "ffunc created" << std::endl;

        for(int i=0; i<fNPeaks; i++) {
            std::cout << "setting lim to he" << i << std::endl;
            fFitFunction->SetParLimits(3*i+2, 0.0, maxPeakCounts.at(i));
        }


        //! limit fit function
        //estimating slope and bckg par


        double slope = 0;
        double range = fRange.second - fRange.first;
        double f = 0.;

        if (!EstimateLinFit(f, slope, maxPeakCounts, bkgInpeak, maxBkg)) return false;

        double free_param = f;
        std::cout << "Slope: " << slope << std::endl;
        std::cout << "free term: " << free_param << std::endl;
        if (slope < 0)	{
            fFitFunction->SetParLimits(fPeakPos.size()*3+1, 1.5*slope, -1.5*slope);
        }
        else {
            fFitFunction->SetParLimits(fPeakPos.size()*3+1, -1.5*slope, 1.5*slope);
        }

        //Can be a problem at higher enegries
        if (free_param < 0){
            //double mb = fRange.second*maxBkg/(fRange.second - fRange.first);
            free_param = maxBkg;
        }
	std::ofstream outfile;

  outfile.open("slope_offset.lib", std::ios_base::app);
  outfile << fPeakPos[0] << "\t" << free_param << "\t" << slope << "\n" << endl;
        fFitFunction->SetParLimits(fPeakPos.size()*3, 0.5*free_param, 1.5*free_param);
        //fFitFunction->SetParLimits(fPeakPos.size()*3, 0, 1.5*free_param);

        std::cout << "Slope: " << slope << std::endl;
        std::cout << "free term: " << free_param << std::endl;

        //! create histogram fitter
        if(fFitSet)  delete fHistFitter;
        fHistFitter = new BCHistogramFitter(fFitHist, fFitFunction);
        //! gaussian priors for fwhm and peak pos

        if(fFitSet) {
            while(! fResPriors.empty()) {
                delete fResPriors.back();
                fResPriors.pop_back();
            }
            while(! fPeakPriors.empty()) {
                delete fPeakPriors.back();
                fPeakPriors.pop_back();
            }
        }


        /* if(ifgammas){
             std::cout << "setting priors" << std::endl;
             for(int i=0; i<fNPeaks; i++) {
                 fFitFunction->SetParLimits(3*i+0, fPeakPos.at(i)-kPeakPosRange,
                                             fPeakPos.at(i)+kPeakPosRange);
                 //fFitFunction->SetParLimits(3*i+1, fRes->Eval(fPeakPos.at(i))-kFWHMRange,
                     //						fRes->Eval(fPeakPos.at(i))+kFWHMRange);
                 fFitFunction->SetParLimits(3*i+2, 0.0, maxPeakCounts.at(i));
             }
             for(int i=0; i<(int) fPeakPos.size(); i++) {

                 fPeakPriors.push_back(new TF1("peak","gaus(0)",0.,3000.));
                 fPeakPriors.back()->SetParameters(1,fPeakPos.at(i),fPeakPriorWidth);
                 fHistFitter->SetPrior(Form("mean%d",i),fPeakPriors.back());

                 fHistFitter->GetParameter(Form("fwhm%d",i))->Fix(fRes->Eval(fPeakPos.at(i)));
             }
         }*/



        std::cout << "paramters fixed to" << std::endl;
        for(int i=0; i<(int) fPeakPos.size(); i++) {
            if(ifgammas && fPeakPos.size() == 1){
                std::cout << "mean" << i << "\t" << fPeakPos.at(i) << std::endl;
                fHistFitter->GetParameter(Form("mean%d",i))->SetLimits(fPeakPos.at(i)-1,
                                                                       fPeakPos.at(i)+1);
                std::cout <<  maxPeakCounts.at(i) << std::endl;
                fFitFunction->SetParLimits(3*i+2, 0.0, maxPeakCounts.at(i));
            }else
                fHistFitter->GetParameter(Form("mean%d",i))->Fix(fPeakPos.at(i));
            if(fResolPrior){
                //fHistFitter->GetParameter(Form("fwhm%d",i))->SetLimits(fRes->Eval(fPeakPos.at(i)) - 1,
                //                                                      fRes->Eval(fPeakPos.at(i)) +1 );
                fHistFitter->GetParameter(Form("fwhm%d",i))->Fix(fRes->Eval(fPeakPos.at(i)));
            }
            std::cout << "fwhm" << i << "\t" << fRes->Eval(fPeakPos.at(i)) << std::endl;
        }

        //! set options for MC
        fHistFitter->SetOptimizationMethod(BCModel::kOptMinuit);
        fHistFitter->MCMCSetPrecision(fPrecision);
        for(int i=0; i<(int) fPeakPos.size(); i++) {
            fHistFitter->GetParameter(Form("mean%d",i))->SetNbins(5000);
            fHistFitter->GetParameter(Form("fwhm%d",i))->SetNbins(5000);
            fHistFitter->GetParameter(Form("intnsty%d",i))->SetNbins(5000);
        }
        fHistFitter->GetParameter("bkgrnd")->SetNbins(5000);
        fHistFitter->GetParameter("slpe")->SetNbins(5000);


        return true;
    }

    bool GammaLineFit::Fit(TString name, bool ifgammas)
    {
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

        //! open bc log file
        BCLog::SetLogLevel(BCLog::error);
        BCLog::OpenLog(Form("%s.txt",name.Data()));
        BCAux::SetStyle();
        std::cout << "BCAux::SetStyle();" << std::endl;

        //! reset fit
        if(ResetFit(name, ifgammas)){
            std::cout << "ResetFit(name);"<< std::endl;

            //! perform fit

            fHistFitter->Fit();
            std::cout << "       fHistFitter->Fit();"<< std::endl;

            // evaluate fit
            EvaluateFit(name);
            std::cout << "        EvaluateFit(name);"<< std::endl;

            // close log file
            BCLog::CloseLog();

            fFitSet = true;
            return true;
        }
        return false;
    }


    void GammaLineFit::SaveFit(TString name){

        ofstream outfile;
        char fname [500];
        //sprintf("%s.txt", name);
        sprintf(fname, "%s_errorband_hist.txt", std::string(name).c_str());

        outfile.open(fname);

        outfile << "entries" << fHistFitter->GetHistogram()->GetEntries() << endl;
//	std::cout << "entried" << std::endl;

        for (int i=0; i<fHistFitter->GetHistogram()->GetEntries(); i++){
            outfile << fHistFitter->GetHistogram()->GetBinCenter(i) << "\t";
        }

        std::cout << "GetBinContent" << std::endl;
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
//		std::cout << "fitf"<< std::endl;
            for(int i =0; i<100; i++){
                double a = 0;
                double b = 0;
                fHistFitter->GetFitFunctionGraph(fHistFitter->GetBestFitParameters())->GetPoint(i, a, b);
                outfile << b << "\t";
            }
        }
//else std::cout << "fitf"<< std::endl;

        outfile << endl;
        if(fHistFitter->GetErrorBand()){
            std::cout << "band"<<fHistFitter->GetErrorBand()->GetN()<< std::endl;
            //else std::cout << "band doesn't exist" << std::endl;
            outfile  << "band" << endl;
            for(int i =0; i<fHistFitter->GetErrorBand()->GetN(); i++){
                double a = 0;
                double b = 0;
                fHistFitter->GetErrorBand() ->GetPoint(i, a, b);
                outfile << a << "\t";
//			std::cout << i << "\t" << a << "\t";

            }
//		std::cout << "band"<< std::endl;

            outfile << endl;
            for(int i =0; i<fHistFitter->GetErrorBand()->GetN(); i++){
                double a = 0;
                double b = 0;
                fHistFitter->GetErrorBand()->GetPoint(i, a, b);
                outfile << b << "\t";
            }
        }else
            std::cout << "band doesn't exist"<< std::endl;

        //outfile.Close();

    }

    void GammaLineFit::EvaluateFit(TString name)
    {
        //fHistFitter->PrintAllMarginalized(Form("%s.pdf",name.Data()));


        if(! fFitSet) fCanvas2 = new TCanvas();
        fCanvas2->Clear();
        fCanvas2->cd();

        int nparams = 3;
        vector<string> marhists(nparams);
        marhists[0] = "intnsty0";
        marhists[1] = "bkgrnd";
        //marhists[3] = "fwhm0";
        marhists[2] = "slpe";

        char fname [500];
        sprintf(fname, "%s_MCMC.txt", std::string(name).c_str());

        std::cout << fname << std::endl;
        std::ofstream outfile(fname);
        outfile << "*******************************************" << endl;
        std::cout << "*******************************************" << endl;
        for (int npar  = 0; npar<nparams;npar++){
            outfile << marhists[npar] << endl;
            outfile << marhists[npar] << endl;
            BCH1D* mar = fHistFitter->GetMarginalized(Form(marhists[npar].c_str()));
            double mode = mar->GetMode();
            double quantile90 = mar->GetQuantile(0.9);
            outfile << "mode:" << "\t" << mode << endl;
            outfile << "mean:" << "\t" << mar->GetMean() << endl;
            outfile << "STD:" << "\t" << mar->GetSTD() << endl;
            double low, high;
            mar->GetSmallestInterval(low,high);
            outfile << "68% smallest interval:" << "\t" << "["<<
                    low << ";" << high << "]" << endl;

            mar->GetSmallestInterval(low,high,0.955);
            outfile << "95% smallest interval:" << "\t" << "["<<
                    low << ";" << high << "]" << endl;


            mar->GetSmallestInterval(low,high, 0.997);
            outfile << "97% smallest interval:" << "\t" << "["<<
                    low << ";" << high << "]" << endl;

            outfile <<"bin center array" <<endl;
            TH1D * h_mar = mar->GetHistogram();
            int nentries = h_mar->GetEntries();
            for(int i =0; i<nentries; i++){
                outfile << h_mar->GetBinCenter(i) << "\t";
            }
            outfile <<endl;
            for(int i =0; i<nentries; i++){
                outfile << h_mar->GetBinContent(i) << "\t";
            }
            outfile<<endl;

        }
        //! png output of fit result

        fHistFitter->DrawFit("HIST", false); // draw without(?) a legend
        fCanvas2->Print(Form("%s.png",name.Data()));

        SaveFit(name);

        //fCanvas2->Print(Form("%s.pdf",name.Data()));
        //fCanvas2->Print(Form("%s.root",name.Data()));
    }

    void GammaLineFit::WriteToCommonLog(TString name, TString logFileName, double exposure, bool ifgammas )
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

        if(exposure > 0)
            xlog << "-----------------------------------------------------" << endl
                 << "exposure:  "
                 << setw(26) << setprecision(1) << fixed << exposure << " kg*yr"
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

            BCH1D* mar_bkg = fHistFitter->GetMarginalized(Form("bkgrnd"));
            TH1D * hbkg = mar_bkg->GetHistogram();
            double mode_bkg = mar_bkg->GetMode();



            double low, high;
            BCH1D * mar = fHistFitter->GetMarginalized(Form("intnsty%d",i));
            mar->GetSmallestInterval(low,high);
            double quantile90 = mar->GetQuantile(0.9);
            double mode = mar->GetMode();

            xlog << "mode:      "
                 << setw(10) << setprecision(2) << fixed << mode << " + "
                 << setw(5)  << setprecision(2) << fixed << (high-mode) << " - "
                 << setw(5)  << setprecision(2) << fixed << (mode-low)  << " cts (68%)"
                 << endl
                 << "90\% quant:"
                 << setw(27) << setprecision(2) << fixed << quantile90 << " cts "
                 << endl;

            //if(exposure > 0) {
            if(exposure == 0) exposure =1;
            xlog << "-----------------------------------------------------" << endl
                 << "count rate:";
            //   if(low > 0)///* && mode > 3*sqrt(mode_bkg)*/ )
            xlog << setw(10) << setprecision(2) << fixed
                 << mode/exposure << " + "
                 << setw(5) << setprecision(2) << fixed
                 << (high-mode)/exposure << " - "
                 << setw(5) << setprecision(2) << fixed
                 << (mode-low)/exposure  << " cts/kg/yr (68%)"
                 << endl;
            // else

            //if( low>0 ) xlog << "signal above 3sigma" << endl;
            // else xlog << "background 3sigma" << endl;

            fHistFitter->GetMarginalized(Form("intnsty%d",i))->GetSmallestInterval(low,high, 0.95);
            xlog << "count rate:" << setw(10) << setprecision(2) << fixed
                 << mode/exposure << " + "
                 << setw(5) << setprecision(2) << fixed
                 << (high-mode)/exposure << " - "
                 << setw(5) << setprecision(2) << fixed
                 << (mode-low)/exposure  << " cts/kg/yr (95%)"
                 << endl;
            //if( low > 0 ) xlog << "signal above 4sigma" << endl;
            //else xlog << "background 4sigma" << endl;

            fHistFitter->GetMarginalized(Form("intnsty%d",i))->GetSmallestInterval(low,high, 0.997);
            xlog << "count rate:" <<  setw(10) << setprecision(2) << fixed
                 << mode/exposure << " + "
                 << setw(5) << setprecision(2) << fixed
                 << (high-mode)/exposure << " - "
                 << setw(5) << setprecision(2) << fixed
                 << (mode-low)/exposure  << " cts/kg/yr (99.7%)"
                 << endl;
            //if( low>0) xlog << "signal above 5sigma" << endl;
            //else xlog << "background 5sigma" << endl;

            xlog << "< "
                 << setw(5) << setprecision(2) << fixed
                 << quantile90/exposure << " cts (90%)"
                 << endl;
            /*
            double quantile99 = mar->GetQuantile(0.997);

            xlog << "< "
                 << setw(5) << setprecision(2) << fixed
                 << quantile99/exposure << " cts (99.7%)"
                 << endl;
            xlog << "mode/sqrt(mode_bkg): "
                 << setw(5) << setprecision(2) << fixed
                 <<mode/sqrt(mode_bkg)         << endl;

*/


            //}
        }
        xlog << "*****************************************************" << endl << endl;
        xlog.close();
    }

} // namespace
