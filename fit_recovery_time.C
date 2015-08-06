#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Double_t fitFunction(Double_t *x,Double_t *par) 
{ 
	double time = x[0] - par[1];
	if (time > 0)	
		return -par[0] * TMath::Exp(- time / par[2]) * ( 1 - TMath::Exp(- time / par[3]) ) + par[4]; 
	else
		return par[4];
}


void fit_recovery_time(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	
		
	int rec_lenght = 296;
		
	Double_t x, y;
	FILE *f = fopen(name,"r");

	int counter = 1;
	
	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf\n", &x, &y);
		xv.push_back(x*4); // in ns
		yv.push_back(y);
		
		
		if (xv.size() % rec_lenght == 0)
		{
			TGraph * gr = new TGraph(xv.size(), &xv[0], &yv[0]);
			TF1 *fitFcn = new TF1("fitFcn", fitFunction, 80, 200, 5);
			
			double base_line = 0;
			for(int j = 0; j < 20; j++)
			{
				base_line += yv[j];			
			}
			
			base_line /= 20;
			
			//cout << base_line << endl;
			
			
			fitFcn->SetParLimits(0, 15, 40); 
			fitFcn->SetParLimits(1, 85, 100); 
			fitFcn->SetParLimits(2, 5, 40); 
			fitFcn->SetParLimits(3, 1, 25);
			
			
			fitFcn->SetParameter(4, base_line);
			fitFcn->SetParLimits(4, base_line, base_line);
			
			
			//fitFcn->SetParameter(3, 153.622);
			//fitFcn->SetParameter(4, 502.345);
			//fitFcn->SetParameter(5, 86.9206);
			
			gr->Fit("fitFcn", "R");	
			gr->SetMarkerColor(4);
			gr->SetMarkerStyle(kFullCircle);
			gr->Draw("AP");	

			//gSystem->Sleep(3000);			
			
			c1->Update();
							
			xv.clear();
			yv.clear();
		}
		
		
	}	
	
}