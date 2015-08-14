#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/*
Double_t fitFunction(Double_t *x,Double_t *par) 
{ 
	double time = x[0] - par[1];
	if (time > 0)	
		return -par[0] * TMath::Exp(- time / par[2]) * ( 1 - TMath::Exp(- time / par[3]) ) + par[4]; 
	else
		return par[4];
}
*/

Double_t fitFunction(Double_t *x,Double_t *par) 
{ 
	return par[0]*( 1 + TMath::Erf( (x[0] - par[1]) / (par[2] * sqrt(2)) ) ) + par[3]; 
}

void fit_generator(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	
	std::vector<double> xverr;
	std::vector<double> yverr;
		
	int rec_lenght = 9983;
		
	Double_t x, y, xerr, yerr;
	FILE *f = fopen(name,"r");

	int counter = 1;
	int index = 0;
	
	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf\n", &x, &y);
		xv.push_back(index); // in ns
		yv.push_back(y);
		
		index++;
		
		//xverr.push_back(xerr*4);
		//yverr.push_back(yerr);
		
		if (xv.size() % rec_lenght == 0)
		{
			double disp = 0;
			double avg = 0;
			for (int j = 4200; j < 4800; j++)
			{
				avg += yv[j];
			}
			
			avg /= 600;
			
			for (int j = 4200; j < 4800; j++)
			{
				disp += pow(yv[j] - avg, 2.0);
			}
			
			disp /= 600;
			
			disp = sqrt(disp);
			
			cout << "error y = " << disp << endl;
			
			
			for(int j=0; j < xv.size(); j++)
			{
				xverr.push_back(0);
				yverr.push_back(disp);
			}
			
			
			TGraphErrors * gr = new TGraphErrors(xv.size(), &xv[0], &yv[0], &xverr[0], &yverr[0]);
			TF1 *fitFcn = new TF1("fitFcn", fitFunction, 4907, 4918, 4);
			
			double base_line = 0;
			for(int j = 4200; j < 4800; j++)
			{
				base_line += yv[j];			
			}
			
			base_line /= 600;
			
			
			//erf model
			fitFcn->SetParameter(0, 1); // A
			fitFcn->SetParLimits(0, 0, 1.5); 
			
			fitFcn->SetParameter(1, 1000); // t_0
			fitFcn->SetParLimits(1, 4800, 5000); 
			
			fitFcn->SetParameter(2, 1); // sigma
			fitFcn->SetParLimits(2, 0, 10); 
			
			fitFcn->SetParameter(3, base_line);
			fitFcn->SetParLimits(3, base_line, base_line);			
				
			gr->Fit("fitFcn", "R");	
			gr->SetMarkerColor(4);
			gr->SetMarkerStyle(kFullCircle);
			//gr->Draw("APE");
			gr->Draw("APE");				

			//gSystem->Sleep(3000);			

			
			c1->Update();
							
			xv.clear();
			yv.clear();
			
			xverr.clear();
			yverr.clear();
		}
		
		
	}	
	
}