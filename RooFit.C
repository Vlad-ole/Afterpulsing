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


void RooFit_my(char name[])
{
   RooRealVar t("t","time", 0, 2000);
   
   RooRealVar nu_f("nu_f","time", 1.0 / 20, 0, 1.0);
   RooRealVar nu_s("nu_s","time", 1.0 / 100, 0, 1.0);
   RooRealVar nu_dc("nu_dc","time", 0.001, 0, 0.010);
   
   RooRealVar p_s("p_s","time", 0.1, 0, 1);
   RooRealVar p_f("p_f","time", 0.2, 0, 1);
   
   //RooRealVar tau("tau","tau parameter", 50, 1, 200);
   //RooGenericPdf genpdf("genpdf","genpdf","( 1 / tau * exp( - t / tau ) )", RooArgSet(t, tau));
   RooGenericPdf genpdf("genpdf","genpdf"," p_s * p_f * (nu_s + nu_f + nu_dc)*exp( - (nu_s + nu_f + nu_dc)*t)  + p_s*(1 - p_f)*(nu_dc + nu_s)*exp(-(nu_dc + nu_s)*t) + p_f*(1 - p_s)*(nu_dc + nu_f)*exp(-(nu_dc + nu_f)*t) + (1 - p_s)*(1- p_f)*nu_dc*exp(-nu_dc *t)", RooArgSet(t, nu_f, nu_s, nu_dc, p_s, p_f));
   
	bool data_from_file = true;
	
	if(!data_from_file)
		RooDataSet* data = genpdf.generate(t, 5000);
	else
	{
		std::vector<double> x;
		RooRealVar varx("t","t",-TMath::Infinity(), TMath::Infinity() );  
		RooDataSet *data = new RooDataSet("data","data",varx); 
		ifstream file_input(name);
		
		while(!file_input.eof())
		{
			double value;
			file_input >> value;
			varx.setVal( value ); 
			data->add(varx);		
		}
		
	}
	/*
	for (unsigned int i = 0; i < 100; ++i) 
	{ 
		varx.setVal( i ); 
		data->add(varx);
		//data->Print("v");
	}
	*/
	
	
	//for (unsigned int i = 0; i < 100; ++i)
	/*while(!file_input.eof())
	{
		double value;
		file_input >> value;
		varx.setVal( value ); 
		data->add(varx);		
	}
	*/
	
   
	genpdf.fitTo(*data, Range(50, 2000));
	cout << endl;
	nu_f.Print();
	nu_s.Print();
	nu_dc.Print();
	p_s.Print();
	p_f.Print();
	cout << endl;
	
   
   RooPlot* tframe = t.frame(Title("Interpreted expression pdf"));
   data->plotOn(tframe);
   genpdf.plotOn(tframe);
   
   cout << endl << tframe->chiSquare(5) << endl;
   
   TCanvas* c = new TCanvas("rf103_interprfuncs","rf103_interprfuncs",800,400);
   tframe->GetYaxis()->SetTitleOffset(1.4);
   tframe->Draw();
}

