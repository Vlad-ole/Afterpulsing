#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Double_t fitFunction(Double_t *x,Double_t *par) 
{ 
	return par[0] * pow(x[0], 1.5) * TMath::Exp(- par[1] / x[0] ); 
	//return par[0] * TMath::Exp( par[1] * x[0] ); 
}


void fit_dc(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	
	std::vector<double> xerrv;
	std::vector<double> yerrv;
	
	Double_t x, y, xerr, yerr;
	FILE *f = fopen(name,"r");
	
	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf %lf %lf\n", &x, &y, &xerr, &yerr);
		xv.push_back(x); 
		yv.push_back(y);
		
		xerrv.push_back(xerr);
		yerrv.push_back(yerr);
	}	

			TGraphErrors * gr = new TGraphErrors(xv.size(), &xv[0], &yv[0], &xerrv[0], &yerrv[0]);
			TF1 *fitFcn = new TF1("fitFcn", fitFunction, 0, 1000, 2);
			
			/*
			fitFcn->SetParameter(0, 0.0303); 
			fitFcn->SetParameter(1, 0.061); 
				
			fitFcn->SetParLimits(0, 0.0303, 0.0303);
			fitFcn->SetParLimits(1, 0.061, 0.061);
			*/
			
			//fitFcn->SetParameter(0, 0.0303); 
			fitFcn->SetParameter(1, 3133); 
				
			//fitFcn->SetParLimits(0, 0, 1E10);
			fitFcn->SetParLimits(1, 2500, 5000);

			
			gr->Fit("fitFcn", "R");	
			gr->SetMarkerColor(4);
			gr->SetMarkerStyle(kFullCircle);
			gr->Draw("AP");	
			
			cout <<  fitFcn->GetChisquare() << endl;

			//gSystem->Sleep(3000);			
			
			c1->Update();
							
			xv.clear();
			yv.clear();
		
		
		
	
	
}