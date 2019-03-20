#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "getopt.h"

#include "TROOT.h"

#include "TStyle.h"
#include "TString.h"
#include "TObject.h"

#include "TF1.h"
#include <TSystem.h>

#include <ctime>

#include <pthread.h>

#include "GammaLineFit.cxx"
#include "GammaSpectrum.cxx"
#include "GammaLineAnalysis.cxx"

using namespace std;





void usage();
vector<double> get_close_lines(double ienergy, double &woi);
        string check_input_str(string);
int main( int argc, char** argv ){
    //TApplication theApp("My App", 0, 0);
    cout << "****************************************************" << endl;
    cout << "The program to estimate the gamma contamination rate" << endl;
    cout << "Please provide all the required input" << endl;
    cout << "****************************************************" << endl;

    gROOT->SetBatch();

    string line="";
    string keylist = "";
    string dataset = "";
    string lar = "";
    string psd = "";
    string rescurve = "";
    string out_dir = "";
    string BAT_precision = "";
    double energy = 0.;
    double binning = 0.5;
    int M =1;
    int choice = 0;
    bool ifresolpr = true;
    double woi = 25.;
	string gfile ="";
	string summaryfile ="PhaseII53-92_orig.root";
    bool if_pseudo_data = false;
    bool if_gammas = false;
    bool if_test = false;
    int pseudo_data_iter = 0;
    string pseudo_data_path = "";
    while(1){
        int optIndex = 0;
        static struct option long_options[] =
                {
                        { "line", required_argument,    0, 'g' },
                        { "energy", required_argument,  0, 'e' },
                        { "keylist", required_argument, 0, 'k' },
                        { "dataset", required_argument, 0, 'd' },
                        { "larmode", required_argument, 0, 'l' },
                        { "binning", required_argument, 0, 'b' },
                        { "rescurve", required_argument, 0, 'r' },
                        { "out_dir", required_argument, 0, 'o' },
                        { "psdmode", required_argument, 0, 'p'},
                        { "precision", required_argument, 0, 'j'},
                        { "multiplicity", required_argument, 0, 'm'},
                        {"offresolprior", required_argument, 0, 'a'},
                        {"woi", required_argument, 0, 'w'},
                        {"pseudo_data", no_argument, 0, 't'},
                        {"pseudo_data_iteration", required_argument, 0, 'i'},
                        {"pseudo_data_path", required_argument, 0, 'f'},
                        {"gammafile", required_argument, 0, 'n'},
                        {"summaryfile", required_argument, 0, 's'},
                        {"ifgammaanalysis", no_argument, 0, 'z'},
                        {"iftest", no_argument, 0, 'u'},
                        


                };
///nfs/gerda2/users/rizalinko/gamma-analysis/GammaTierHandler.h
        choice = getopt_long ( argc, argv, "g:e:k:d:l:b:r:o:p:j:a:i:f:w:n:s:tzu", long_options, &optIndex );

        if( choice == -1 )
            break;

        if( choice== 'g'  ){
            line = check_input_str(optarg);
            cout << "analysing line: " << line << endl;
        }else if( choice == 'e' ){
            energy = atof(optarg);
            cout << "energy: " << energy << endl;
        }else if( choice== 'k'  ){
            keylist = check_input_str(optarg);
            cout << "keylist: " << keylist << endl;
        }else if( choice== 'd'  ){
            dataset  = check_input_str(optarg);
            cout << "dataset: " << dataset << endl;
        }else if( choice== 'l'  ){
            lar = check_input_str(optarg);
            cout << "lar mode: " << lar << endl;
        }else if( choice== 'b'  ) {
            binning = atof(optarg);
            cout << "binning: " << binning <<endl;
        }else if( choice== 'r'  ) {
            rescurve = check_input_str(optarg);
            cout << "res curve for priors: " << rescurve << endl;
        }else if( choice== 'o'  ) {
            out_dir = check_input_str(optarg);
            cout << "outdir: " << out_dir << endl;
        }else if( choice== 'j'  ) {
            BAT_precision = check_input_str(optarg);
            cout << "BAT precision" << BAT_precision << endl;
        }else if( choice== 'p'  ) {
            psd = check_input_str(optarg);
            cout << "psd mode: " << psd << endl;
        }else if( choice== 'm'  ) {
            M = atoi(optarg);
            cout << "Multiplicity " << M << endl;
        }else if( choice== 'a'  ) {
            ifresolpr = false;
            cout << "Switching off resol prior" << M << endl;
        }else if( choice== 't'  ) {
            if_pseudo_data = true;
            cout << "Running tests on pseudo data " << endl;
        }else if( choice== 'z'  ) {
            if_gammas = true;
            cout << "Running gamma analysis " << endl;
        }else if( choice== 'f'  ) {
            pseudo_data_path = check_input_str(optarg);
            cout << "pseudo data directory " << pseudo_data_path << endl;
        }else if( choice== 'w'  ) {
            woi = atof(optarg);
            cout << "woi " << woi << endl;
        }
        else if( choice== 'i'  ) {
            pseudo_data_iter = atoi(optarg);
            cout << "pseudo data iteration " << pseudo_data_iter << endl;
        }else if( choice== 'f'  ) {
            pseudo_data_path = check_input_str(optarg);
            cout << "pseudo data directory " << pseudo_data_path << endl;
        }else if( choice== 'n'  ) {
            gfile = check_input_str(optarg);
            cout << "gammas are listed in " << gfile << endl;
        }else if( choice== 's'  ) {
            summaryfile = check_input_str(optarg);
            cout << "summary file" << summaryfile << endl;
        }else if( choice== 'u'  ) {
            if_test = true;
            cout << "Running tests " << endl;
        }

    }

    const char * sc =  rescurve.c_str();
    cout << "res curve for priors: " << sc << endl;

    TFile* resFile = new TFile(sc);

    TF1* resCurve = new TF1();
    if (dataset.find("Coax") !=std::string::npos)
        resCurve = (TF1*) resFile->Get("Coax");
    else if (dataset.find("BEGe") !=std::string::npos)
        resCurve = (TF1*) resFile->Get("BEGe");
    else
        resCurve = (TF1*) resFile->Get("Nat");
	vector < GammaAutoSpectrum * > specs;
	GammaLineAnalysis *gg = new GammaLineAnalysis(dataset, lar, out_dir, gfile, summaryfile, binning);
    if(!if_pseudo_data) {
		
        // create spectra (e.g. EnrCoax + EnrCoaxNoPSD for w/ and w/o LAr veto)
        vector <string> ds;
        ds.push_back(dataset);

        string spec_name = dataset;
        //give spectra name accordingly to lar mode
        if (lar.find("LArAC") != std::string::npos)
            spec_name.append("-LArAC");
        else if (lar.find("LArC") != std::string::npos)
			spec_name.append("-LArC");

        //give spectra name accordingly to psd mode
        if (psd.find("PSDAC") != std::string::npos) {
            spec_name.append("-PSDAC");
            cout << spec_name << endl;
        } else if (psd.find("PSDC") != std::string::npos)
            spec_name.append("-PSDC");

        cout << "pushing spectra" << spec_name << endl;
        specs.push_back(new GammaAutoSpectrum(spec_name, binning)); //  0.2 -> binning
        specs.back()->SetDatasets(ds);

        if (lar.find("LArAC") != std::string::npos)
            specs.back()->SetLArMode(GammaAutoSpectrum::LArMode::kLArAC);
        else if (lar.find("LArC") != std::string::npos)
            specs.back()->SetLArMode(GammaAutoSpectrum::LArMode::kLArC);

        if (psd.find("PSDAC") != std::string::npos)
            specs.back()->SetPSDMode(GammaAutoSpectrum::PSDMode::kPSDAC);
        else if (psd.find("PSDC") != std::string::npos)
            specs.back()->SetPSDMode(GammaAutoSpectrum::PSDMode::kPSDC);

     

        // create the gamma analysis

        if (BAT_precision.find("low") != std::string::npos)
            gg->SetPrecision(BCEngineMCMC::kLow);
            //ToDo: pass precision from the input vars
        else if (BAT_precision.find("med") != std::string::npos)
            gg->SetPrecision(BCEngineMCMC::kMedium);
        else
            gg->SetPrecision(BCEngineMCMC::kHigh);
		for(auto& kv : specs) gg->RegisterSpectrum(kv->GetName(),kv,resCurve);

    }
	 else if(if_pseudo_data) {
		char spectrum_name[100];
		sprintf(spectrum_name, "pseudo_data_%d", pseudo_data_iter);
		specs.push_back(new GammaAutoSpectrum(spectrum_name, binning));
		for (auto &kv : specs) gg->RegisterSpectrum(kv->GetName(), kv, resCurve);
		std::cout << "the spectra is registered" << std::endl;
		gg->RegisterPseudoData(pseudo_data_iter, pseudo_data_path, energy);
		std::cout << "the pseudodata is registered" << std::endl;

	//
		}

	
	if(if_gammas)
		gg->RegisterStandardLines(woi, resCurve);
	
	else{
		double fwhm = resCurve->Eval(energy);
		vector<double> peaks = gg->GetCloseLinesToLine(energy, woi, fwhm);
		gg->RegisterLine(line, peaks,{energy - woi,energy+woi});
		std::cout << line << "  the line is registered" << std::endl;
	}


std::cout<< if_gammas<< if_test<<std::endl;
    // start fits (will fit registered lines to registerd spectra)
    gg->PerformFits(resCurve, ifresolpr, if_gammas, if_test);
    // you will find some output in the log directory
    // *spectrName*lineName*.pdf  -> posterior distributions
    // *spectrName*lineName*.png  -> pic of the fit
    // *spectrName*lineName*.root -> fit hist, fit, band
    // *spectrName*lineName*.txt  -> BAT log file
    // gamma.log-> summary file with result for all fits
    delete resCurve;
    delete resFile;
    delete gg;

}


string check_input_str(string inp){
    inp.erase(std::remove_if(inp.begin(), inp.end(), (int(*)(int))isspace), inp.end());
    return inp;
}

