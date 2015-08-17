#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;


double F(double t, double sigma, double tau)
{
	return TMath::Exp( (sigma*sigma - 2*t*sigma) / (2*tau*tau) ) * ( 1 + TMath::Erf( (t - sigma*sigma/tau) / (sigma * sqrt(2)) ) );
}


Double_t fitFunction(Double_t *x,Double_t *par) 
{ 
	double A = par[0];
	double t_0 = par[1];
	double tau_rec_fast = par[2];
	double tau_rise = par[3];
	double V_0 = par[4];

	double sigma = par[5];

	double t = x[0] - t_0;
	double tau_total_fast = (tau_rec_fast * tau_rise) / (tau_rec_fast + tau_rise);
	

	return -(A/2) * ( F(t, sigma, tau_rec_fast) - F(t, sigma, tau_total_fast) )  + V_0; 

}

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
	
	//string name_out = name + "_residue";
	//ofstream residue(name_out.c_str());
	ofstream residue("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\residue.dat");
	
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
			
			double left_limit = 950;
			double right_limit = 1040;
			
			TGraphErrors * gr = new TGraphErrors(xv.size(), &xv[0], &yv[0], &xverr[0], &yverr[0]);
			TF1 *fitFcn = new TF1("fitFcn", fitFunction, left_limit, right_limit, 6);
			
			double base_line = 0;
			for(int j = 400; j < 800; j++)
			{
				base_line += yv[j];			
			}
			
			base_line /= 400;
			
			/*
			//exp model
			fitFcn->SetParameter(0, 4.74969e-002); // A
			fitFcn->SetParLimits(0, 0, 1); 
			
			fitFcn->SetParameter(1, 9.96439e+002); // t_0
			fitFcn->SetParLimits(1, 900, 1000); 
			
			fitFcn->SetParameter(2, 1.67762e+001); // tau_rec
			fitFcn->SetParLimits(2, 5, 40); 
			
			fitFcn->SetParameter(3, 1.41106e+001); // tau_rise
			fitFcn->SetParLimits(3, 1, 25);
			
			fitFcn->SetParameter(4, base_line);
			fitFcn->SetParLimits(4, base_line, base_line);
			*/
			
			
			
			//convolution
			fitFcn->SetParameter(0, 0.012);
			fitFcn->SetParLimits(0, 0.001, 0.1); // A
			
			//t_0
			fitFcn->SetParameter(1, 1000);
			fitFcn->SetParLimits(1, 990, 1010); 
			
			// tau_rec
			fitFcn->SetParameter(2, 1.67762e+001);
			fitFcn->SetParLimits(2, 2, 18);

			// tau_rise
			fitFcn->SetParameter(3, 9);
			fitFcn->SetParLimits(3, 5, 15); 

			fitFcn->SetParameter(4, base_line);
			fitFcn->SetParLimits(4, base_line, base_line);
			
			//sigma
			fitFcn->SetParameter(5, 0.7);
			fitFcn->SetParLimits(5, 0.6, 5);
			//fitFcn->FixParameter(1, 0);
					
			
			
			
			/*
			fitFcn->SetParameter(6, 40);
			fitFcn->SetParLimits(6, 40, 40); //tau_rec_slow
				
			fitFcn->SetParameter(7, 0);
			fitFcn->FixParameter(7, 0); //R_slow
			
			fitFcn->SetParameter(8, 1);
			fitFcn->SetParLimits(8, 1, 1); //R_fast
			*/
			
			gr->Fit("fitFcn", "R");	
			gr->SetMarkerColor(4);
			gr->SetMarkerStyle(kFullCircle);
			//gr->Draw("APE");
			gr->Draw("APE");				

			//gSystem->Sleep(3000);			
			
			int left_point = left_limit / 0.2;
			int right_point = right_limit / 0.2;
			
			double par_vector[6];
			double x_vector[1];
			
			for (int j = 0; j < 6; j++)
			{
				par_vector[j] = fitFcn->GetParameter(j);
			}
						
			
			for (int j = left_point; j < right_point; j++)
			{
				x_vector[0] = xv[j];
				residue << xv[j] << "\t" <<  yv[j] - fitFunction(x_vector, par_vector) << endl;
			}
			
			
			c1->Update();
							
			xv.clear();
			yv.clear();
			
			xverr.clear();
			yverr.clear();
		}
		
		
	}	
	
}