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

		TF1 *f2 = new TF1("linfit", "[0]+[1]*x", x1, x2);
		f2->SetParLimits(0, 0, maxbkg);
		fHist->Fit("linfit", "FLR");

		f = f2->GetParameter(0);
		s = f2->GetParameter(1);
		return true;
	}

	bool GammaLineFit::FitGauss(double x1, double x2, double &f, double &s, double maxpeakheight, double peakpos, double maxbkg) {
		std::cout << "FitGauss" << std::endl;

		double fwhm = fRes->Eval(peakpos);
		TF1* g1 = new TF1("l1","pol1",x1,peakpos-2*fwhm);
		TF1* g2 = new TF1("l2","pol1",peakpos+2*fwhm,x2);

		TF1* g3 = new TF1("m3","gaus",peakpos-2*fwhm, peakpos+2*fwhm);
		// The total is the sum of the three, each has 3 parameters
		TF1* total = new TF1("mstotal","pol1(0)+pol1(2)+gaus(4)", x1, x2);

		g1->SetParLimits(0, 0, maxbkg);
		g2->SetParLimits(0, 0, maxbkg);
		g3->SetParLimits(0, 0, maxpeakheight);
		g3->FixParameter(1, peakpos);
		g3->FixParameter(2, fwhm/2.55);

		fHist->Fit(g1,"R");
		fHist->Fit(g2,"R+");
		fHist->Fit(g3,"R+");

		Double_t par[7];
		// Get the parameters from the fit
		g1->GetParameters(&par[0]);
		g2->GetParameters(&par[2]);
		g3->GetParameters(&par[4]);

		// Use the parameters on the sum
		total->SetParameters(par);
		//g3->SetParLimits(0, 0, maxpeakheight[0]);
		total->FixParameter(5, fPeakPos.at(0));
		total->FixParameter(6, fRes->Eval(fPeakPos.at(0))/2.55);
		fHist->Fit(total,"R+");
		f = (par[0]+par[2])/2;
		s = (par[1]+par[3])/2;
		return true;

	}
    bool GammaLineFit::FitOnePeak(double x1, double x2, double &f, double &s, double maxpeakheight, double peakpos, double maxbkg){
		std::cout << "FitOnePeak" << std::endl;

		if(maxpeakheight < 3*sqrt(maxbkg)){
    		return FitLinear(x1, x2, f, s, maxbkg);
    	}
    	else return FitGauss(x1,x2, f, s, maxpeakheight, peakpos, maxbkg);
    }

	bool GammaLineFit::FitTwoGauss(double x1, double x2, double &f, double &s, std::vector<double> maxpeakheight, std::vector<double> peakpos, double maxbkg){
		std::cout << "FitTwoGauss" << std::endl;
		double fwhm = fRes->Eval(peakpos[0]);
		if (peakpos[0]>peakpos[1]){
			double tmp = peakpos[0];
			peakpos[0] = peakpos[1];
			peakpos[1] = tmp;

			tmp = maxpeakheight[0];
			maxpeakheight[0] = maxpeakheight[1];
			maxpeakheight[1] = tmp;
		}

		TF1* g1 = new TF1("l1","pol1",x1, peakpos[0]-2*fwhm);
		TF1* g2 = new TF1("l2","pol1",peakpos[1]+2*fwhm,x2);
		TF1* g3 = new TF1("m3","gaus",peakpos[0]-2*fwhm,peakpos[0]+2*fwhm);
		TF1* g4 = new TF1("m4","gaus",peakpos[1]-2*fwhm,peakpos[1]+2*fwhm);

		TF1* total = new TF1("mstotal","pol1(0)+pol1(2)+gaus(4)+gaus(7)", x1, x2 );

		g1->SetParLimits(0, 0, maxbkg);
		g2->SetParLimits(0, 0, maxbkg);
		if (maxpeakheight[0]>1)	g3->SetParLimits(0, 0, maxpeakheight[0]);
		g3->FixParameter(1, peakpos[0]);
		g3->FixParameter(2, fwhm/2.55);

		if (maxpeakheight[1]>1) g4->SetParLimits(0, 0, maxpeakheight[1]);
		g4->FixParameter(1, peakpos[1]);
		g4->FixParameter(2, fRes->Eval(peakpos[1])/2.55);


		fHist->Fit(g1,"R");
		fHist->Fit(g2,"R+");
		fHist->Fit(g3,"R+");
		fHist->Fit(g4,"R+");

		Double_t par[10];
		// Get the parameters from the fit
		g1->GetParameters(&par[0]);
		g2->GetParameters(&par[2]);
		g3->GetParameters(&par[4]);
		g4->GetParameters(&par[7]);

		// Use the parameters on the sum
		total->SetParameters(par);
		//g3->SetParLimits(0, 0, maxpeakheight[0]);
		total->FixParameter(5, peakpos[0]);
		total->FixParameter(6, fwhm/2.55);

		total->FixParameter(8, peakpos[1]);
		total->FixParameter(9, fRes->Eval(peakpos[1])/2.55);
		fHist->Fit(total,"R+");
		f=(par[0]+par[2])/2;
		s = (par[1]+par[3])/2;
		return true;
	}

	bool GammaLineFit::FitTwoPeak(double x1, double x2, double &f, double &s, std::vector<double> maxpeakheight, std::vector<double> peakpos, double maxbkg){
		std::cout << "FitTwoPeak" << std::endl;
		double fwhm = fRes->Eval(peakpos[0]);
		if( maxpeakheight[0] < 3*sqrt(maxbkg))
			return FitOnePeak(x1, x2, f, s, maxpeakheight[1], peakpos[1], maxbkg);
		else if ( maxpeakheight[1] < 3*sqrt(maxbkg))
			return FitOnePeak(x1, x2, f, s, maxpeakheight[0], peakpos[0], maxbkg);
		else if (fabs(peakpos[0] - x1) < 2*fwhm && peakpos[0]>x1)
			return FitOnePeak(peakpos[0]+2*fwhm, x2, f, s, maxpeakheight[1], peakpos[1], maxbkg);
		else if (fabs(peakpos[1] - x2) < 2*fwhm && peakpos[1]<x2)
			return FitOnePeak(x1, peakpos[1]-2*fwhm, f, s, maxpeakheight[1], peakpos[1], maxbkg);
		else
			return FitTwoGauss(x1, x2, f, s, maxpeakheight, peakpos, maxbkg);
	}

	bool GammaLineFit::FitTreePeak(double x1, double x2, double &f, double &s, std::vector<double> maxpeakheight, std::vector<double> peakpos, double maxbkg){
		std::cout << "FitTreePeak" << std::endl;
		double fwhm = fRes->Eval(peakpos[0]);

		if(maxpeakheight[1] < 3*sqrt(maxbkg) || maxpeakheight[2] < 3*sqrt(maxbkg))
			return FitTwoGauss(x1, x2, f, s, maxpeakheight, peakpos, maxbkg);
		else
			return FitTwoGauss(peakpos[0]+2*fwhm, x2, f, s, maxpeakheight, peakpos, maxbkg);
	}
	
	bool GammaLineFit::EstimateLinFit(double &f, double &s, std::vector<double> maxpeakheight, double maxbkg){
		switch (fPeakPos.size()) {
			case 1: return FitOnePeak(fRange.first, fRange.second, f, s, maxpeakheight[0], fPeakPos[0], maxbkg);
			case 2: return FitTwoPeak(fRange.first, fRange.second, f, s, maxpeakheight, fPeakPos, maxbkg);
			case 3: return FitTreePeak(fRange.first, fRange.second, f, s, maxpeakheight, fPeakPos, maxbkg);
			case 4: return FitTreePeak(fPeakPos[0]+2*fRes->Eval(fPeakPos[0]), fRange.second, f, s, maxpeakheight, fPeakPos, maxbkg);

		}
	}
	/*bool GammaLineFit::EstimateLinFit(double &f, double &s, std::vector<double> maxpeakheight, double maxbkg){
		double s1 = 0.;
        double f1 = 0.;
        double s2 = 0.;
        double f2 = 0.;
        if(fPeakPos.size() == 1){
			double x1, x2 ;
			double fwhm = fRes->Eval(fPeakPos[0]);
			if (maxpeakheight[0] < 5*sqrt(maxbkg)){
				std::cout << "prelim fit on whole fit range" << std::endl;
				x1 =fRange.first;
				x2 = fRange.second; 
			}else if (fPeakPos.at(0) - fRange.first > fRange.second - fPeakPos.at(0)){
				x1 = fRange.first;
				x2= fPeakPos.at(0) - 2*fwhm;
				//if(fgl) x2 -=2;
				if(abs(fPeakPos.at(0) - 1460) < 2 or abs(fPeakPos.at(0) - 1525)<2)x2-=2;
				TF1 *f2 = new TF1("linfit", "[0]+[1]*x", x1, x2);
				f2->SetParLimits(0, 0, maxbkg);
				fHist->Fit("linfit", "FLR");

				f1 = f2->GetParameter(0);
				s1 = f2->GetParameter(1);

			
			/*}else if (fPeakPos.at(0) - fRange.first ==fRange.second - fPeakPos.at(0)){
				x1 = fPeakPos.at(0) + 2*fwhm;
				//if (fgl) x1+=2;
				//std::cout << "is ga line" << fgl<<std::endl;
				if(abs(fPeakPos.at(0) - 1460) < 2 or abs(fPeakPos.at(0) - 1525)<2)x1+=2;
				x2 = fRange.second;
				TF1* f = new TF1("linfit", "[0]+[1]*x", x1, x2);
				f->SetParLimits(0, 0, maxbkg);
				fHist->Fit("linfit", "FLR");

				f2 = f->GetParameter(0);
				s2 = f->GetParameter(1);
				std::cout << "the upepr lim on the bkg" << std::endl;                               
			}*/	
/*
			}else if (fPeakPos.at(0) - fRange.first ==fRange.second - fPeakPos.at(0)){
				x1 = fPeakPos.at(0) + 2*fwhm;
				//if (fgl) x1+=2;
				//std::cout << "is ga line" << fgl<<std::endl;
				if(abs(fPeakPos.at(0) - 1460) < 2 or abs(fPeakPos.at(0) - 1525)<2)x1+=2;
				x2 = fRange.second;

				TF1* g1 = new TF1("l1","pol1",fRange.first,fPeakPos.at(0)-2*fRes->Eval(fPeakPos.at(0)));
			
				TF1* g2 = new TF1("l2","pol1",fPeakPos.at(0)+2*fRes->Eval(fPeakPos.at(0)),fRange.second);
				
				TF1* g3 = new TF1("m3","gaus",fPeakPos.at(0)-2*fRes->Eval(fPeakPos.at(0)), fPeakPos.at(0)+2*fRes->Eval(fPeakPos.at(0)));
				// The total is the sum of the three, each has 3 parameters
				TF1* total = new TF1("mstotal","pol1(0)+pol1(2)+gaus(4)", fRange.first,fRange.second );  

				g1->SetParLimits(0, 0, maxbkg);
				g2->SetParLimits(0, 0, maxbkg);
				g3->SetParLimits(0, 0, maxpeakheight[0]);
				g3->FixParameter(1, fPeakPos.at(0));				
				g3->FixParameter(2, fRes->Eval(fPeakPos.at(0))/2.55);		
				
				fHist->Fit(g1,"R");
				fHist->Fit(g2,"R+");
				fHist->Fit(g3,"R+");
				
				 Double_t par[7];
				// Get the parameters from the fit
				g1->GetParameters(&par[0]);
				g2->GetParameters(&par[2]);
				g3->GetParameters(&par[4]);

				// Use the parameters on the sum
				total->SetParameters(par);
				//g3->SetParLimits(0, 0, maxpeakheight[0]);
				total->FixParameter(5, fPeakPos.at(0));				
				total->FixParameter(6, fRes->Eval(fPeakPos.at(0))/2.55);		
				fHist->Fit(total,"R+"); 
				f=(par[0]+par[2])/2;
				s = (par[1]+par[3])/2;
				 return true;
			}                        
			
			if (s1==0 && f1==0 && s2 == 0 && f2 ==0){
				TF1 *f2 = new TF1("linfit", "[0]+[1]*x", x1, x2);
				f2->SetParLimits(0, 0, maxbkg);
				fHist->Fit("linfit", "FLR");
			
				f = f2->GetParameter(0);
				s = f2->GetParameter(1);
				std::cout << "Goodnes: " << double(f2->GetChisquare()/f2->GetNDF()) << std::endl;
				delete f2;
			}
			else if(s1 == 0 && f1 == 0){
				s = s2; f = f2;
			}else if (s2 == 0 && f2 ==0){						
				s = s1; f = f1;
			}else{
				s = (s1+s2)/2.; f = (f1+f2)/2.;
			}

			std::cout << "lin fit one peak on the range " << x1 << "\t" << x2 << std::endl;
			if (s1==0 && f1==0 && s2 == 0 && f2 ==0){
				TF1 *f2 = new TF1("linfit", "[0]+[1]*x", x1, x2);
				f2->SetParLimits(0, 0, maxbkg);
				fHist->Fit("linfit", "FLR");
			
				f = f2->GetParameter(0);
				s = f2->GetParameter(1);
				std::cout << "Goodnes: " << double(f2->GetChisquare()/f2->GetNDF()) << std::endl;
				delete f2;
			}
			else if(s1 == 0 && f1 == 0){
				s = s2; f = f2;
			}else if (s2 == 0 && f2 ==0){						
				s = s1; f = f1;
			}else{
				s = (s1+s2)/2.; f = (f1+f2)/2.;
			}

			std::cout << "lin fit one peak on the range " << x1 << "\t" << x2 << std::endl;
		}
		
		
		else if (fPeakPos.size() == 2){	
		
				std::cout << "1" << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));		
				TF1* g1 = new TF1("l1","pol1",fRange.first,fPeakPos.at(0)-2*fRes->Eval(fPeakPos.at(0)));
			
				std::cout << "2" << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));		
				TF1* g2 = new TF1("l2","pol1",fPeakPos.at(1)+2*fRes->Eval(fPeakPos.at(1)),fRange.second);
				
				std::cout << "3" << std::endl;				
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));		
				TF1* g3 = new TF1("m3","gaus",fPeakPos.at(0)-2*fRes->Eval(fPeakPos.at(0)), fPeakPos.at(0)+2*fRes->Eval(fPeakPos.at(0)));
				TF1* g4 = new TF1("m4","gaus",fPeakPos.at(1)-2*fRes->Eval(fPeakPos.at(1)), fPeakPos.at(1)+2*fRes->Eval(fPeakPos.at(1)));
				
				
				std::cout << "4" << std::endl;
				
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));		
				// The total is the sum of the three, each has 3 parameters
				TF1* total = new TF1("mstotal","pol1(0)+pol1(2)+gaus(4)+gaus(7)", fRange.first,fRange.second );  
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));		
				
				g1->SetParLimits(0, 0, maxbkg);
				g2->SetParLimits(0, 0, maxbkg);
				if (maxpeakheight[0]>1)	g3->SetParLimits(0, 0, maxpeakheight[0]);
				g3->FixParameter(1, fPeakPos.at(0));				
				g3->FixParameter(2, fRes->Eval(fPeakPos.at(0))/2.55);		
				
			 if (maxpeakheight[1]>1) g4->SetParLimits(0, 0, maxpeakheight[1]);
				g4->FixParameter(1, fPeakPos.at(1));				
				g4->FixParameter(2, fRes->Eval(fPeakPos.at(1))/2.55);		
				
				
				fHist->Fit(g1,"R");
				fHist->Fit(g2,"R+");
				fHist->Fit(g3,"R+");
				fHist->Fit(g4,"R+");
				
				 Double_t par[10];
				// Get the parameters from the fit
				g1->GetParameters(&par[0]);
				g2->GetParameters(&par[2]);
				g3->GetParameters(&par[4]);
				g4->GetParameters(&par[7]);

				// Use the parameters on the sum
				total->SetParameters(par);
				//g3->SetParLimits(0, 0, maxpeakheight[0]);
				total->FixParameter(5, fPeakPos.at(0));				
				total->FixParameter(6, fRes->Eval(fPeakPos.at(0))/2.55);		
				
				total->FixParameter(8, fPeakPos.at(1));				
				total->FixParameter(9, fRes->Eval(fPeakPos.at(1))/2.55);		
				fHist->Fit(total,"R+"); 
				f=(par[0]+par[2])/2;
				s = (par[1]+par[3])/2;
				 return true;
		}
		 else{
			double max_peak = *max_element(fPeakPos.begin(), fPeakPos.end());
			int maxElIdx = std::max_element(fPeakPos.begin(),fPeakPos.end()) - fPeakPos.begin();
			double min_peak = *min_element(fPeakPos.begin(), fPeakPos.end());
			int minElIdx = std::min_element(fPeakPos.begin(),fPeakPos.end()) - fPeakPos.begin();
			std::cout<<"Max/min value: "<<max_peak << "\t"<< min_peak << std::endl;
			std::cout << "idx max min el" << maxElIdx << "\t" << minElIdx << std::endl;	
			
			TF1 *fl1;// = new TF1();
			TF1 *fl2;// = new TF1();
			
			double cndf1 = 0.;
			double cndf2 = 0.;
			double fwhm = ceil(fRes->Eval(min_peak));
			double x1, x2;

			// fit for the peak with the smallest en
			if( min_peak- fRange.first > 3*fwhm){
				x1 =fRange.first;
				
				// if the height of the peak w/min energy is small
				if (maxpeakheight[minElIdx] < 4*sqrt(maxbkg)){
                                x2 =max_peak -2*fwhm;}
				
				else x2 = min_peak -2*fwhm;
				std::cout << "1st line bkg to the range" << x1 << "\t" << x2 <<std::endl;
				fl1 = new TF1("linfit1", "[0]+[1]*x", x1,x2);
				fl1->SetParLimits(0, 0, maxbkg);
				std::cout << "*** ******************* 1st fit **********************" <<  std::endl;
				fHist->Fit(fl1, "LR");
				cndf1 = double(fl1->GetChisquare()/fl1->GetNDF());
				std::cout << "Goodnes 1st lin" <<fl1->GetNDF()<<"\t" << cndf1 << std::endl;
				f1 = (fl1->GetParameter(0));
			        s1 = (fl1->GetParameter(1));



			}else std::cout << "1st peak is out of range" << std::endl;
			
			if (fRange.second - max_peak > 3*fwhm){
				 if (maxpeakheight[maxElIdx] < 4*sqrt(maxbkg)){
                                x1 =min_peak +2*fwhm;}
				else x1 = max_peak+2*fwhm;
				std::cout << "fit around the 2nd peak " << x1 << "\t" << fRange.second << std::endl;
				fl2 = new TF1("linfit2", "[0]+[1]*x", ceil(x1), fRange.second );
				std::cout << "*********************** 2nd fit **********************" <<  std::endl;
				 fl2->SetParLimits(0, 0, maxbkg);
		                fHist->Fit(fl2, "LRF");
                		cndf2 = double(fl2->GetChisquare()/fl2->GetNDF());
				std::cout << "Goodnes 2nd lin"  << fl2->GetNDF()<<"\t" <<cndf2 << std::endl;
				 f2 = (fl2->GetParameter(0));
		  	         s2 = (fl2->GetParameter(1));

			}else std::cout << "2nd peak is out of range" << std::endl;
			 
			if (cndf1 ==0 && cndf2 == 0 && fPeakPos.size() == 3){
			// bot peaks are too close to the bothers, zb: 3 peaks in the range
				if (maxpeakheight[1] < 5*sqrt(maxbkg)){
					std::cout << "prelim fit on whole fit range" << std::endl;
					x1 =fPeakPos.at(1)-fwhm;
					x2 =fPeakPos.at(1)+fwhm;
					TF1 *f2 = new TF1("linfit", "[0]+[1]*x", x1, x2);
					f2->SetParLimits(0, 0, maxbkg);
					fHist->Fit("linfit", "FLR");

					f= f2->GetParameter(0);
					s = f2->GetParameter(1);
					return true;
				}else{

					TF1* g1 = new TF1("l1","pol1",fPeakPos.at(0)+2*fRes->Eval(fPeakPos.at(1)),fPeakPos.at(1)-2*fRes->Eval(fPeakPos.at(1)));

					TF1* g2 = new TF1("l2","pol1",fPeakPos.at(1)+2*fRes->Eval(fPeakPos.at(1)),fPeakPos.at(2)-2*fRes->Eval(fPeakPos.at(1)));

					TF1* g3 = new TF1("m3","gaus",fPeakPos.at(1)-2*fRes->Eval(fPeakPos.at(0)), fPeakPos.at(1)+2*fRes->Eval(fPeakPos.at(0)));
					// The total is the sum of the three, each has 3 parameters
					TF1* total = new TF1("mstotal","pol1(0)+pol1(2)+gaus(4)", fRange.first,fRange.second );

					g1->SetParLimits(0, 0, maxbkg);
					g2->SetParLimits(0, 0, maxbkg);
					g3->SetParLimits(0, 0, maxpeakheight[1]);
					g3->FixParameter(1, fPeakPos.at(1));
					g3->FixParameter(2, fRes->Eval(fPeakPos.at(1))/2.55);

					fHist->Fit(g1,"R");
					fHist->Fit(g2,"R+");
					fHist->Fit(g3,"R+");

					Double_t par[7];
					// Get the parameters from the fit
					g1->GetParameters(&par[0]);
					g2->GetParameters(&par[2]);
					g3->GetParameters(&par[4]);

					// Use the parameters on the sum
					total->SetParameters(par);
					//g3->SetParLimits(0, 0, maxpeakheight[0]);
					total->FixParameter(5, fPeakPos.at(0));
					total->FixParameter(6, fRes->Eval(fPeakPos.at(0))/2.55);
					fHist->Fit(total,"R+");
					f=(par[0]+par[2])/2;
					s = (par[1]+par[3])/2;
					return true;
				}
}
			 if (cndf1 > 2|| cndf1 < 0.5){
				 
				 if (cndf2 > 2|| cndf2 < 0.5){
					 
					std::cout << "***********************************" << std::endl;
					
					std::cout << "EROOR:both fits are screwed" << std::endl;
					
					std::cout << "***********************************" << std::endl;
					
					//exit(0);
					s = 0;
					f = maxbkg;
					//return false;
				 }
				 else{
					 std::cout << "Goodness is bad for for the 1st func" << std::endl;
					 f = f2;
					 s = s2;
				 }
			}else if(cndf2 > 2 || cndf2 < 0.5){
				 std::cout << "Goodness is bad for for the 2nd func" << std::endl;
				 f = f1;
	             s = s1;
			}else{
				std::cout << "both fits are well" << std::endl;
				f = (f1+f2)/2.;		
				s = (s1+s2)/2.;			
			}
			//delete fl1;
			//delete fl2;
			}
			//f = 12;
			//s = -0.006;

			if(abs(fPeakPos.at(0) - 1460) < 2 or abs(fPeakPos.at(0) - 1525)<2){
s = 0; f = maxbkg;
}
return true;
	}
	
	*/
	
	
    bool GammaLineFit::ResetFit(TString name, bool ifgammas)
    {
		  //! create fit function
        if(fFitSet)  delete fFitFunction;
        // Just move this line after you remove tiny dicks in the collection of peaks
        fFitFunction = new GammaFitFunction("fit",fNPeaks,fRange,fLinBkg);
		
		 
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

        vector<double> maxPeakCounts;
        maxPeakCounts.resize(fNPeaks,0);
        for (int i=1;i<=fNBins;i++) {
            for(int j=0; j<fNPeaks; j++) {
                if(fabs(fFitHist->GetBinCenter(i)-fPeakPos.at(j))
                   < fRes->Eval(fPeakPos.at(j))) {
			double add = fFitHist->GetBinContent(i)-maxBkg;
                    maxPeakCounts.at(j) += (fFitHist->GetBinContent(i)-maxBkg);
                }
            }
        }
        maxBkg = maxBkg/fFitHist->GetBinWidth(0);

        //! limit fit function
        for(int i=0; i<fNPeaks; i++) {
              if( maxPeakCounts.at(i)<1)  maxPeakCounts.at(i) =1;

			fFitFunction->SetParLimits(3*i+2, 0.0, maxPeakCounts.at(i));
            std::cout << "peak " << i << "\t"<< fPeakPos[i] << " max peak height " << maxPeakCounts.at(i) << "\t" << "max bkg " << maxBkg << std::endl;
        
        }
        //estimating slope and bckg par
        double slope = 0;
		std::cout << "linear fit" << std::endl;
		double range = fRange.second - fRange.first;
		ofstream myfile;
		myfile.open (string(name).c_str());
		
		std::cout << "making a linear fit; printing out the params in " << string(name).c_str() << std::endl;
		
		
		
		myfile << "Counts in peak: " <<maxPeakCounts.at(0)<< std::endl;
		TCanvas *c = new TCanvas();
		fFitFunction->Draw();
		c->SaveAs("linfit.root");
		c->SaveAs("linfit.png");
		myfile.close();
		
        double s = 0.;
        double f = 0.;
        
        if (!EstimateLinFit(f, s, maxPeakCounts, maxBkg)) return false;
		 
		slope = s;
		double free_param = f;
		std::cout << "Slope: " << slope << std::endl;
		std::cout << "free term: " << free_param << std::endl;
		if (slope < 0)	{			
			fFitFunction->SetParLimits(fPeakPos.size()*3+1, 1.5*slope, 0);
		}
		else {
			fFitFunction->SetParLimits(fPeakPos.size()*3+1, 0., 1.5*slope);		
		}

		//Can be a problem at higher enegries
		if (free_param < 0)
			free_param = maxBkg;	
		fFitFunction->SetParLimits(fPeakPos.size()*3, 0.6*free_param, 1.4*free_param);
	
		
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
		    /*if(ifgammas){
                std::cout << "mean" << i << "\t" << fPeakPos.at(i) << std::endl;
				fHistFitter->GetParameter(Form("mean%d",i))->SetLimits(fPeakPos.at(i)-1,
                                                                       fPeakPos.at(i)+1);
                std::cout <<  maxPeakCounts.at(i) << std::endl;
                fFitFunction->SetParLimits(3*i+2, 0.0, maxPeakCounts.at(i));
			}else*/
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
