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


void Fit_my_2()
{
	RooRealVar t("t","time", 1, 30001);

	RooRealVar tau("tau","tau parameter", 50, 1, 200);
	RooGenericPdf genpdf("genpdf","genpdf","( 1 / tau * exp( - t / tau ) )", RooArgSet(t, tau)); 

	RooDataSet* data = genpdf.generate(t, 30001);

	t.setRange("R1", 800, 1000);
	t.setRange("R2", 250, 400);

	genpdf.fitTo(*data, Range("R1,R2"));

	RooPlot* tframe = t.frame(Title("Interpreted expression pdf"));
	data->plotOn(tframe, Binning(5000));
	genpdf.plotOn(tframe);

	TCanvas* c = new TCanvas("rf103_interprfuncs","rf103_interprfuncs",800,400);
	tframe->GetYaxis()->SetTitleOffset(1.4);
	tframe->Draw();
}

