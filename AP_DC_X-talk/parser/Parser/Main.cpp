#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "TRandom.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TFile.h"
//#include  "gStyle.h"

using namespace std;

const int num_of_param = 6;
const int step = 20;

int counter = 0;
int counter_rec_length = 0;
const int rec_lenght = 1000000;

const double disp = 0.00113151;
double base_line;

double left_limit;
double right_limit;
int vector_index;

double threshold_der = -0.006;

const int time_pre = (20 / 0.2);
const int time_post = (100 / 0.2);
int x_time;


double F(double t, double sigma, double tau)
{
	return TMath::Exp((sigma*sigma - 2 * t*tau) / (2 * tau*tau)) * (1 + TMath::Erf((t - sigma*sigma / tau) / (sigma * sqrt(2))));
}

Double_t fitFunction(Double_t *x, Double_t *par)
{
	double A = par[0];
	double t_0 = par[1];
	double tau_rec_fast = par[2];
	double tau_rise = par[3];
	double V_0 = par[4];

	double sigma = par[5];

	double t = x[0] - t_0;
	double tau_total_fast = (tau_rec_fast * tau_rise) / (tau_rec_fast + tau_rise);
	

	return -(A / 2) * (F(t, sigma, tau_rec_fast) - F(t, sigma, tau_total_fast)) + V_0;
}

double fitFunction_2(Double_t *x, Double_t *par)
{
	return fitFunction(x, par) + fitFunction(x, &par[6]);
}

double fitFunction_3(Double_t *x, Double_t *par)
{
	return fitFunction(x, par) + fitFunction(x, &par[6]) + fitFunction(x, &par[12]);
}


int main()
{
	char name[] = "D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\raw\\dat.txt";
	
	TObjArray Hlist_all(0); // create an array
	TObjArray Hlist_chi2_fnc1(0);
	TObjArray Hlist_chi2_fnc2(0);
	TObjArray Hlist_chi2_fnc3(0);

	vector<double> xv;
	vector<double> yv;

	vector<double> xverr;
	vector<double> yverr;

	vector<double> yv_der;

	Double_t x, y, xerr, yerr;
	FILE *f = fopen(name, "r");

	

	ofstream amp_chi2_fnc1("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\amp_chi2_fnc1.dat");
	ofstream amp_chi2_fnc2("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\amp_chi2_fnc2.dat");
	ofstream amp_chi2_fnc3("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\amp_chi2_fnc3.dat");

	

	

	while (!feof(f))
	{
		fscanf(f, "%lf %lf\n", &x, &y);
		xv.push_back(counter*0.2); // in ns
		yv.push_back(y);

		counter++;


		if (counter % 100000 == 0)
		{
			cout << "read file " << counter / double(2E7) * 100 << endl;
		}

		if (xv.size() % rec_lenght == 0)
		{
			
			for (int j = 0; j < xv.size(); j++)
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
			
			for (int i = 0; i < (xv.size() - step); i++)
			{
				if ((yv_der[i] < threshold_der) && flag && (i > time_pre) && ((i + time_post) < (xv.size() - step)))
				{
					TGraphErrors * gr = new TGraphErrors(time_pre + time_post, &xv[i - time_pre], &yv[i - time_pre], &xverr[i - time_pre], &yverr[i - time_pre]);
					TF1 *fitFcn = new TF1("fitFcn", fitFunction, xv[i - time_pre], xv[i + time_post], num_of_param);

					cout << "*************** " << endl;



					fitFcn->SetParameter(0, 0.012);
					fitFcn->SetParLimits(0, 0.001, 1000); // A

					//t_0
					fitFcn->SetParameter(1, (xv[i - time_pre] + xv[i + time_post]) / 2.0);
					fitFcn->SetParLimits(1, xv[i - time_pre], xv[i + time_post]);

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
					//gStyle->SetOptFit();
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

					if (fitFcn->GetChisquare() > 5000 && fitFcn->GetParameter(0) > 0.01)
					{
						Hlist_chi2_fnc1.Add(gr);
					}

					amp_chi2_fnc1 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() << endl;


					//delete gr;
					//-------------------------------------------------------

					if (fitFcn->GetChisquare() > 5000 && fitFcn->GetParameter(0) > 0.01)
					{
						TGraphErrors * gr_2 = new TGraphErrors(time_pre + time_post, &xv[i - time_pre], &yv[i - time_pre], &xverr[i - time_pre], &yverr[i - time_pre]);
						TF1 *fitFcn_fnc2 = new TF1("fitFcn_fnc2", fitFunction_2, xv[i - time_pre], xv[i + time_post], num_of_param * 2);

						gr_2->SetMarkerColor(4);
						gr_2->SetMarkerStyle(kFullCircle);

						// A
						fitFcn_fnc2->SetParameter(0, 0.012);
						fitFcn_fnc2->SetParLimits(0, 0.001, 1000);

						fitFcn_fnc2->SetParameter(6, 0.012);
						fitFcn_fnc2->SetParLimits(6, 0.001, 1000);

						//t_0
						fitFcn_fnc2->SetParameter(1, (xv[i - time_pre] + xv[i + time_post]) / 2.0);
						fitFcn_fnc2->SetParLimits(1, xv[i - time_pre], xv[i + time_post]);

						fitFcn_fnc2->SetParameter(7, (xv[i - time_pre] + xv[i + time_post]) / 2.0);
						fitFcn_fnc2->SetParLimits(7, xv[i - time_pre], xv[i + time_post]);

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

						gr_2->Fit("fitFcn_fnc2", "R");

						Hlist_chi2_fnc2.Add(gr_2);

						amp_chi2_fnc2 << fitFcn_fnc2->GetParameter(0) << "\t" << fitFcn_fnc2->GetChisquare() << endl;

					}









					x_time = xv[i];
					flag = 0;
				}

				if (yv_der[i] > threshold_der && flag == 0 && ((xv[i] - x_time) > time_post*0.2))
				{
					flag = 1;
				}
			}
			
			xv.clear();
			yv.clear();

			xverr.clear();
			yverr.clear();

			yv_der.clear();


			counter_rec_length++;
			cout << "counter_rec_length = " << counter_rec_length << endl;
		}

		if (counter_rec_length == 1)
			break;

	}


	// Open a file, save the ntuple and close the file
	TFile ofile_0("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\Hlist_all.root", "RECREATE");
	Hlist_all.Write();
	ofile_0.Close();

	TFile ofile_1("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\Hlist_chi2_fnc1.root", "RECREATE");
	Hlist_chi2_fnc1.Write();
	ofile_1.Close();

	TFile ofile_2("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\Hlist_chi2_fnc2.root", "RECREATE");
	Hlist_chi2_fnc2.Write();
	ofile_2.Close();

	TFile ofile_3("D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\Hlist_chi2_fnc3.root", "RECREATE");
	Hlist_chi2_fnc3.Write();
	ofile_3.Close();


	system("pause");
	return 0;	
}