#include "RootFit.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TMath.h"
#include "Monostate.h"

RootFit::RootFit(int time_start_index, int time_finish_index, vector<double>& xv, vector<double>& yv, vector<double>& xverr, vector<double>& yverr, short int number_of_functions)
{
	gr = new TGraphErrors(time_finish_index - time_start_index, &xv[time_start_index], &yv[time_start_index], &xverr[time_start_index], &yverr[time_start_index]);
	gr->SetMarkerColor(4);
	gr->SetMarkerStyle(kFullCircle);

	this->number_of_functions = number_of_functions;

	if (number_of_functions == 1)
		fitFcn = new TF1("fitFcn", fitFunction, xv[time_start_index], xv[time_finish_index], number_of_functions * 6);
	if (number_of_functions == 2)
		fitFcn = new TF1("fitFcn", fitFunction_2, xv[time_start_index], xv[time_finish_index], number_of_functions * 6);
	if (number_of_functions == 3)
		fitFcn = new TF1("fitFcn", fitFunction_3, xv[time_start_index], xv[time_finish_index], number_of_functions * 6);
}


RootFit::~RootFit()
{
}

void RootFit::DoFit()
{
	gr->Fit("fitFcn", "RQ");
}

double RootFit::GetChi2PerDof()
{
	return fitFcn->GetChisquare() / fitFcn->GetNDF();
}

double RootFit::GetAmplitude()
{
	return fitFcn->GetParameter(0);
}


void RootFit::SetParameters(double time_start_ns, double time_finish_ns, vector<double>& time_front_ns)
{
	const double A_start = 0.05;

	// A
	fitFcn->SetParameter(0, A_start);
	fitFcn->SetParLimits(0, 0.001, 1000);

	//t_0
	fitFcn->SetParameter(1, time_front_ns[0]);
	fitFcn->SetParLimits(1, time_start_ns, time_finish_ns);

	// tau_rec
	fitFcn->SetParameter(2, 17.7373);
	fitFcn->SetParLimits(2, 17.7373, 17.7373);

	// tau_rise
	fitFcn->SetParameter(3, 10.5194);
	fitFcn->SetParLimits(3, 10.5194, 10.5194);

	//baseline
	fitFcn->SetParameter(4, 0);
	fitFcn->SetParLimits(4, -0.002, 0.002);

	//sigma
	fitFcn->SetParameter(5, 1.64932);
	fitFcn->SetParLimits(5, 1.64932, 1.64932);


	if (number_of_functions > 1)
	{

		// A
		fitFcn->SetParameter(6, A_start);
		fitFcn->SetParLimits(6, 0.001, 1000);

		//t_0
		if (time_front.size() == 1)
		{
			fitFcn->SetParameter(7, time_front_ns[0]);
		}
		else
		{
			fitFcn->SetParameter(7, time_start_ns);
		}

		fitFcn->SetParLimits(7, time_start_ns, time_finish_ns);

		// tau_rec
		fitFcn->SetParameter(8, 17.7373);
		fitFcn->SetParLimits(8, 17.7373, 17.7373);

		// tau_rise
		fitFcn->SetParameter(9, 10.5194);
		fitFcn->SetParLimits(9, 10.5194, 10.5194);


		//baseline
		fitFcn->SetParameter(10, 0);
		fitFcn->SetParLimits(10, -0.002, 0.002);

		//sigma
		fitFcn->SetParameter(11, 1.64932);
		fitFcn->SetParLimits(11, 1.64932, 1.64932);

		if (number_of_functions > 2)
		{
			// A
			fitFcn->SetParameter(12, A_start);
			fitFcn->SetParLimits(12, 0.001, 1000);

			//t_0
			fitFcn->SetParameter(7, time_start_ns);
			fitFcn->SetParameter(13, time_start_ns);

			/*if (time_front.size() > 1)
				fitFcn->SetParameter(7, time_front_ns[1]);
			else
				fitFcn->SetParameter(7, time_front_ns[0]);


			if (time_front.size() > 2)
				fitFcn->SetParameter(13, time_front_ns[2]);
			else
				fitFcn->SetParameter(13, time_front_ns[0]);*/


			fitFcn->SetParLimits(13, time_start_ns, time_finish_ns);

			// tau_rec
			fitFcn->SetParameter(14, 17.7373);
			fitFcn->SetParLimits(14, 17.7373, 17.7373);

			// tau_rise
			fitFcn->SetParameter(15, 10.5194);
			fitFcn->SetParLimits(15, 10.5194, 10.5194);

			//base_line
			fitFcn->SetParameter(16, 0);
			fitFcn->SetParLimits(16, -0.002, 0.002);

			//sigma
			fitFcn->SetParameter(18, 1.64932);
			fitFcn->SetParLimits(18, 1.64932, 1.64932);
		}

	}

}

void RootFit::Print_dt_amp()
{
	//amp_chi2_fnc2 << fitFcn_fnc2->GetParameter(0) << "\t" << fitFcn_fnc2->GetChisquare() / (time_finish[i] - time_start_index) << endl;
	//amp_chi2_fnc2 << fitFcn_fnc2->GetParameter(6) << "\t" << fitFcn_fnc2->GetChisquare() / (time_finish[i] - time_start_index) << endl;

	//if (fitFcn->GetParameter(0) > 0.02 && fitFcn_fnc2->GetChisquare() / (time_finish[i] - time_start_index) < chi2_per_dof)
	//{
	//	amp_chi2_fnc3 << fitFcn_fnc2->GetParameter(0) << "\t" << fitFcn_fnc2->GetChisquare() / (time_finish[i] - time_start_index) << endl;
	//	amp_chi2_fnc3 << fitFcn_fnc2->GetParameter(6) << "\t" << fitFcn_fnc2->GetChisquare() / (time_finish[i] - time_start_index) << endl;

	//	if (fitFcn_fnc2->GetParameter(1) > fitFcn_fnc2->GetParameter(7))
	//	{
	//		time_i << fitFcn_fnc2->GetParameter(7) << "\t" << fitFcn_fnc2->GetParameter(6) << endl;
	//		time_i << fitFcn_fnc2->GetParameter(1) << "\t" << fitFcn_fnc2->GetParameter(0) << endl;

	//	}
	//	else
	//	{
	//		time_i << fitFcn_fnc2->GetParameter(1) << "\t" << fitFcn_fnc2->GetParameter(0) << endl;
	//		time_i << fitFcn_fnc2->GetParameter(7) << "\t" << fitFcn_fnc2->GetParameter(6) << endl;

	//	}

	//}
	//
	//
	//
	//
	//
	//
	//
	//
	//
	//
	//amp_chi2_fnc3 << fitFcn_fnc3->GetParameter(0) << "\t" << fitFcn_fnc3->GetChisquare() / (time_finish[i] - time_start_index) << endl;
	//amp_chi2_fnc3 << fitFcn_fnc3->GetParameter(6) << "\t" << fitFcn_fnc3->GetChisquare() / (time_finish[i] - time_start_index) << endl;
	//amp_chi2_fnc3 << fitFcn_fnc3->GetParameter(12) << "\t" << fitFcn_fnc3->GetChisquare() / (time_finish[i] - time_start_index) << endl;

	//if (fitFcn_fnc3->GetParameter(1) < fitFcn_fnc3->GetParameter(7) && fitFcn_fnc3->GetParameter(1) < fitFcn_fnc3->GetParameter(13))
	//{
	//	if (fitFcn_fnc3->GetParameter(7) < fitFcn_fnc3->GetParameter(13))
	//	{
	//		time_i << fitFcn_fnc3->GetParameter(1) << "\t" << fitFcn_fnc3->GetParameter(0) << endl;
	//		time_i << fitFcn_fnc3->GetParameter(7) << "\t" << fitFcn_fnc3->GetParameter(6) << endl;
	//		time_i << fitFcn_fnc3->GetParameter(13) << "\t" << fitFcn_fnc3->GetParameter(12) << endl;
	//	}
	//	else
	//	{
	//		time_i << fitFcn_fnc3->GetParameter(1) << "\t" << fitFcn_fnc3->GetParameter(0) << endl;
	//		time_i << fitFcn_fnc3->GetParameter(13) << "\t" << fitFcn_fnc3->GetParameter(12) << endl;
	//		time_i << fitFcn_fnc3->GetParameter(7) << "\t" << fitFcn_fnc3->GetParameter(6) << endl;
	//	}
	//}

	//if (fitFcn_fnc3->GetParameter(1) > fitFcn_fnc3->GetParameter(7) && fitFcn_fnc3->GetParameter(1) < fitFcn_fnc3->GetParameter(13))
	//{
	//	time_i << fitFcn_fnc3->GetParameter(7) << "\t" << fitFcn_fnc3->GetParameter(6) << endl;
	//	time_i << fitFcn_fnc3->GetParameter(1) << "\t" << fitFcn_fnc3->GetParameter(0) << endl;
	//	time_i << fitFcn_fnc3->GetParameter(13) << "\t" << fitFcn_fnc3->GetParameter(12) << endl;
	//}

	//if (fitFcn_fnc3->GetParameter(1) > fitFcn_fnc3->GetParameter(13) && fitFcn_fnc3->GetParameter(1) < fitFcn_fnc3->GetParameter(7))
	//{

	//	time_i << fitFcn_fnc3->GetParameter(13) << "\t" << fitFcn_fnc3->GetParameter(12) << endl;
	//	time_i << fitFcn_fnc3->GetParameter(1) << "\t" << fitFcn_fnc3->GetParameter(0) << endl;
	//	time_i << fitFcn_fnc3->GetParameter(7) << "\t" << fitFcn_fnc3->GetParameter(6) << endl;
	//}


	//if (fitFcn_fnc3->GetParameter(1) > fitFcn_fnc3->GetParameter(7) && fitFcn_fnc3->GetParameter(1) > fitFcn_fnc3->GetParameter(13))
	//{
	//	if (fitFcn_fnc3->GetParameter(7) < fitFcn_fnc3->GetParameter(13))
	//	{
	//		time_i << fitFcn_fnc3->GetParameter(7) << "\t" << fitFcn_fnc3->GetParameter(6) << endl;
	//		time_i << fitFcn_fnc3->GetParameter(13) << "\t" << fitFcn_fnc3->GetParameter(12) << endl;
	//		time_i << fitFcn_fnc3->GetParameter(1) << "\t" << fitFcn_fnc3->GetParameter(0) << endl;
	//	}
	//	else
	//	{
	//		time_i << fitFcn_fnc3->GetParameter(13) << "\t" << fitFcn_fnc3->GetParameter(12) << endl;
	//		time_i << fitFcn_fnc3->GetParameter(7) << "\t" << fitFcn_fnc3->GetParameter(6) << endl;
	//		time_i << fitFcn_fnc3->GetParameter(1) << "\t" << fitFcn_fnc3->GetParameter(0) << endl;
	//	}

	//}

	//ofstream time_delta(dir_name + "time_delta.dat");
	//
	//string string_time_i = dir_name + "time_i.dat";
	//FILE *f2 = fopen(string_time_i.c_str(), "r");

	//double x_old;
	//bool flag = 0;
	//while (!feof(f2))
	//{
	//	fscanf(f2, "%lf %lf\n", &x, &y);

	//	if (flag)
	//		time_delta << x - x_old << "\t" << y << endl;

	//	x_old = x;

	//	flag = 1;
	//}

}


void RootFit::SaveGraphs(TObjArray &Hlist)
{
	Hlist.Add(gr);
}


//вспомогательная функция
double RootFit::F(double t, double sigma, double tau)
{
	return TMath::Exp((sigma*sigma - 2 * t*tau) / (2 * tau*tau)) * (1 + TMath::Erf((t - sigma*sigma / tau) / (sigma * sqrt(2))));
}

//функция, которой буду фитировать
Double_t RootFit::fitFunction(Double_t *x, Double_t *par)
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

//сумма двух сигналов
double RootFit::fitFunction_2(Double_t *x, Double_t *par)
{
	return fitFunction(x, par) + fitFunction(x, &par[6]);
}

//сумма трех сигналов
double RootFit::fitFunction_3(Double_t *x, Double_t *par)
{
	return fitFunction(x, par) + fitFunction(x, &par[6]) + fitFunction(x, &par[12]);
}