#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Double_t fitFunction(Double_t *x,Double_t *par) 
{ 
	//return (par[0] / par[1] * TMath::Exp(- par[1] * x[0] ) + par[2] * par[3] * TMath::Exp(- par[3] * x[0] )) / (par[0] + par[2]); 
	//return par[0] * TMath::Exp( - x[0] / par[1]) + par[2] * TMath::Exp( - x[0] / par[3]);
	//return par[1] * TMath::Exp( - x[0] / par[0]);
	
	double t = x[0];
	double lam_f = par[0];
	double nu_f = par[1];
	double nu_dc = par[2];

	double h_f = exp(-t * nu_f);

	double p_f = exp(-lam_f) / (1 - exp(-lam_f)) * lam_f * nu_f * h_f * exp(lam_f * h_f);
	double p_f_integral = exp(-lam_f) / (1 - exp(-lam_f)) * (exp(lam_f * h_f) - 1);

	double p_dc = nu_dc * exp(-t* nu_dc);
	double p_dc_integral = exp(-t* nu_dc);

	return p_f * p_dc_integral + p_f_integral * p_dc;
	
}


void fit_dt(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	double x;
	
	FILE *f = fopen(name,"r");
	
	ROOT::Math::MinimizerOptions::SetDefaultStrategy(2);
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Simplex");
	
	const int n_bins = 200;
	const double fit_range_min = 0;
	const double fit_range_max = 5000;
	
	TH1F *h1 = new TH1F("h1f","hist", n_bins, fit_range_min, fit_range_max);
	h1->SetFillColor(kRed);
	h1->SetFillStyle(3002);
	
	while (!feof(f))
	{ 
		fscanf(f,"%lf \n", &x);		
		h1->Fill(x);
	}	
			
			TF1 *fitFcn = new TF1("fitFcn", fitFunction, 15, 1700, 3);
							
			Double_t norm = n_bins / (fit_range_max - fit_range_min);
			Double_t scale = norm / (h1->Integral());
			h1->Scale(scale);
						
			fitFcn->SetParameter(0, 0.05); 
			fitFcn->SetParLimits(0, 0, 5);
			
			fitFcn->SetParameter(1, 1 / 100.0); 			
			fitFcn->SetParLimits(1, 0, 1 / 10.0);
			
			fitFcn->SetParameter(2, 100 * 1E-6); 			
			fitFcn->SetParLimits(2, 0, 1E-2);
			
			//fitFcn->SetParameter(3, 50); 			
			//fitFcn->SetParLimits(3, 5, 100);
	
			h1->Fit("fitFcn", "R");	
			h1->Draw("");	
			
			//cout <<  fitFcn->GetChisquare() << endl;

			//gSystem->Sleep(3000);			
			
			c1->Update();	
}