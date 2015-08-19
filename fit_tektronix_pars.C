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

double fitFunction_2(Double_t *x,Double_t *par)
{
	return fitFunction(x,par) + fitFunction(x, &par[6]);
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

void fit_tektronix_pars(char name[])
{
	//bool draw_ = false;
	
	//if(draw_)
	//{
		//TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
		//c1->SetGrid();
	//}
	
	TGraphErrors * gr;
	
	TObjArray Hlist_all(0); // create an array
	TObjArray Hlist_chi2_fnc1(0);
	TObjArray Hlist_chi2_fnc2(0);
	
	TNtuple *ntuple = new TNtuple("ntuple","data from ascii file","x:y");
	
	
	std::vector<double> xv;
	std::vector<double> yv;
	
	std::vector<double> xverr;
	std::vector<double> yverr;
		
		
	vector<double> yv_der;	
		
	//int rec_fit_ratio  = 1;
	//int fit_lenght = 10000 * 5;
	int rec_lenght = 1000000;
		
	Double_t x, y, xerr, yerr;
	FILE *f = fopen(name,"r");

	int counter = 0;
	int counter_rec_length = 0;
	
	//string name_out = name + "_residue";
	//ofstream residue(name_out.c_str());
	ofstream amp("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\amp.dat");
	ofstream ti("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\ti.dat");
	ofstream chi2("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\chi2.dat");
	
	ofstream amp_chi2("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\amp_chi2.dat");
	ofstream ti_chi2("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\ti_chi2.dat");
	ofstream amp_ti("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\amp_ti.dat");
	
	
	ofstream amp_chi2_fnc1("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\amp_chi2_fnc1.dat");
	ofstream amp_chi2_fnc2("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\amp_chi2_fnc2.dat");
		
	
	double disp = 0.00113151;
	double base_line;
	
	double left_limit;
	double right_limit;
	int vector_index;
	
	const int step = 20;
	
	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf\n", &x, &y);
		xv.push_back(counter*0.2); // in ns
		yv.push_back(y);
		
		counter++;
		
		
		if(counter % 100000 == 0)
		{
			cout << "read file " << counter / double (2E7) * 100 << endl;
		}
		
		
	//} //while			
	
		if (xv.size() % rec_lenght == 0)
		{
			//cout << "xv.size() = " << xv.size() << endl;
			
			for(int j=0; j < xv.size(); j++)
			{
				xverr.push_back(0);
				yverr.push_back(disp);
			}
			
				
			//--------------------------------------------------------------------
			//caclulate derivative
			for (int i = 0; i < (xv.size() - step); i++)
			{
				yv_der.push_back(yv[i + step] - yv[i]);
			}
			
			bool flag = 1;
			double threshold_der = -0.006;
			
			const int time_pre = (20 / 0.2);
			const int time_post = (100 / 0.2);
			int x_time;
			
			for (int i = 0; i < (xv.size() - step); i++)
			{
						
				if ((yv_der[i] < threshold_der) && flag && (i > time_pre) && ((i + time_post) < (xv.size() - step)))
				{
					gr = new TGraphErrors(time_pre + time_post, &xv[i-time_pre], &yv[i-time_pre], &xverr[i-time_pre], &yverr[i-time_pre]);
					TF1 *fitFcn = new TF1("fitFcn", fitFunction, xv[i-time_pre], xv[i+time_post], num_of_param);
					
					cout << "*************** " << endl;
					
					
					
					//gSystem->Sleep(3000);	
					
					//convolution
					fitFcn->SetParameter(0, 0.012);
					fitFcn->SetParLimits(0, 0.001, 1000); // A
					
					//t_0
					fitFcn->SetParameter(1, (xv[i-time_pre] + xv[i+time_post])/2.0 );
					fitFcn->SetParLimits(1, xv[i-time_pre], xv[i+time_post]); 
					
					// tau_rec
					fitFcn->SetParameter(2, 17.7373);
					fitFcn->SetParLimits(2, 17.7373, 17.7373);

					// tau_rise
					fitFcn->SetParameter(3, 10.5194);
					fitFcn->SetParLimits(3, 10.5194, 10.5194);
					
					base_line = -3.91000e-004;
					fitFcn->SetParameter(4, base_line);
					fitFcn->SetParLimits(4, base_line, base_line);
					
				
					//sigma
					fitFcn->SetParameter(5, 1.64932);
					fitFcn->SetParLimits(5, 1.64932, 1.64932);
					//fitFcn->FixParameter(1, 0);
					
					gr->Fit("fitFcn", "R");	
					gStyle->SetOptFit();
					//TPaveStats *st = (TPaveStats*)gr->GetListOfFunctions()->FindObject("stats")
					
					gr->SetMarkerColor(4);
					gr->SetMarkerStyle(kFullCircle);
					
					/*
					if(draw_)
					{
						gr->Draw("APE");
						c1->Update();
					}
					*/
					
					Hlist_all.Add(gr);
					
					if(fitFcn->GetChisquare() > 5000 && fitFcn->GetParameter(0) > 0.01)
					{
						Hlist_chi2_fnc1.Add(gr);
					}

					amp_chi2_fnc1 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() << endl;
					
					if(fitFcn->GetChisquare() > 5000 && fitFcn->GetParameter(0) > 0.01)
					{
						TF1 *fitFcn_fnc2 = new TF1("fitFcn_fnc2", fitFunction_2, xv[i-time_pre], xv[i+time_post], num_of_param*2);
						
						// A
						fitFcn_fnc2->SetParameter(0, 0.012);
						fitFcn_fnc2->SetParLimits(0, 0.001, 1000); 
						
						fitFcn_fnc2->SetParameter(6, 0.012);
						fitFcn_fnc2->SetParLimits(6, 0.001, 1000); 
						
						//t_0
						fitFcn_fnc2->SetParameter(1, (xv[i-time_pre] + xv[i+time_post])/2.0 );
						fitFcn_fnc2->SetParLimits(1, xv[i-time_pre], xv[i+time_post]);
						
						fitFcn_fnc2->SetParameter(7, (xv[i-time_pre] + xv[i+time_post])/2.0 );
						fitFcn_fnc2->SetParLimits(7, xv[i-time_pre], xv[i+time_post]);
						
						// tau_rec
						fitFcn_fnc2->SetParameter(2, 17.7373);
						fitFcn_fnc2->SetParLimits(2, 17.7373, 17.7373);
						
						fitFcn_fnc2->SetParameter(8, 17.7373);
						fitFcn_fnc2->SetParLimits(8, 17.7373, 17.7373);
						
						// tau_rise
						fitFcn_fnc2->SetParameter(3, 10.5194);
						fitFcn_fnc2->SetParLimits(3, 10.5194, 10.5194);
						
						fitFcn_fnc2->SetParameter(9, 10.5194);
						fitFcn_fnc2->SetParLimits(9, 10.5194, 10.5194);
						
						base_line = -3.91000e-004;
						fitFcn_fnc2->SetParameter(4, base_line);
						fitFcn_fnc2->SetParLimits(4, base_line, base_line);
						
						fitFcn_fnc2->SetParameter(10, base_line);
						fitFcn_fnc2->SetParLimits(10, base_line, base_line);
						
						//sigma
						fitFcn_fnc2->SetParameter(5, 1.64932);
						fitFcn_fnc2->SetParLimits(5, 1.64932, 1.64932);
						
						fitFcn_fnc2->SetParameter(11, 1.64932);
						fitFcn_fnc2->SetParLimits(11, 1.64932, 1.64932);
						
						gr->Fit("fitFcn_fnc2", "R");	
						
						Hlist_chi2_fnc2.Add(gr);
						
						amp_chi2_fnc2 << fitFcn_fnc2->GetParameter(0) << "\t" << fitFcn_fnc2->GetChisquare() << endl;
						
					}
					
					//amp << fitFcn->GetParameter(0) << endl;
					//ti << fitFcn->GetParameter(1) << endl;
					//chi2 << fitFcn->GetChisquare() << endl;						
					
					//amp_ti << fitFcn->GetParameter(0) << "\t" << fitFcn->GetParameter(1) << endl;
					//amp_chi2 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() << endl;
					//ti_chi2 << fitFcn->GetParameter(1) << "\t" << fitFcn->GetChisquare() << endl;
					

					
										
					
					x_time = xv[i];
					flag = 0;
				}

				if (yv_der[i] > threshold_der && flag == 0 && ((xv[i] - x_time) > time_post*0.2))
				{
					flag = 1;
				}				
				
			}
			//--------------------------------------------------------------------
			
			/*
			for (int k = 0; k < rec_fit_ratio; k++)
			{
								
				left_limit = fit_lenght * k + counter_rec_length * rec_lenght;
				right_limit = fit_lenght * (k + 1) + counter_rec_length * rec_lenght;
				
				vector_index = fit_lenght * k;
				
				cout << left_limit*0.2 << "\t" << right_limit*0.2 << endl;
				
				TGraphErrors * gr = new TGraphErrors(fit_lenght, &xv[vector_index], &yv[vector_index], &xverr[vector_index], &yverr[vector_index]);
				TF1 *fitFcn = new TF1("fitFcn", fitFunction, left_limit*0.2, right_limit*0.2, num_of_param);
				
				
				//double base_line = 0;
				//for(int j = 400; j < 800; j++)
				//{
				//	base_line += yv[j];			
				//}				
				//base_line /= 400;
				
				
				//convolution
				fitFcn->SetParameter(0, 0.012);
				fitFcn->SetParLimits(0, 0.001, 1000); // A
				
				//t_0
				fitFcn->SetParameter(1, (left_limit*0.2 + right_limit*0.2)/2.0 );
				fitFcn->SetParLimits(1, left_limit*0.2, right_limit*0.2); 
				
				// tau_rec
				fitFcn->SetParameter(2, 17.7373);
				fitFcn->SetParLimits(2, 17.7373, 17.7373);

				// tau_rise
				fitFcn->SetParameter(3, 10.5194);
				fitFcn->SetParLimits(3, 10.5194, 10.5194);
				
				base_line = -3.91000e-004;
				fitFcn->SetParameter(4, base_line);
				fitFcn->SetParLimits(4, base_line, base_line);
				
			
				//sigma
				fitFcn->SetParameter(5, 1.64932);
				fitFcn->SetParLimits(5, 1.64932, 1.64932);
				//fitFcn->FixParameter(1, 0);
				
				gr->Fit("fitFcn", "RQ");	
				
				
				gr->SetMarkerColor(4);
				gr->SetMarkerStyle(kFullCircle);
				gr->Draw("APE");				

				//gSystem->Sleep(500);			
			
				amp_ti << fitFcn->GetParameter(0) << "\t" << fitFcn->GetParameter(1) << endl;
				amp_chi2 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() << endl;

				ti_chi2 << fitFcn->GetParameter(1) << "\t" << fitFcn->GetChisquare() << endl;
				chi2 << fitFcn->GetChisquare() << endl;
				
				c1->Update();
			}
			*/			
							
			xv.clear();
			yv.clear();
			
			xverr.clear();
			yverr.clear();
			
			yv_der.clear();
			
			//break;
			
			counter_rec_length++;
			cout << "counter_rec_length = " << counter_rec_length << endl;
			
		}
		
		if( counter_rec_length == 1)
			break;
		
	} //while	
		
	// Open a file, save the ntuple and close the file
    TFile ofile("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\Hlist_all.root","RECREATE");
    Hlist_all.Write();
    ofile.Close();
	
	TFile ofile("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\Hlist_chi2_fnc1.root","RECREATE");
	Hlist_chi2_fnc1.Write();
	ofile.Close();
	
	TFile ofile("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\Hlist_chi2_fnc2.root","RECREATE");
	Hlist_chi2_fnc2.Write();
	ofile.Close();
	
}