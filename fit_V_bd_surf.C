#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Double_t fitFunction(Double_t *x, Double_t *par) 
{ 
	double T = x[0];
	double V = x[1];
	
	double a = par[0];
	double b = par[1];
	double c = par[2];
	
	//return par[0] * pow(x[0], 1.5) * TMath::Exp(- par[1] / x[0] ) * TMath::Exp( par[2] * x[1] );  // exp
	return a*V + b*T + c;
}


void fit_V_bd_surf(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	std::vector<double> zv;
	
	std::vector<double> xerrv;
	std::vector<double> yerrv;
	std::vector<double> zerrv;
	
	Double_t x, y, z;
	FILE *f = fopen(name,"r");
	
	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf %lf \n", &x, &y, &z);
		xv.push_back(x); 
		yv.push_back(y);
		zv.push_back(z);
		
		
	}	

			for (int i = 0; i < xv.size(); i++)
			{
				xerrv.push_back(0.29);
				yerrv.push_back(0.01);
				zerrv.push_back(1);
			}
			
			
			TGraph2DErrors * gr = new TGraph2DErrors(xv.size(), &xv[0], &yv[0], &zv[0], &xerrv[0], &yerrv[0], &zerrv[0]);
			TF2 *fitFcn = new TF2("fitFcn", fitFunction, 260, 300, 68, 75, 3);
	
			
			fitFcn->SetParameter(0, 4.80590e+001); 
			fitFcn->SetParameter(1, -1.97756e+000); 
			fitFcn->SetParameter(2, -2.74847e+003); 

			
			fitFcn->SetParLimits(0, 1, 100);
			fitFcn->SetParLimits(1, -10, 0);
			fitFcn->SetParLimits(2, -10000, -100);
			
			
			
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