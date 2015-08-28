#include "RootFit.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TMath.h"
#include "Monostate.h"

RootFit::RootFit(short int number_of_functions)
{
		
	gr = new TGraphErrors(time_finish_index - time_start_index, &xv[time_start_index], &yv[time_start_index], &xverr[time_start_index], &yverr[time_start_index]);
	gr->SetMarkerColor(4);
	gr->SetMarkerStyle(kFullCircle);

	gr_der = new TGraph(time_finish_index - time_start_index, &xv[time_start_index], &yv_der[time_start_index]);
	gr_der2 = new TGraph(time_finish_index - time_start_index, &xv[time_start_index], &yv_der2[time_start_index]);

	//cout << xv_front.size() << "\t" << yv_front.size() << endl;

	
	//for (int i = 0; i < xv_front.size(); i++)
	//{
	//	cout << xv_front[i] << "\t" << yv_front[i] << "\t" << xv_front.size() << endl;
	//}

	//system("pause");

	gr_front = new TGraph(time_finish_index - time_start_index, &xv_front[0], &yv_front[0]);// problem

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


void RootFit::SetParameters(double time_first, double time_second, double time_third)
{
	const double A_start = 0.05;

	// A
	fitFcn->SetParameter(0, A_start);
	fitFcn->SetParLimits(0, 0.001, 1000);

	//t_0
	fitFcn->SetParameter(1, time_first);
	fitFcn->SetParLimits(1, xv[time_start_index], xv[time_finish_index]);

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
		fitFcn->SetParameter(7, time_second);
		fitFcn->SetParLimits(7, xv[time_start_index], xv[time_finish_index]);

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
			fitFcn->SetParameter(13, time_third);
			fitFcn->SetParLimits(13, xv[time_start_index], xv[time_finish_index]);

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

	//fitFcn->Modify();
	//fitFcn->ReleaseParameter();
	fitFcn->Update();

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





