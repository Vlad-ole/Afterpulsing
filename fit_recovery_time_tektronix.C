#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <TRandom.h>

using namespace std;

const int num_of_param = 6;

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
	
	//double tau_rec_slow = par[6];
	//double tau_total_slow = (tau_rec_slow * tau_rise) / (tau_rec_slow + tau_rise);
	
	//double R_slow = par[7];

	return -(A/2) * ( F(t, sigma, tau_rec_fast) - F(t, sigma, tau_total_fast) )  + V_0; 
	
	//return -(A/2) * ( F(t, sigma, tau_rec_fast) - F(t, sigma, tau_total_fast) + R_slow*(F(t, sigma, tau_rec_slow) - F(t, sigma, tau_total_slow)) )  + V_0; 

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
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	
	std::vector<double> xverr;
	std::vector<double> yverr;
		
	int rec_lenght = 9981;
		
	Double_t x, y, xerr, yerr;
	FILE *f = fopen(name,"r");

	int counter = 0;
	
	//string name_out = name + "_residue";
	//ofstream residue(name_out.c_str());
	//ofstream residue_x("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\residue_x.dat");
	//ofstream residue_y("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\residue_y.dat");
	
	//ofstream check_test("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\check_test_conv_fast.dat");
	
	
	//double par_vector[num_of_param] = {1, 1000, 20, 5, 0, 0.1};
	double x_vector[1];
	
	//TRandom	rnd;
	
	
	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf\n", &x, &y);
		xv.push_back(counter*0.2); // in ns
		yv.push_back(y);
		
		counter++;
		
		//xv.push_back(x*0.2); // in ns
		//x_vector[0] = x*0.2;
		//yv.push_back(fitFunction(x_vector, par_vector));
		//check_test << x_vector[0] << "\t" << fitFunction(x_vector, par_vector) << endl;
		
		//xverr.push_back(xerr*4);
		//yverr.push_back(yerr);
		
	}	//if (xv.size() % rec_lenght == 0)
		{
			double disp = 0;
			double avg = 0;
			for (int j = 100; j < 400; j++)
			{
				avg += yv[j] /*+ rnd.Uniform()*0.0005*/;
			}
			
			avg /= 300;
			
			for (int j = 100; j < 400; j++)
			{
				disp += pow(yv[j] - avg, 2.0);
			}
			
			disp /= 300;
			
			disp = sqrt(disp);
			
			cout << "disp = " << disp << endl;
			
			
			for(int j=0; j < xv.size(); j++)
			{
				xverr.push_back(0);
				yverr.push_back(disp);
			}
			
			double left_limit = 490;
			double right_limit = 600;
			
			TGraphErrors * gr = new TGraphErrors(xv.size(), &xv[0], &yv[0], &xverr[0], &yverr[0]);
			TF1 *fitFcn = new TF1("fitFcn", fitFunction, left_limit, right_limit, num_of_param);
			
			double base_line = 0;
			for(int j = 100; j < 400; j++)
			{
				base_line += yv[j];			
			}
			
			base_line /= 300;
			
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
			fitFcn->SetParameter(1, 500);
			fitFcn->SetParLimits(1, 490, 510); 
			
			// tau_rec
			fitFcn->SetParameter(2, 1.67762e+001);
			fitFcn->SetParLimits(2, 10, 25);

			// tau_rise
			fitFcn->SetParameter(3, 5);
			fitFcn->SetParLimits(3, 3, 18); 

			fitFcn->SetParameter(4, base_line);
			fitFcn->SetParLimits(4, base_line, base_line);
			
		
			//sigma
			fitFcn->SetParameter(5, 1.5);
			fitFcn->SetParLimits(5, 1.5, 1.5);
			//fitFcn->FixParameter(1, 0);
				
			/*	
			//tau_rec_slow
			fitFcn->SetParameter(6, 38);
			fitFcn->SetParLimits(6, 27, 80); 
			

			//R_slow
			fitFcn->SetParameter(7, 0.1);
			fitFcn->SetParLimits(7, 0, 5); 
			
				
			
				
			fitFcn->SetParameter(8, 1);
			fitFcn->SetParLimits(8, 1, 1); //R_fast
			*/
			
			gr->Fit("fitFcn", "RM");	
			gr->SetMarkerColor(4);
			gr->SetMarkerStyle(kFullCircle);
			//gr->Draw("APE");
			gr->Draw("APE");				

			//gSystem->Sleep(3000);			
			
			int left_point = left_limit / 0.2;
			int right_point = right_limit / 0.2;
			
			double par_vector[num_of_param];
			
			for (int j = 0; j < num_of_param; j++)
			{
				par_vector[j] = fitFcn->GetParameter(j);
			}
						
			/*
			for (int j = left_point; j < right_point; j++)
			{
				x_vector[0] = xv[j];
				residue_x << xv[j] << "\t" <<  yv[j] - fitFunction(x_vector, par_vector) << endl;
				residue_y << yv[j] << "\t" <<  yv[j] - fitFunction(x_vector, par_vector) << endl;
			}
			
			double square = 0;
			for(int j = 0; j < xv.size(); j ++)
			{
				square += 0.2 * yv[j];
			}
			
			cout << "square = " << square << endl;
			*/
			
			c1->Update();
							
			xv.clear();
			yv.clear();
			
			xverr.clear();
			yverr.clear();
		}
		
		
	//}	
	
}