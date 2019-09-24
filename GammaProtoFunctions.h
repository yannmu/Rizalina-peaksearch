//proto functions for gamma line fits
// - container for par names (ProtoFct)
// - sum function
// - gaussian peak
// - constant and linear background

#ifndef GammaProtoFunctions_H
#define GammaProtoFunctions_H

//root
#include "TMath.h"
#include "TString.h"

// c++
#include <vector>
#include <array>
#include <iostream>

namespace std {

//! parameter no, parameter names, operators
class GammaProtoFct
{
   public:
      //! Constructor
      GammaProtoFct() { };
      GammaProtoFct(vector<TString> parNames) : fParNames(parNames) { };
      //! Destructor
      virtual ~GammaProtoFct() { };

      //! Operators
      virtual double operator() ( double *x, double *par ) { return 0; };
      friend ostream& operator<<(ostream& os, const GammaProtoFct& fct) {
         os << "[";
         for(int i=0; i<(int) fct.fParNames.size(); i++)
            os << " " << fct.fParNames.at(i).Data();
         os << " ]";
         return os;
      };

      //! Getters
      int              GetNPar()         { return (int) fParNames.size(); };
      TString          GetParName(int i) { return fParNames.at(i);        };
      vector<TString>  GetParNames()     { return fParNames;              };

      //! Setters
      void SetParName(int i,TString name) { fParNames.at(i) = name;       };
      void AddPar(TString name)           { fParNames.push_back(name);    };
      void AddPars(vector<TString> names) { fParNames.insert(fParNames.end(),
                                            names.begin(), names.end());  };
   private:
      vector<TString> fParNames;
};

//! sum of proto functions
class GammaProtoFctSum : public GammaProtoFct
{
   public:
      //! Constructor
      GammaProtoFctSum() { };
      //! Destructor
      virtual ~GammaProtoFctSum() { };

      //! Operators
      double operator() ( double *x, double *par ) {
        double y=0;
        int parCount=0;
        for(int i=0; i<(int) fFcts.size(); i++) {
           y+=(*(fFcts.at(i)))(x,&par[parCount]);
           parCount+=fFcts.at(i)->GetNPar();
        }
        return y;
      };

     void AddProtoFct( GammaProtoFct *fct ) { fFcts.push_back(fct);
                                             AddPars(fct->GetParNames()); };

   private:
     vector<GammaProtoFct*> fFcts;
};

//! standard gauss implementation (with fwhm, not sigma)
class GammaProtoStdGaus : public GammaProtoFct
{
   public:
      //! Constructor
      GammaProtoStdGaus( TString parName1 = "mean",
                         TString parName2 = "fwhm" )
       : GammaProtoFct( { parName1, parName2 } ) { };
      //! Destructor
      virtual ~GammaProtoStdGaus() {};

      //! Operators
      double operator() ( double *x, double *par ) {
         return ( 1 / sqrt( 2 * TMath::Pi() * pow(par[1]/2.35,2) ) )
          *exp( -0.5 * pow(x[0]-par[0],2) / pow(par[1]/2.35,2) );
      };
};

//! normalized gauss implementation (with fwhm, not sigma)
class GammaProtoNormGaus : public GammaProtoStdGaus
{
   public:
      //! Constructor
      GammaProtoNormGaus( TString parName1 = "mean",
                          TString parName2 = "fwhm",
                          TString parName3 = "constant" )
       : GammaProtoStdGaus( parName1, parName2 ) { AddPar( parName3 ); };
      //! Destructor
      virtual ~GammaProtoNormGaus() {};

      //! Operators
      double operator() ( double *x, double *par ) {
        return par[GetNPar()-1] * GammaProtoStdGaus::operator()(x,par);
      };
};

//! const background implementation
class GammaProtoConstBackground : public GammaProtoFct
{
   public:
      //! Constructor
      GammaProtoConstBackground( TString parName1 = "background" )
       : GammaProtoFct( { parName1 } ) { };
      //! Destructor
      virtual ~GammaProtoConstBackground() {};

      //! Operators
      double operator() ( double *x, double *par ) {
         return par[0];
      };
};

//! linear background implementation
class GammaProtoLinearBackground : public GammaProtoFct
{
   public:
      //! Constructor
      GammaProtoLinearBackground( TString parName1 = "background",
                                  TString parName2 = "slope" ,
                                  TString parName3 = "peakpos"
                                )
       : GammaProtoFct( { parName1, parName2, parName3 } ) { };
      //! Destructor
      virtual ~GammaProtoLinearBackground() {};

      //! Operators
      double operator() ( double *x, double *par ) {
         return par[0] + par[1]*(x[0]-par[2]);
      };
};

} // namespace std

#endif
