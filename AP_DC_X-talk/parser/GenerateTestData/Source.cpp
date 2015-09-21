#include <iostream>
#include <fstream>
#include "TMath.h"
#include "TRandom.h"
#include <vector>
using namespace std;


//вспомогательная функция
double F(double t, double sigma, double tau)
{
	if ((sigma*sigma - 2 * t*tau) / (2 * tau*tau) > 700)
	{
		cout << "Attention! Exp(x) in F function too big!" << endl;
		system("pause");
	}
	return TMath::Exp((sigma*sigma - 2 * t*tau) / (2 * tau*tau)) * (1 + TMath::Erf((t - sigma*sigma / tau) / (sigma * sqrt(2))));
}

//функция сигнала, без учета базавой линии
double fitFunction_nobaseline(double *x, double *par)
{
	//основные параметры
	const double A = par[0];
	const double t_0 = par[1];
	const double tau_rec_fast = par[2];
	const double tau_rise = par[3];
	const double sigma = par[4];

	//вспомагательные переменные
	const double t = x[0] - par[1];
	const double tau_total_fast = (tau_rec_fast * tau_rise) / (tau_rec_fast + tau_rise);

	return -(A / 2) * (F(t, sigma, tau_rec_fast) - F(t, sigma, tau_total_fast));
}

//функция, которой буду фитировать
Double_t fitFunction(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0;
}


int main()
{
	ofstream file_out("D:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\raw\\test_signal_pure.txt");
	ofstream file_ti("D:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\raw\\test_signal_ti_pure.txt");
	ofstream file_ti_hist("D:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\raw\\test_signal_ti_hist_pure.txt");
	//const double noise_amp = 0.001;
	

	const int cycles = 10000;
	const int signal_length = 2001;
	
	const double noise_amp = 0;
	const double ampl = 0.04;

	const int a = (signal_length - 1) / 2.0;
	
	vector<double> xv;
	vector<double> yv;
	vector<double> yv_signal;

	cout << "Calculate baseline vector ..." << endl;

	for (int i = 0; i < cycles * signal_length; i++)
	{
		yv.push_back(0.003 + noise_amp*gRandom->Uniform(-1, 1));
	}

	
	

	for (int i = 0; i < signal_length; i++)
	{
		double x = (i - a) * 0.2;
		Double_t par[5] = { ampl, 0, 17.7373, 10.5194, 1.64932 };
		yv_signal.push_back( fitFunction_nobaseline(&x, par) );
	}

	/*for (int i = 0; i < cycles; i++)
	{
	if (gRandom->Uniform() > 0.5)
	{
	par[0] = gRandom->Gaus(0.04, 0.04 * 0.1);
	}
	else
	{
	par[0] = gRandom->Gaus(0.08, 0.08 * 0.1);
	}

	par[1] = gRandom->Exp(50);


	for (double j = 0; j < period; j += 0.2)
	{
	file_out << j + i*period << "\t" << fitFunction(&j, par) << endl;
	}
	}*/


	//for (int j = 0; j < cycles; j++)
	//{
		//double shit = ;
		//par[1] += gRandom->Exp(250);

		/*int shit = 0;
		for (int i = 0; i < yv_signal.size(); i++)
		{
			if (i >= int( signal_length/2.0) )
				yv[i - signal_length / 2.0] += yv_signal[i];
		}*/

	//}
	
	
	double time_i = 0;
	double amp_fraction;

	int b = 0;
	for (int k = 0; k < cycles; k++)
	{
		bool temp_1 = 1;
		bool temp_2 = 1;
		bool temp_3 = 1;
		bool temp_4 = 1;
		if (k % 1000 == 0)
		{
			cout << "calculate ... " << double(k) / cycles * 100 << " %" << endl;
		}
		
		//double dt = 20;
		double dt = gRandom->Exp(150);
		time_i += dt;
		
		int b = time_i * 5;
		//amp_fraction = 1 - exp(-dt / 17.7373);
		amp_fraction = 1;

		file_ti << dt << "\t" << amp_fraction * ampl << endl;
		file_ti_hist << dt << endl;

		for (int i = 0, j = 0; i < cycles * signal_length; i++)
		{
			
			
			if (b <= a && j < ( signal_length - (a - b) ) )
			{
				if (b + a <= cycles * signal_length) //work
				{
					yv[i] += amp_fraction * yv_signal[i + a - b];
					j++;

					if (temp_1)
					{
						//cout << "case 1" << endl;
						temp_1 = 0;
					}
					
				}

				if ( b + a > cycles * signal_length && j < (cycles * signal_length - b) )
				{
					yv[i] += amp_fraction * yv_signal[i + a - b];
					j++;
					//cout << "case 2" << endl;
					if (temp_2)
					{
						//cout << "case 2" << endl;
						temp_2 = 0;
					}
				}
				
			}

			if (b > a && j < signal_length && i >= (b - a) )
			{
				if (b + a <= cycles * signal_length)
				{
					yv[i] += amp_fraction * yv_signal[j];
					j++;
					//cout << "case 3" << endl;
					if (temp_3)
					{
						//cout << "case 3" << endl;
						temp_3 = 0;
					}
				}

				if (b + a > cycles * signal_length && j < (cycles * signal_length - (b - a) ) ) //work
				{
					yv[i] += amp_fraction * yv_signal[j];
					j++;
					//cout << "case 4" << endl;
					if (temp_4)
					{
						//cout << "case 4" << endl;
						temp_4 = 0;
					}
				}

			}

			

			///////
			//if (i > b - a && i < b + a)
			//	{
			//		yv[i] += amp_fraction * yv_signal[j]; //+ noise_amp*gRandom->Uniform(-1, 1);
			//		j++;
			//	}		
			//
			//	if (i < a + b)
			//	{
			//		yv[i] += amp_fraction * yv_signal[j + a + abs(b)];// +noise_amp*gRandom->Uniform(-1, 1);
			//		j++;
			//	}
			
		}
	}

	cout << "write in file..." << endl;
	for (int i = 0; i < cycles * signal_length; i++)
	{

		file_out << i*0.2 << "\t" << yv[i] << endl;
	}

	system("pause");
	return 0;
}