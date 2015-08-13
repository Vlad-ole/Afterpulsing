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

void fit_recovery_time_tektronix(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	
	std::vector<double> xverr;
	std::vector<double> yverr;
		
	int rec_lenght = 10000;
		
	Double_t x, y, xerr, yerr;
	FILE *f = fopen(name,"r");

	int counter = 1;
	
	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf\n", &x, &y);
		xv.push_back(x*0.2); // in ns
		yv.push_back(y);
		
		//xverr.push_back(xerr*4);
		//yverr.push_back(yerr);
		
		if (xv.size() % rec_lenght == 0)
		{
			double disp = 0;
			double avg = 0;
			for (int j = 400; j < 800; j++)
			{
				avg += yv[j];
			}
			
			avg /= 800;
			
			for (int j = 400; j < 800; j++)
			{
				disp += pow(yv[j] - avg, 2.0);
			}
			
			disp /= 400;
			
			disp = sqrt(disp);
			
			cout << "disp = " << disp << endl;
			
			
			for(int j=0; j < xv.size(); j++)
			{
				xverr.push_back(0);
				yverr.push_back(disp);
			}
			
			
			TGraphErrors * gr = new TGraphErrors(xv.size(), &xv[0], &yv[0], &xverr[0], &yverr[0]);
			TF1 *fitFcn = new TF1("fitFcn", fitFunction, 950, 1500, 6);
			
			double base_line = 0;
			for(int j = 400; j < 800; j++)
			{
				base_line += yv[j];			
			}
			
			base_line /= 400;
			
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
			fitFcn->SetParameter(0, 0.012);
			fitFcn->SetParLimits(0, 0.008, 0.025); // A
			
			 //sigma
			fitFcn->SetParameter(1, 2);
			fitFcn->SetParLimits(1, 2, 8);
			//fitFcn->FixParameter(1, 0);
			
			// tau_rec
			fitFcn->SetParameter(2, 15);
			fitFcn->SetParLimits(2, 10, 30);
			
			fitFcn->SetParameter(3, 10);
			fitFcn->SetParLimits(3, 1, 15); // tau_rise
			
			fitFcn->SetParameter(4, base_line);
			fitFcn->SetParLimits(4, base_line, base_line);
			
			fitFcn->SetParameter(5, 1000);
			fitFcn->SetParLimits(5, 900, 1100); //t_0
				
			
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