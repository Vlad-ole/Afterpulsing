#include <iostream>
#include <fstream>
#include <string>

#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"

#include "TCanvas.h"
#include "TAxis.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include "RooGenericPdf.h"
#include "RooConstVar.h"
#include "RooClassFactory.h"

using namespace std;
using namespace RooFit;


void RooFit_my()
{
   RooRealVar t("t","time", 0, 150);
   RooRealVar tau("tau","tau parameter", 50, 1, 100);
   RooGenericPdf genpdf("genpdf","genpdf","( 1 / tau * exp( - t / tau ) )", RooArgSet(t, tau));
   
   //double x[5] = {0, 5, 4, 8, 0};
  //RooDataSet* data = genpdf.generate(t, 5000);

	std::vector<double> x;
	RooRealVar varx("t","t",-TMath::Infinity(), TMath::Infinity() );  
	RooDataSet *data = new RooDataSet("data","data",varx); 
	for (unsigned int i = 0; i < 100; ++i) 
	{ 
		varx.setVal( i ); 
		data->add(varx);
		//data->Print("v");
	}
	
   
	genpdf.fitTo(*data);
	cout << endl;
	tau.Print();
	cout << endl;
   
   RooPlot* tframe = t.frame(Title("Interpreted expression pdf"));
   data->plotOn(tframe);
   genpdf.plotOn(tframe);
   
   TCanvas* c = new TCanvas("rf103_interprfuncs","rf103_interprfuncs",800,400);
   tframe->GetYaxis()->SetTitleOffset(1.4);
   tframe->Draw();
}

