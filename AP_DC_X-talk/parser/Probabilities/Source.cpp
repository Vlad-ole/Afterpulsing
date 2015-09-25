#include <iostream>
#include <fstream>
#include <vector>

#include "TMath.h"
#include "TRandom.h"

using namespace std;

//плотность вероятности временных интервалов при учете быстрых послеимпульсов и темновых токов
double f_dt_dc_fast(double t)
{
	double lam_f = 0.00000001;
	double nu_f = 1 / 100.0;
	double nu_dc = 1 * 1E-3;

	double h_f = exp(-t * nu_f);

	double p_f = exp(-lam_f) / (1 - exp(-lam_f)) * lam_f * nu_f * h_f * exp(lam_f * h_f);
	double p_f_integral = exp(-lam_f) / (1 - exp(-lam_f)) * (exp(lam_f * h_f) - 1);

	double p_dc = nu_dc * exp(-t* nu_dc);
	double p_dc_integral = exp(-t* nu_dc);

	return p_f * p_dc_integral + p_f_integral * p_dc;
}

double p_a(double t)
{
	double lam_f = 1;
	double nu_f = 1 / 50.0;
	double h_f = exp(- t * nu_f);
	
	return (exp(-lam_f) * (lam_f * nu_f * h_f * exp(lam_f * h_f))) / (1 - exp(-lam_f));
}

double f_exp(double t)
{
	double tau = 5;
	return 1 / tau * exp( - t / tau);
}

double Get_dt()
{
	double x;
	double a = 0;
	double b = 1000;
	double f_max = f_dt_dc_fast(0);

	while (true)
	{
		double k1 = gRandom->Uniform(0, 1);
		double k2 = gRandom->Uniform(0, 1);

		x = a + k1* (b - a);
		double y = k2 * f_max;

		if (y <= f_dt_dc_fast(x))
		{
			break;
		}

	}

	return x;
}

int main()
{
	
	ofstream file_out("D:\\Data_work\\probabilities.txt");
	double temp = 0;

	double N = 1000;

	for (double i = 0; i < N; i += 0.01)
	{
		//file_out << Get_dt() << endl;

		if (int(i) % 200 == 0)
		{
			cout << "progress is " << i / N * 100 << endl;
		}

		file_out << i << "\t" << f_dt_dc_fast(i) << endl;
		temp += f_dt_dc_fast(i);
	}

	cout << temp * 0.01 << endl;

	system("pause");
	return 0;
}
