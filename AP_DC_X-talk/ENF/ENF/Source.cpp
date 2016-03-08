#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "Functions.h"

using namespace std;



int main()
{
	ofstream file_out("D:\\git_repositories\\Afterpulsing\\AP_DC_X-talk\\ENF\\out.dat");
	bool line = false;

	//for (double V = 0.16; V < 10.0; V+=0.01)
	double V = 4.5;
	{
		//PQE
		const double PDE = 0.5056 * (1 - exp(-0.40539*V));




		//x-talk
		double P_xtalk;

		if (line)
		{
			P_xtalk = 0.15*V - 0.03;
		}
		else
		{
			P_xtalk = 0.00392*V*V;
		}		

		const double p = 1 - pow(1 - P_xtalk, 0.25);

		double x_talk_factor_E = Functions::x_talk_factor(p, PDE)[0];
		double x_talk_factor_Var = Functions::x_talk_factor(p, PDE)[1];
		




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
			p_f = 0.00658*V*V;
			p_s = 0*V*V;
		}	
				
		const double tau_f = 8.90841;
		const double tau_s = 0;
		const double tau_rec = 51;
		const double a_s = /*tau_rec / tau_s */ 0;
		const double a_f = tau_rec / tau_f;


		//cout characteristic
		cout << "PDE = " << PDE << endl;
		cout << "P_xtalk = " << P_xtalk << endl;
		cout << "p_f = " << p_f << endl;
		cout << "p_s = " << p_s << endl;
		cout << "tau_f = " << tau_f << endl;
		cout << "tau_rec = " << tau_rec << endl;


		//Energy resolution
		//const double N_init = 16.543;

		//const double ER_born = 1 / N_init + (x_talk_factor_Var / pow(x_talk_factor_E, 2.0)) * (1 / N_init) /*+  afp_factor(p_s, p_f, a_s, a_f) / (N_init * x_talk_factor_E) */;

		//file_out << V << "\t" << sqrt(ER_born) << endl;
		//file_out << V << "\t" << sqrt(1 / (N_init*PDE) ) << endl;
		
		//read file
		FILE *f = fopen("D:\\git_repositories\\Afterpulsing\\AP_DC_X-talk\\ENF\\ER_LuYAG.dat", "r");

		vector<double> Ev;
		vector<double> ERv;
		vector<double> Nonpropv;

		if (f != NULL)
		{
			double x, y, z;
			while (!feof(f))
			{
				fscanf(f, "%lf %lf %lf\n", &x, &y, &z);
				Ev.push_back(x);
				ERv.push_back(y);
				Nonpropv.push_back(z);
			}
		}
		else
		{
			cout << "can't open this file" << endl;
			system("pause");
			return 0;
		}

		const double D = 2.355;

		for (int i = 0; i < Ev.size(); i++)
		{
			const double N_b = 33 * Ev[i]; // число родившихся фотонов
			const double N_abs = 0.5;
			
			const double ER2 = pow(ERv[i] / D, 2.0) + (x_talk_factor_Var / pow(x_talk_factor_E, 2.0)) * (1 / (N_b * N_abs)) + Functions::afp_factor(p_s, p_f, a_s, a_f) / (N_b * N_abs * x_talk_factor_E);
			
			file_out << Ev[i] << "\t" << D * sqrt(ER2) << endl;
		}



	}

	system("pause");
	return 0;
}