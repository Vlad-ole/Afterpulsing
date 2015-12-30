#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
using namespace std;

double *x_talk_factor(double p, double PDE)
{
	const double q = 1 - p;
	vector<double> P;
	P.push_back( pow(q, 4.0) );
	P.push_back(4 * p * pow(q, 6.0));
	P.push_back(18 * pow(p, 2.0) * pow(q, 8.0));
	P.push_back( 4 * pow(p, 3.0) * pow(q, 8.0) * (1 + 3 * q + 18 * pow(q, 2.0) ) );
	P.push_back(5 * pow(p, 4.0) * pow(q, 10.0) * (8 + 24 * q + 55 * pow(q, 2.0)));

	const double summ = P[0] + P[1] + P[2] + P[3];
	for (int N = 6; N < 100; N++)
	{
		const double value = P[4] * pow(1 - P[4] / (1 - summ), N - 5);
		P.push_back(value);
	}

	double E_x = 0;
	double E_x2 = 0;
	for (int i = 0; i < P.size(); i++)
	{
		E_x += PDE * (i + 1) * P[i];
		E_x2 += PDE * pow(i + 1, 2.0) * P[i];
	}

	//const double E_x = PDE * (1 * P[0] + 2 * P[1] + 3 * P[2] + 4 * P[3]);
	//const double E_x2 = PDE * (1 * P[0] + 4 * P[1] + 9 * P[2] + 16 * P[3]);
	const double Var = E_x2 - pow(E_x , 2.0);
	
	double array[2] = { E_x, Var };

	return array;
	//return Var;
}

double afp_factor(double p_s, double p_f, double a_s, double a_f)
{
	const double E_x = (1 - p_s - p_f) + p_s / (1 + a_s) + p_f / (1 + a_f);
	const double E_x2 = (1 - p_s - p_f) + p_s * a_s * (2 * tgamma(a_s) / (tgamma(3 + a_s))) + p_f * a_f * (2 * tgamma(a_f) / (tgamma(3 + a_f)));

	return ( E_x2 - pow(E_x, 2.0) ) / pow(E_x, 2.0);
}

int main()
{
	ofstream file_out("D:\\git_repositories\\Afterpulsing\\AP_DC_X-talk\\ENF\\out.dat");
	bool line = false;

	for (double V = 0.1; V < 3.5; V+=0.01)
	{
		//PQE
		const double PDE = 0.65 * (1 - exp(-0.56*V));
		
		//x-talk
		double P_xtalk;

		if (line)
		{
			P_xtalk = 0.15*V - 0.03;
		}
		else
		{
			P_xtalk = 0.10349*V*V;
		}		

		const double p = 1 - pow(1 - P_xtalk, 0.25);

		double x_talk_factor_E = x_talk_factor(p, PDE)[0];
		double x_talk_factor_Var = x_talk_factor(p, PDE)[1];

		//apf
		double p_f;
		double p_s;
		if (line)
		{
			p_f = 0.26*V - 0.12;
			p_s = 0.21*V - 0.07;
		}
		else
		{
			p_f = 0.12602*V*V;
			p_s = 0.14813*V*V;
		}	
				
		const double tau_f = 35;
		const double tau_s = 170;
		const double tau_rec = 40;
		const double a_s = tau_rec / tau_s;
		const double a_f = tau_rec / tau_f;



		//Energy resolution
		const double N_init = 3.54;

		const double ER_born = 1 / N_init + (x_talk_factor_Var / pow(x_talk_factor_E, 2.0)) * (1 / N_init) /*+ afp_factor(p_s, p_f, a_s, a_f) / (N_init * x_talk_factor_E)*/;

		file_out << V << "\t" << sqrt(ER_born) << endl;
		//file_out << V << "\t" << P_xtalk << endl;


		//file_out << V << "\t" << 1 /*+ x_talk_factor_Var / pow(x_talk_factor_E, 2.0)*/ + afp_factor(p_s, p_f, a_s, a_f) / x_talk_factor_E  << endl;
		//file_out << V << "\t" << afp_factor(p_s, p_f, a_s, a_f) << "\t" << x_talk_factor_E << endl;
		//file_out << V << "\t" << afp_factor(p_s, p_f, a_s, a_f) / x_talk_factor_E << endl;
	}

	system("pause");
	return 0;
}