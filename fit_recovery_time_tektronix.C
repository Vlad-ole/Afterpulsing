#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <TRandom.h>

using namespace std;

const int num_of_param = 8;

double F(double t, double sigma, double tau)
{
	return TMath::Exp( (sigma*sigma - 2*t*tau) / (2*tau*tau) ) * ( 1 + TMath::Erf( (t - sigma*sigma/tau) / (sigma * sqrt(2)) ) );
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
	


	//return -(A/2) * ( F(t, sigma, tau_rec_fast) - F(t, sigma, tau_total_fast) )  + V_0; 
	
	double tau_rec_slow = par[6];
	double tau_total_slow = (tau_rec_slow * tau_rise) / (tau_rec_slow + tau_rise);
	
	double R_slow = par[7];
	
	return -(A/2) * ( (F(t, sigma, tau_rec_fast) - F(t, sigma, tau_total_fast)) + R_slow*(F(t, sigma, tau_rec_slow) - F(t, sigma, tau_total_slow)) )  + V_0; 

}

/*
Double_t fitFunction(Double_t *x,Double_t *par) 
{ 
	double time = x[0] - par[1];
	double A = par[0];
	double tau_rec_fast = par[2];
	double tau_rise = par[3];
	double tau_total_fast = (tau_rec_fast * tau_rise) / (tau_rec_fast + tau_rise);
	
	if (time > 0)	
		return - A / (tau_rec_fast - tau_total_fast) * TMath::Exp(- time / tau_rec_fast) * ( 1 - TMath::Exp(- time / tau_rise) ) + par[4]; 
	else
		return par[4];
}
*/

void fit_recovery_time_tektronix(char name[])
{
	ROOT::Math::MinimizerOptions::SetDefaultStrategy(2);
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Migrad");
	
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	
	std::vector<double> xverr;
	std::vector<double> yverr;

	Double_t x, y, xerr, yerr;
	FILE *f = fopen(name,"r");

	int counter = 0;
	
	//string name_out = name + "_residue";
	//ofstream residue(name_out.c_str());
	//ofstream residue_x("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\residue_x.dat");
	//ofstream residue_y("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\residue_y.dat");
	
	//ofstream check_test("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\check_test_conv_fast.dat");

	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf\n", &x, &y);
		xv.push_back(counter * 0.2); // in ns
		yv.push_back(y);
		
		counter++;
		
	}	
	
		double time_baseline_left = 100;
		double time_baseline_right = 150;
		
		int time_baseline_left_index = time_baseline_left * 5;
		int time_baseline_right_index = time_baseline_right * 5;
		
		double disp = 0;
		double avg = 0;
		for (int j = time_baseline_left_index; j < time_baseline_right_index; j++)
		{
			avg += yv[j] /*+ rnd.Uniform()*0.0005*/;
		}
		
		avg /= (time_baseline_right_index - time_baseline_left_index);
		
		for (int j = time_baseline_left_index; j < time_baseline_right_index; j++)
		{
			disp += pow(yv[j] - avg, 2.0);
		}
		
		disp /= (time_baseline_right_index - time_baseline_left_index);
		
		disp = sqrt(disp);
		
		cout << "disp = " << disp << endl;
		
		
		for(int j=0; j < xv.size(); j++)
		{
			xverr.push_back(0);
			yverr.push_back(disp);
		}
		
		double base_line = 0;
		for(int j = time_baseline_left_index; j < time_baseline_right_index; j++)
		{
			base_line += yv[j];			
		}
		
		base_line /= (time_baseline_right_index - time_baseline_left_index);
		
		cout << endl << "baseline " << base_line << endl;
		
		
		double left_limit = 195;
		double right_limit = 350;
		vector<double> chi_per_nfd;
		ofstream file_out("G:\\Chi2_per_dof.dat");
		

			TGraphErrors * gr = new TGraphErrors(xv.size(), &xv[0], &yv[0], &xverr[0], &yverr[0]);
			
			
			TF1 *fitFcn = new TF1("fitFcn", fitFunction, left_limit, right_limit, num_of_param);
					
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
			fitFcn->SetParLimits(0, 0.001, 1000); // A
			
			//t_0
			fitFcn->SetParameter(1, 200);
			fitFcn->SetParLimits(1, 150, 210); 
			
			// tau_rec_fast
			fitFcn->SetParameter(2, 10);
			fitFcn->SetParLimits(2, 0.1, 30);

			double rise = 5;
			// tau_rise
			fitFcn->SetParameter(3, rise);
			fitFcn->SetParLimits(3, 1, 35); 

			//base_line = -1e-006;
			//baseline
			fitFcn->SetParameter(4, base_line);
			fitFcn->SetParLimits(4, base_line, base_line);
			
			double sigma = 2.5;
			//sigma
			fitFcn->SetParameter(5, sigma);
			fitFcn->SetParLimits(5, 0.1, 5);
			//fitFcn->SetParLimits(5, sigma, sigma);
			//fitFcn->FixParameter(1, 0);
				
			
			//tau_rec_slow
			fitFcn->SetParameter(6, 39);
			fitFcn->SetParLimits(6, 38, 45); 
			

			//R_slow
			fitFcn->SetParameter(7, 0.25);
			fitFcn->SetParLimits(7, 0.1, 0.4); 
	
						
			gr->Fit("fitFcn", "R");
			//chi_per_nfd	.push_back(fitFcn->GetChisquare() / fitFcn->GetNDF());			
			//cout << "Chi2 / Ndf  = " << fitFcn->GetChisquare() / fitFcn->GetNDF() << "right_limit " << i << endl;
			cout << "Chi2 / Ndf  = " << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
			//file_out << i << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;

		gr->SetMarkerColor(4);
		gr->SetMarkerStyle(kFullCircle);
		gr->Draw("APE");				

		
		
		c1->Update();
						
		xv.clear();
		yv.clear();
		
		xverr.clear();
		yverr.clear();
		
		
		
	//}	
	
}