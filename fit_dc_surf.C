#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Double_t fitFunction(Double_t *x, Double_t *par) 
{ 
	return par[0] * pow(x[0], 1.5) * TMath::Exp(- par[1] / x[0] ) * TMath::Exp( par[2] * x[1] ); 
	//return par[0] * TMath::Exp( par[1] * x[0] ); 
}


void fit_dc_surf(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	std::vector<double> zv;
	
	std::vector<double> xerrv;
	std::vector<double> yerrv;
	std::vector<double> zerrv;
	
	Double_t x, y, z, xerr, yerr, zerr;
	FILE *f = fopen(name,"r");
	
	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf %lf %lf %lf %lf\n", &x, &xerr, &y, &yerr, &z, &zerr);
		xv.push_back(x); 
		yv.push_back(y);
		zv.push_back(z);
		
		xerrv.push_back(xerr);
		yerrv.push_back(yerr);
		zerrv.push_back(zerr);
	}	

			TGraph2DErrors * gr = new TGraph2DErrors(xv.size(), &xv[0], &yv[0], &zv[0], &xerrv[0], &yerrv[0], &zerrv[0]);
			TF2 *fitFcn = new TF2("fitFcn", fitFunction, 260, 300, 2, 6, 3);
			
					
			fitFcn->SetParameter(0, 1E5); 
			fitFcn->SetParameter(1, 3133); 
			fitFcn->SetParameter(2, 0.442); 
				
			fitFcn->SetParLimits(0, 0, 1E10);
			fitFcn->SetParLimits(1, 1000, 5000);
			fitFcn->SetParLimits(2, 0.1, 1);

			
			gr->Fit("fitFcn", "R");	
			gr->SetMarkerColor(4);
			gr->SetMarkerStyle(kFullCircle);
			gr->Draw("APE");	
			
			//cout <<  fitFcn->GetChisquare() << endl;

			//gSystem->Sleep(3000);			
			
			c1->Update();
							
			xv.clear();
			yv.clear();	
	
	
}