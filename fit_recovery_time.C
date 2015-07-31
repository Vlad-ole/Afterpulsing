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

Double_t fitFunction_summ(Double_t *x, Double_t *par) 
{ 
	return fitFunction(x, par) + fitFunction(x, &par[3]) + fitFunction(x, &par[6]); 
}

void fit_1e(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	
	string  directory_init = "D:\\Data_work\\afterpulsing\\275T_71.00V_ham_33_25\\1e\\";	
	string directory_amp = directory_init + "amp.dat";
	string directory_t0 = directory_init + "t0.dat";
	string directory_tau_d = directory_init + "tau_d.dat";
	string directory_tau_r = directory_init + "tau_r.dat";
	string directory_y0 = directory_init + "y0.dat";
	string directory_chi2 = directory_init + "chi2.dat";
	string directory_ndf = directory_init + "ndf.dat";
	
	ofstream file_amp(directory_amp.c_str());
	ofstream file_t0(directory_t0.c_str());
	ofstream file_tau_d(directory_tau_d.c_str());
	ofstream file_tau_r(directory_tau_r.c_str());
	ofstream file_y0(directory_y0.c_str());
	ofstream file_chi2(directory_chi2.c_str());
	ofstream file_ndf(directory_ndf.c_str());
	
	int rec_lenght = 40;
		
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
			cout << "part " << counter << " was started" << endl;
			counter++;
			
			TGraph * gr = new TGraph(xv.size(), &xv[0], &yv[0]);
			TF1 *fitFcn = new TF1("fitFcn", fitFunction, 0, 160, 5);
			
			double base_line = 0;
			for(int j = 30; j < 40; j++)
			{
				base_line += yv[j];			
			}
			
			base_line /= 10;
			
			//cout << base_line << endl;
			
			fitFcn->SetParLimits(0, 10, 50); 
			fitFcn->SetParLimits(1, 25, 100); 
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

			
			
			file_amp <<  fitFcn->GetParameter(0) << endl;
			file_t0 <<  fitFcn->GetParameter(1) << endl;
			file_tau_d <<  fitFcn->GetParameter(2) << endl;
			file_tau_r <<  fitFcn->GetParameter(3) << endl;
			file_y0 <<  fitFcn->GetParameter(4) << endl;
			file_chi2 <<  fitFcn->GetChisquare() << endl;
			file_ndf <<  fitFcn->GetNDF() << endl;
			
			
			xv.clear();
			yv.clear();
		}
		
	}	
	
}