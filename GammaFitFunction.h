//gamma fit function
// - multiple gauss on flat bkgrnd

#ifndef GammaFitFunction_H
#define GammaFitFunction_H

//root
#include "TF1.h"
#include "TString.h"

#include "GammaProtoFunctions.h"

namespace std {
class GammaFitFunction : public TF1
{
   public:
      //! Constructor
      GammaFitFunction() {};
      GammaFitFunction( TF1* fct ) : TF1(*fct) {};
      GammaFitFunction( const char* name,
                        GammaProtoFct* protoFct,
                        pair<double,double> range ) {
         cout << "creating fit function with parameters " << *protoFct << endl;

         TF1* fct = new TF1(name,protoFct,range.first,range.second,protoFct->GetNPar());
         for(int i=0; i<protoFct->GetNPar();i++)
            fct->SetParName(i,protoFct->GetParName(i));

         new (this) GammaFitFunction(fct);
      };
      GammaFitFunction( const char* name,
                        int nGaus,
                        pair<double,double> range,
                        double peak
                       ) {
         GammaProtoFctSum* protoFct = new GammaProtoFctSum();
	
         for(int i=0; i<nGaus; i++)
            protoFct->AddProtoFct(new GammaProtoNormGaus(Form("mean%d",i),Form("fwhm%d",i),Form("intnsty%d",i)));
        protoFct->AddProtoFct(new GammaProtoLinearBackground("bkgrnd","slpe", "peakpos"));

         new (this) GammaFitFunction(name,protoFct,range);

      };

      //! Destructor
      virtual ~GammaFitFunction() {

       //ADD PROPER DESTRUCTOR HERE!
      };
};

} // namespace std

#endif
