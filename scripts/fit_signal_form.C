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


void fit_signal_form(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	
	FILE *f = fopen(name,"r");
	
	while (!feof(f))
	{ 
		Double_t x, y;
		fscanf(f,"%lf %lf\n", &x, &y);
		xv.push_back(x*1E9); // in ns
		yv.push_back(y);
	}
	
	TGraph * gr = new TGraph(xv.size(), &xv[0], &yv[0]);
	TF1 *fitFcn = new TF1("fitFcn", fitFunction, -100, 200, 6);
	
	double base_line = 0;
	int conter = 0;
	for(int j = 0; j < xv.size(); j++)
	{
		if(xv[j] > -100 && xv[j] < -50)
		{
			base_line += yv[j];
			conter++;
		}
	}
	
	base_line /= conter;
	cout << "base_line = " << base_line << endl;
	
	fitFcn->SetParLimits(0, 0, 0.1); // A
	
	
	fitFcn->SetParLimits(1, 0, 0.1); //sigma
	//fitFcn->SetParLimits(1, -10, 0); //t_0
	
	fitFcn->SetParLimits(2, 51, 52); // tau_rec
	fitFcn->SetParLimits(3, 4, 5); // tau_rise
	
	fitFcn->SetParameter(4, base_line);
	fitFcn->SetParLimits(4, base_line, base_line);
	
	fitFcn->SetParLimits(5, -10, 0); //t_0
	
	
	
	gr->Fit("fitFcn", "R");	
	gr->SetMarkerColor(4);
	gr->SetMarkerStyle(kFullCircle);
	gr->Draw("AP");	
	
	c1->Update();
}