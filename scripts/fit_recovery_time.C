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
	double A = par[0];
	double sigma = par[1];
	double tau_rec = par[2];
	double tau_rise = par[3];
	double V_0 = par[4];
	double t_0 = par[5];
	
	double t = x[0] - t_0;
	double tau_total = (tau_rec * tau_rise) / (tau_rec + tau_rise);
	
	double arg_1 = (pow(sigma, 2.0) - 2*t*tau_rec) / (2 * pow(tau_rec, 2.0)) ;
	double arg_2 = ( t - pow(sigma, 2.0) / tau_rec ) / ( sigma * sqrt(2) );
	double arg_3 = (pow(sigma, 2.0) - 2*t*tau_total) / (2 * pow(tau_total, 2.0));
	double arg_4 = ( t - pow(sigma, 2.0) / tau_total ) / ( sigma * sqrt(2) );
	
	//if (t > 0)	
		return -(A/2) * ( TMath::Exp( arg_1 ) * ( 1 + TMath::Erf(arg_2) )  + TMath::Exp( arg_3 ) * ( 1 + TMath::Erf(arg_4) ) ) + V_0; 
	//else
		//return V_0;
}

void fit_recovery_time(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	
	std::vector<double> xverr;
	std::vector<double> yverr;
		
	int rec_lenght = 296;
		
	Double_t x, y, xerr, yerr;
	FILE *f = fopen(name,"r");

	int counter = 1;
	
	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf %lf %lf\n", &x, &y, &xerr, &yerr);
		xv.push_back(x*4); // in ns
		yv.push_back(y);
		
		//xverr.push_back(xerr*4);
		//yverr.push_back(yerr);
		
		if (xv.size() % rec_lenght == 0)
		{
			double disp = 0;
			double avg = 0;
			for (int j = 100; j < 250; j++)
			{
				avg += yv[j];
			}
			
			avg /= 150;
			
			for (int j = 100; j < 250; j++)
			{
				disp += pow(yv[j] - avg, 2.0);
			}
			
			disp /= 150;
			
			disp = sqrt(disp);
			
			cout << "disp = " << disp << endl;
			
			
			for(int j=0; j < xv.size(); j++)
			{
				xverr.push_back(0);
				yverr.push_back(disp);
			}
			
			
			TGraphErrors * gr = new TGraphErrors(xv.size(), &xv[0], &yv[0], &xverr[0], &yverr[0]);
			TF1 *fitFcn = new TF1("fitFcn", fitFunction, 0, 250, 6);
			
			double base_line = 0;
			for(int j = 0; j < 20; j++)
			{
				base_line += yv[j];			
			}
			
			base_line /= 20;
			
			//cout << base_line << endl;
			
			/*
			fitFcn->SetParLimits(0, 15, 40); 
			fitFcn->SetParLimits(1, 85, 100); 
			fitFcn->SetParLimits(2, 5, 40); 
			fitFcn->SetParLimits(3, 1, 25);
			
			fitFcn->SetParameter(4, base_line);
			fitFcn->SetParLimits(4, base_line, base_line);
			*/
			
			//convolution
			fitFcn->SetParLimits(0, 35, 50); // A
			
			 //sigma
			fitFcn->SetParameter(1, 5);
			fitFcn->SetParLimits(1, 5, 5);
			//fitFcn->FixParameter(1, 0);
			
			fitFcn->SetParLimits(2, 15, 30); // tau_rec
			fitFcn->SetParLimits(3, 1, 8); // tau_rise
			
			fitFcn->SetParameter(4, base_line);
			fitFcn->SetParLimits(4, base_line, base_line);
			
			fitFcn->SetParLimits(5, 80, 110); //t_0
					
			
			gr->Fit("fitFcn", "R");	
			gr->SetMarkerColor(4);
			gr->SetMarkerStyle(kFullCircle);
			//gr->Draw("APE");
			gr->Draw("APE");				

			//gSystem->Sleep(3000);			
			
			/*
			for (int j =0; j < xv.size(); j++)
			{
				cout << xv[j] << "\t" << yv[j] << "\t" << xverr[j] << "\t" << yverr[j] << endl;
			}
			*/
			
			c1->Update();
							
			xv.clear();
			yv.clear();
			
			xverr.clear();
			yverr.clear();
		}
		
		
	}	
	
}