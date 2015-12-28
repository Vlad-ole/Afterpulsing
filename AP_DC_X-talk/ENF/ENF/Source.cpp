#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
using namespace std;

double *x_talk_factor(double p)
{
	const double q = 1 - p;
	vector<double> P;
	P.push_back( pow(q, 4.0) );
	P.push_back(4 * p * pow(q, 6.0));
	P.push_back(18 * pow(p, 2.0) * pow(q, 8.0));
	P.push_back( 4 * pow(p, 3.0) * pow(q, 8.0) * (1 + 3 * q + 18 * pow(q, 2.0) ) );

	const double E_x = 1 * P[0] + 2 * P[1] + 3* P[2] + 4*P[3];
	const double E_x2 = 1 * P[0] + 4 * P[1] + 9 * P[2] + 16 * P[3];
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

	for (double V = 0.5; V < 1.5; V+=0.05)
	{
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

		double x_talk_factor_E = x_talk_factor(p)[0];
		double x_talk_factor_Var = x_talk_factor(p)[1];

		file_out << V << "\t" << 1 + x_talk_factor_Var / pow(x_talk_factor_E, 2.0) + afp_factor(p_s, p_f, a_s, a_f) / x_talk_factor_E << endl;
		//file_out << V << "\t" << afp_factor(p_s, p_f, a_s, a_f) << "\t" << x_talk_factor_E << endl;
		//file_out << V << "\t" << afp_factor(p_s, p_f, a_s, a_f) / x_talk_factor_E << endl;
	}

	system("pause");
	return 0;
}