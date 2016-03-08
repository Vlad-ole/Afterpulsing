#include "Functions.h"
#include <vector>

using namespace std;

Functions::Functions()
{
}


Functions::~Functions()
{
}

double* Functions::x_talk_factor(double p, double PDE)
{
	const double q = 1 - p;
	vector<double> P;
	P.push_back(pow(q, 4.0));
	P.push_back(4 * p * pow(q, 6.0));
	P.push_back(18 * pow(p, 2.0) * pow(q, 8.0));
	P.push_back(4 * pow(p, 3.0) * pow(q, 8.0) * (1 + 3 * q + 18 * pow(q, 2.0)));
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


	const double Var = E_x2 - pow(E_x, 2.0);

	double array[2] = { E_x, Var };

	return array;
}

double Functions::afp_factor(double p_s, double p_f, double a_s, double a_f)
{
	const double E_x = (1 - p_s - p_f) + p_s / (1 + a_s) + p_f / (1 + a_f);

	double E_x2;
	if (a_s != 0 && a_f != 0)
	{
		E_x2 = (1 - p_s - p_f) + p_s * a_s * (2 * tgamma(a_s) / (tgamma(3 + a_s))) + p_f * a_f * (2 * tgamma(a_f) / (tgamma(3 + a_f)));
	}
	else
	{
		E_x2 = (1 - p_f) + p_f * a_f * (2 * tgamma(a_f) / (tgamma(3 + a_f)));
	}


	return (E_x2 - pow(E_x, 2.0)) / pow(E_x, 2.0);
}