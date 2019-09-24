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
void SetLArPSDMode(GammaAutoSpectrum * spec, string, string);
string GetSpectraName(string , string, string );
TF1*  getResolCurve(string , string);

int main( int argc, char** argv ){
    //TApplication theApp("My App", 0, 0);
    cout << "****************************************************" << endl;
    cout << "The program to estimate the gamma contamination rate v.1" << endl;
    cout << "Please provide all the required input" << endl;
    cout << "****************************************************" << endl;

    gROOT->SetBatch();

    string keylist = "";
    int M =1;

    string line=""; // The name of line to fit. Typically consists of dataset and energy
    string dataset = "";
    string lar = ""; // mode for the LAr cut: LArAC,LArC, bare
    string psd = ""; //
    string rescurve = "";
    string out_dir = "";
    string BAT_precision = "";
    double energy = 0.;
    double binning = 0.5;


    double woi = 25.;
    string gfile =""; // list of the bkg gamma lines
    string summaryfile ="PhaseII53-92_orig.root"; // file with the data
    bool if_pseudo_data = false; // flag: on for the analysis on pseudo-data
    bool if_gammas = false; //on: for analysis of the gamma bkg

    int pseudo_data_iter = 0; // num of the pseudo-exp, used for the output naming
    string pseudo_data_path = ""; //
    string pseudo_data_file = "";
    string priors_file = ""; // priors on the bkg shape; provided as a starting vals for the pseudo-data analysis
    bool ifpriors = false; // ToDo: check


    while(1) {
        int optIndex = 0;
        static struct option long_options[] =
                {
                        {"line",                  required_argument, 0, 'g'},
                        {"energy",                required_argument, 0, 'e'},
                        {"priors",                no_argument,       0, 'k'},
                        {"dataset",               required_argument, 0, 'd'},
                        {"larmode",               required_argument, 0, 'l'},
                        {"binning",               required_argument, 0, 'b'},
                        {"rescurve",              required_argument, 0, 'r'},
                        {"out_dir",               required_argument, 0, 'o'},
                        {"psdmode",               required_argument, 0, 'p'},
                        {"precision",             required_argument, 0, 'j'},
                        {"multiplicity",          required_argument, 0, 'm'},
                        {"offresolprior",         required_argument, 0, 'a'},
                        {"woi",                   required_argument, 0, 'w'},
                        {"pseudo_data",           no_argument,       0, 't'},
                        {"pseudo_data_iteration", required_argument, 0, 'i'},
                        {"pseudo_data_path",      required_argument, 0, 'f'},
                        {"gammafile",             required_argument, 0, 'n'},
                        {"summaryfile",           required_argument, 0, 's'},
                        {"ifgammaanalysis",       no_argument,       0, 'z'},
                        {"pseudo_file",           required_argument, 0, 'v'},
                        {"priorsfile",            required_argument, 0, 'c'},


                };
        int choice = 0;
        choice = getopt_long(argc, argv, "g:e:d:l:b:r:o:p:j:i:f:w:n:s:v:c:tz", long_options, &optIndex);

        if (choice == -1)
            break;

        if (choice == 'g') {
            line = check_input_str(optarg);
            cout << "analysing line: " << line << endl;
        } else if (choice == 'e') {
            energy = atof(optarg);
            cout << "energy: " << energy << endl;
        } else if (choice == 'd') {
            dataset = check_input_str(optarg);
            cout << "dataset: " << dataset << endl;
        } else if (choice == 'l') {
            lar = check_input_str(optarg);
            cout << "lar mode: " << lar << endl;
        } else if (choice == 'b') {
            binning = atof(optarg);
            cout << "binning: " << binning << endl;
        } else if (choice == 'r') {
            rescurve = check_input_str(optarg);
            cout << "res curve for priors: " << rescurve << endl;
        } else if (choice == 'o') {
            out_dir = check_input_str(optarg);
            cout << "outdir: " << out_dir << endl;
        } else if (choice == 'j') {
            BAT_precision = check_input_str(optarg);
            cout << "BAT precision" << BAT_precision << endl;
        } else if (choice == 'p') {
            psd = check_input_str(optarg);
            cout << "psd mode: " << psd << endl;
        } else if (choice == 't') {
            if_pseudo_data = true;
            cout << "Running tests on pseudo data " << endl;
        } else if (choice == 'z') {
            if_gammas = true;
            cout << "Running gamma analysis " << endl;
        } else if (choice == 'f') {
            pseudo_data_path = check_input_str(optarg);
            cout << "pseudo data directory " << pseudo_data_path << endl;
        } else if (choice == 'w') {
            woi = atof(optarg);
            cout << "woi " << woi << endl;
        } else if (choice == 'i') {
            pseudo_data_iter = atoi(optarg);
            cout << "pseudo data iteration " << pseudo_data_iter << endl;
        } else if (choice == 'n') {
            gfile = check_input_str(optarg);
            cout << "gammas are listed in " << gfile << endl;
        } else if (choice == 's') {
            summaryfile = check_input_str(optarg);
            cout << "summary file" << summaryfile << endl;
        } else if (choice == 'v') {
            pseudo_data_file = check_input_str(optarg);
        } else if (choice == 'c') {
            priors_file = check_input_str(optarg);
            cout << "pr file" << priors_file << endl;
        }
    }


    TF1* resCurve =   getResolCurve(dataset, rescurve);

    vector < GammaAutoSpectrum * > specs; // list of the spectra with specified settings

    // instance of the class handling the fit procedure
    GammaLineAnalysis *gg = new GammaLineAnalysis(dataset, lar, out_dir, gfile, summaryfile, binning);


    if(!if_pseudo_data) { // analysis of the phy data
        //create spectra name accordingly to lar and psd mode
        string spec_name = GetSpectraName(dataset, lar, psd);

        cout << "pushing spectra" << spec_name << endl;
        specs.push_back(new GammaAutoSpectrum(spec_name, binning));

        SetLArPSDMode(specs.back(), lar, psd);


        if (BAT_precision.find("low") != std::string::npos)
            gg->SetPrecision(BCEngineMCMC::kLow);
        else if (BAT_precision.find("med") != std::string::npos)
            gg->SetPrecision(BCEngineMCMC::kMedium);
        else
            gg->SetPrecision(BCEngineMCMC::kHigh);


        for(auto& kv : specs) gg->RegisterSpectrum(kv->GetName(),kv,resCurve);

    }
    else{
        char spectrum_name[100];
        sprintf(spectrum_name, "pseudo_data_%d", pseudo_data_iter);
        specs.push_back(new GammaAutoSpectrum(spectrum_name, binning));
        for (auto &kv : specs) gg->RegisterSpectrum(kv->GetName(), kv, resCurve);
        std::cout << "the spectra is registered" << std::endl;
        gg->RegisterPseudoData(pseudo_data_iter, pseudo_data_path, energy, pseudo_data_file);
        std::cout << "the pseudodata is registered" << std::endl;

        //
    }


    if(if_gammas && line.size() == 0)
        gg->RegisterStandardLines(woi, resCurve);

    else{
        double fwhm = resCurve->Eval(energy);
        vector<double> peaks = gg->GetCloseLinesToLine(energy, woi, fwhm);
        gg->RegisterLine(line, peaks,{energy - woi, energy+woi});
        std::cout << line << "  the line is registered" << std::endl;
    }


    // start fits (will fit registered lines to registerd spectra)
    gg->PerformFits(resCurve, priors_file, ifpriors, if_gammas);
    // you will find some output in the log directory
    // *spectrName*lineName*.pdf  -> posterior distributions
    // *spectrName*lineName*.png  -> pic of the fit
    // *spectrName*lineName*.root -> fit hist, fit, band
    // *spectrName*lineName*.txt  -> BAT log file
    // gamma.log-> summary file with result for all fits
    delete resCurve;
    delete gg;
}


TF1*  getResolCurve(string dataset, string rescurve) {

    TFile *resFile = new TFile(rescurve.c_str());
    TF1 *resCurve = new TF1();
    if (dataset.find("Coax") != std::string::npos)
        resCurve = (TF1 *) resFile->Get("Coax");
    else if (dataset.find("BEGe") != std::string::npos)
        resCurve = (TF1 *) resFile->Get("BEGe");
    else
        resCurve = (TF1 *) resFile->Get("Nat");

    delete resFile;
    return resCurve;
}

string check_input_str(string inp){
    inp.erase(std::remove_if(inp.begin(), inp.end(), (int(*)(int))isspace), inp.end());
    return inp;
}

string GetSpectraName(string dataset, string lar, string psd) {
    string spec_name = dataset;
    if (lar.find("LArAC") != std::string::npos)
        spec_name.append("-LArAC");
    else if (lar.find("LArC") != std::string::npos)
        spec_name.append("-LArC");

    if (psd.find("PSDAC") != std::string::npos) {
        spec_name.append("-PSDAC");
        cout << spec_name << endl;
    } else if (psd.find("PSDC") != std::string::npos)
        spec_name.append("-PSDC");
    return  spec_name;
}



void SetLArPSDMode(GammaAutoSpectrum * spec, string lar, string psd){
    if (lar.find("LArAC") != std::string::npos)
        spec->SetLArMode(GammaAutoSpectrum::LArMode::kLArAC);
    else if (lar.find("LArC") != std::string::npos)
        spec->SetLArMode(GammaAutoSpectrum::LArMode::kLArC);
    if (psd.find("PSDAC") != std::string::npos)
        spec->SetPSDMode(GammaAutoSpectrum::PSDMode::kPSDAC);
    else if (psd.find("PSDC") != std::string::npos)
        spec->SetPSDMode(GammaAutoSpectrum::PSDMode::kPSDC);
}