#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Double_t fitFunction(Double_t *x,Double_t *par) 
{ 
	//return (par[0] / par[1] * TMath::Exp(- par[1] * x[0] ) + par[2] * par[3] * TMath::Exp(- par[3] * x[0] )) / (par[0] + par[2]); 
	return par[0] * TMath::Exp( - x[0] / par[1]) + par[2] * TMath::Exp( - x[0] / par[3]);
}


void fit_dt(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	double x;
	
	FILE *f = fopen(name,"r");
	
	TH1F *h1 = new TH1F("h1f","hist", 495, 0, 2000);
	h1->SetFillColor(kRed);
	h1->SetFillStyle(3002);
	
	while (!feof(f))
	{ 
		fscanf(f,"%lf \n", &x);		
		h1->Fill(x);
	}	
			
			TF1 *fitFcn = new TF1("fitFcn", fitFunction, 20, 1600, 4);
							
			Double_t norm = 0.1;
			Double_t scale = norm / (h1->Integral());
			h1->Scale(scale);
			
			//fitFcn->SetParameter(0, 50); 
			//fitFcn->SetParLimits(0, 5, 500);
			
			fitFcn->SetParameter(1, 200); 			
			fitFcn->SetParLimits(1, 100, 500);
			
			//fitFcn->SetParameter(2, 0.5); 			
			//fitFcn->SetParLimits(2, 0, 1);
			
			fitFcn->SetParameter(3, 50); 			
			fitFcn->SetParLimits(3, 5, 100);
	
			h1->Fit("fitFcn", "R");	
			h1->Draw();	
			
			//cout <<  fitFcn->GetChisquare() << endl;

			//gSystem->Sleep(3000);			
			
			c1->Update();	
}