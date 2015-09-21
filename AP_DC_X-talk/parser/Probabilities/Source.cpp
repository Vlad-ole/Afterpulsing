#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

double p_a(double t)
{
	double lam_f = 1;
	double nu_f = 1 / 50.0;
	double h_f = exp(- t * nu_f);
	
	return (exp(-lam_f) * (lam_f * nu_f * h_f * exp(lam_f * h_f))) / (1 - exp(-lam_f));
}

double p_b(double t)
{
	double nu_dc = 1;
	double h_dc = exp(-t * nu_dc);

	return exp(t * nu_dc * (h_dc - 1));
}

int main()
{
	
	ofstream file_out("D:\\Data_work\\probabilities.txt");
	double temp = 0;

	for (double i = 0; i < 100; i+= 0.01)
	{
		file_out << i << "\t" << p_a(i) << endl;
		temp += p_a(i);
	}

	cout << temp * 0.01 << endl;

	system("pause");
	return 0;
}
