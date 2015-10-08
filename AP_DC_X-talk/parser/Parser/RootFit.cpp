#include "RootFit.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TMath.h"
#include "Monostate.h"
#include <chrono>
#include <iomanip>

#include <Windows.h>

#include "Math/MinimizerOptions.h"

using namespace std;

vector<double> RootFit::yv_s;
vector<double> RootFit::yv_der;
vector<double> RootFit::yv_der2;

vector<double> RootFit::xverr;
vector<double> RootFit::yverr;
vector<double> RootFit::yv_front;
vector<double> RootFit::xv_front;

vector<double> RootFit::C_i_s;
vector<double> RootFit::C_i_der;
vector<double> RootFit::C_i_der2;

bool RootFit::previousIs1e;
double RootFit::previous_time;

//bool RootFit::PreviousIsSingle;
//double RootFit::temp_time_i;

RootFit::RootFit(short int number_of_functions)
{
	this->number_of_functions = number_of_functions;

	gr = new TGraphErrors(time_finish_index - time_start_index, &xv[time_start_index], &yv[time_start_index], &xverr[time_start_index], &yverr[time_start_index]);
	gr->SetMarkerColor(4);
	gr->SetMarkerStyle(kFullCircle);

	gr_der = new TGraph(time_finish_index - time_start_index, &xv[time_start_index], &yv_der[time_start_index]);
	gr_der2 = new TGraph(time_finish_index - time_start_index, &xv[time_start_index], &yv_der2[time_start_index]);

	for (int i = 0; i < gr_der->GetN(); i++) gr_der->GetY()[i] *= 50;
	for (int i = 0; i < gr_der2->GetN(); i++) gr_der2->GetY()[i] *= 500;

	gr_der2->SetLineColor(7);

	gr_front = new TGraph(time_finish_index - time_start_index, &xv_front[0], &yv_front[0]);
	gr_front->SetMarkerColor(6);
	gr_front->SetMarkerStyle(kFullCircle);
	gr_front->SetMarkerSize(1);

	if (number_of_functions == 1)
		fitFcn = new TF1("fitFcn", fitFunction, xv[time_start_index], xv[time_finish_index], 6);
	if (number_of_functions == 2)
		fitFcn = new TF1("fitFcn", fitFunction_2, xv[time_start_index], xv[time_finish_index], 6 + 5);
	if (number_of_functions == 3)
		fitFcn = new TF1("fitFcn", fitFunction_3, xv[time_start_index], xv[time_finish_index], 6 + 10);
	if (number_of_functions == 4)
		fitFcn = new TF1("fitFcn", fitFunction_4, xv[time_start_index], xv[time_finish_index], 6 + 15);
	if (number_of_functions == 5)
		fitFcn = new TF1("fitFcn", fitFunction_5, xv[time_start_index], xv[time_finish_index], 6 + 20);
	if (number_of_functions == 6)
		fitFcn = new TF1("fitFcn", fitFunction_6, xv[time_start_index], xv[time_finish_index], 6 + 25);

	//ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit", "Simplex");
	//ROOT::Math::MinimizerOptions::SetDefaultPrecision(17);
	ROOT::Math::MinimizerOptions::SetDefaultStrategy(2);
}


RootFit::~RootFit()
{
	delete fitFcn;
	delete gr_front;
	delete gr_der2;
	delete gr_der;
	delete gr;
}


void RootFit::SetParameters()
{
	const double A_start = 0.04;
	const double A_limit_l = 0.001;
	const double A_limit_h = 1;

	const double baseline_limit = 0.003;
	const double sigma = 1.64932;

	const double time_first = xv[time_front[0]];

	//fitFcn->SetParErrors

	// A
	fitFcn->SetParameter(0, A_start);
	fitFcn->SetParLimits(0, A_limit_l, A_limit_h);

	//t_0
	fitFcn->SetParameter(1, time_first);
	fitFcn->SetParLimits(1, xv[time_start_index], xv[time_finish_index]);

	// tau_rec
	fitFcn->SetParameter(2, 17.7373);
	fitFcn->SetParLimits(2, 17.7373, 17.7373);

	// tau_rise
	fitFcn->SetParameter(3, 10.5194);
	fitFcn->SetParLimits(3, 10.5194, 10.5194);

	//sigma
	fitFcn->SetParameter(4, sigma);
	fitFcn->SetParLimits(4, sigma, sigma);

	//baseline
	fitFcn->SetParameter(5, 0);
	fitFcn->SetParLimits(5, -baseline_limit, baseline_limit);




	if (number_of_functions > 1)
	{

		double time_second;
		if (time_front.size() > 1)
		{
			time_second = xv[time_front[1]];
		}
		else
		{
			time_second = xv[time_front[0]];
		}

		// A
		fitFcn->SetParameter(6, A_start);
		fitFcn->SetParLimits(6, A_limit_l, A_limit_h);

		//t_0
		fitFcn->SetParameter(7, time_second);
		fitFcn->SetParLimits(7, xv[time_start_index], xv[time_finish_index]);

		// tau_rec
		fitFcn->SetParameter(8, 17.7373);
		fitFcn->SetParLimits(8, 17.7373, 17.7373);

		// tau_rise
		fitFcn->SetParameter(9, 10.5194);
		fitFcn->SetParLimits(9, 10.5194, 10.5194);

		//sigma
		fitFcn->SetParameter(10, sigma);
		fitFcn->SetParLimits(10, sigma, sigma);

		if (number_of_functions > 2)
		{
			double time_third;
			if (time_front.size() > 2)
			{
				time_third = xv[time_front[2]];
			}
			else
			{
				time_third = xv[time_front[0]];
			}


			// A
			fitFcn->SetParameter(11, A_start);
			fitFcn->SetParLimits(11, A_limit_l, A_limit_h);

			//t_0
			fitFcn->SetParameter(12, time_third);
			fitFcn->SetParLimits(12, xv[time_start_index], xv[time_finish_index]);

			// tau_rec
			fitFcn->SetParameter(13, 17.7373);
			fitFcn->SetParLimits(13, 17.7373, 17.7373);

			// tau_rise
			fitFcn->SetParameter(14, 10.5194);
			fitFcn->SetParLimits(14, 10.5194, 10.5194);

			//sigma
			fitFcn->SetParameter(15, sigma);
			fitFcn->SetParLimits(15, sigma, sigma);


			if (number_of_functions > 3)
			{
				double time_fourth;
				if (time_front.size() > 3)
				{
					time_fourth = xv[time_front[3]];
				}
				else
				{
					time_fourth = xv[time_front[0]];
				}

				// A
				fitFcn->SetParameter(11 + 5, A_start);
				fitFcn->SetParLimits(11 + 5, A_limit_l, A_limit_h);

				//t_0
				fitFcn->SetParameter(12 + 5, time_fourth);
				fitFcn->SetParLimits(12 + 5, xv[time_start_index], xv[time_finish_index]);

				// tau_rec
				fitFcn->SetParameter(13 + 5, 17.7373);
				fitFcn->SetParLimits(13 + 5, 17.7373, 17.7373);

				// tau_rise
				fitFcn->SetParameter(14 + 5, 10.5194);
				fitFcn->SetParLimits(14 + 5, 10.5194, 10.5194);

				//sigma
				fitFcn->SetParameter(15 + 5, sigma);
				fitFcn->SetParLimits(15 + 5, sigma, sigma);

				if (number_of_functions > 4)
				{
					double time_fiths;
					if (time_front.size() > 4)
					{
						time_fiths = xv[time_front[4]];
					}
					else
					{
						time_fiths = xv[time_front[0]];
					}

					// A
					fitFcn->SetParameter(11 + 10, A_start);
					fitFcn->SetParLimits(11 + 10, A_limit_l, A_limit_h);

					//t_0
					fitFcn->SetParameter(12 + 10, time_fiths);
					fitFcn->SetParLimits(12 + 10, xv[time_start_index], xv[time_finish_index]);

					// tau_rec
					fitFcn->SetParameter(13 + 10, 17.7373);
					fitFcn->SetParLimits(13 + 10, 17.7373, 17.7373);

					// tau_rise
					fitFcn->SetParameter(14 + 10, 10.5194);
					fitFcn->SetParLimits(14 + 10, 10.5194, 10.5194);

					//sigma
					fitFcn->SetParameter(15 + 10, sigma);
					fitFcn->SetParLimits(15 + 10, sigma, sigma);
				}

			}

		}

	}

	//fitFcn->Modify();
	//fitFcn->ReleaseParameter();
	//fitFcn->Update();

}

void RootFit::SaveAllGraphs()
{

	if (this->number_of_functions == 1)
	{
		////записать графики с условием отбора
		//bool flag_temp = Fit_single->fitFcn->GetParameter(0) < 0.03;
		//if (flag_temp && Fit_single->GetChi2PerDof() < Monostate::chi2_per_dof_th)
		//{
		//	Fit_single->SaveGraphs(Monostate::Hlist_test);
		//}		

		//записать все графики
		this->SaveGraphs(Monostate::Hlist_f1);

		//записать графики с плохим Chi2 после фита одной функцией
		if (this->GetChi2PerDof() > Monostate::chi2_per_dof_th)
		{
			this->SaveGraphs(Monostate::Hlist_f1_bad);
		}


		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
		{
			//записать графики с хорошим Chi2 после фита одной функцией
			this->SaveGraphs(Monostate::Hlist_f1_good);

			//записать все графики после фита одной функцией
			this->SaveGraphs(Monostate::Hlist_f2);
		}

		//печатать в файл амплитуду и время успешно фитированных импульсов
		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th && fitFcn->GetParameter(0) < 0.05)
		{
			this->Print_dt_amp();
		}
		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th && fitFcn->GetParameter(0) > 0.05)
		{
			previousIs1e = false;
		}

	}

	if (this->number_of_functions == 2)
	{
		////записать графики с условием отбора
		//bool flag_temp = Fit_double->fitFcn->GetParameter(0) < 0.03 || Fit_double->fitFcn->GetParameter(6) < 0.03;
		//if (flag_temp && Fit_double->GetChi2PerDof() < Monostate::chi2_per_dof_th)
		//{
		//	Fit_double->SaveGraphs(Monostate::Hlist_test);
		//}

		//записать все графики после фита одной функцией
		this->SaveGraphs(Monostate::Hlist_f2);

		//записать графики с плохим Chi2 после фита двумя функциями
		if (this->GetChi2PerDof() > Monostate::chi2_per_dof_th)
		{
			this->SaveGraphs(Monostate::Hlist_f2_bad);
		}


		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
		{
			//записать графики с хорошим Chi2 после фита двумя функциями
			this->SaveGraphs(Monostate::Hlist_f2_good);
		}

		//печатать в файл амплитуду и время успешно фитированных импульсов
		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th && fitFcn->GetParameter(0) < 0.05 && fitFcn->GetParameter(6) < 0.05)
			this->Print_dt_amp();
		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
		{
			if (fitFcn->GetParameter(0) > 0.05 || fitFcn->GetParameter(6) > 0.05)
			{
				previousIs1e = false;
			}
		}

	}

	if (this->number_of_functions == 3)
	{
		//записать графики с условием отбора
		/*bool flag_temp = this->fitFcn->GetParameter(0) < 0.03 || this->fitFcn->GetParameter(6) < 0.03 || this->fitFcn->GetParameter(11) < 0.03;
		if (flag_temp && this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
		{
			this->SaveGraphs(Monostate::Hlist_test);
		}*/

		//записать графики, после фита тремя функциями
		this->SaveGraphs(Monostate::Hlist_f3);

		//записать графики с плохим Chi2 после фита тремя функциями
		if (this->GetChi2PerDof() > Monostate::chi2_per_dof_th)
		{
			this->SaveGraphs(Monostate::Hlist_f3_bad);
		}

		//записать графики с хорошим Chi2 после фита тремя функциями
		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
		{
			this->SaveGraphs(Monostate::Hlist_f3_good);
		}

		//печатать в файл амплитуду и время успешно фитированных импульсов
		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th && fitFcn->GetParameter(0) < 0.05 && fitFcn->GetParameter(6) < 0.05 && fitFcn->GetParameter(11) < 0.05)
			this->Print_dt_amp();

		//if (fitFcn->GetParameter(0) > 0.02)
		//{
		//	
		//	time_test << fitFcn_fnc3->GetParameter(1) << "\t" << fitFcn_fnc3->GetParameter(7) << "\t" << fitFcn_fnc3->GetParameter(13) << endl;

		//} 
	}


	if (this->number_of_functions == 4)
	{
		//записать графики с плохим Chi2 после фита 4 функциями
		if (this->GetChi2PerDof() > Monostate::chi2_per_dof_th)
		{
			this->SaveGraphs(Monostate::Hlist_f4_bad);
		}

		//записать графики с хорошим Chi2 после фита 4 функциями
		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
		{
			this->SaveGraphs(Monostate::Hlist_f4_good);
		}

		//печатать в файл амплитуду и время успешно фитированных импульсов
		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th && fitFcn->GetParameter(0) < 0.05 && fitFcn->GetParameter(6) < 0.05 && fitFcn->GetParameter(11) < 0.05 && fitFcn->GetParameter(16) < 0.05)
			this->Print_dt_amp();
	}

	if (this->number_of_functions == 5)
	{
		//печатать в файл амплитуду и время успешно фитированных импульсов
		//if (Fit_quintuple->GetChi2PerDof() < Monostate::chi2_per_dof_th)
		this->Print_dt_amp();

		//записать графики с плохим Chi2 после фита 5 функциями
		if (this->GetChi2PerDof() > Monostate::chi2_per_dof_th)
		{
			this->SaveGraphs(Monostate::Hlist_f5_bad);
		}

		//записать графики с хорошим Chi2 после фита 5 функциями
		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th && fitFcn->GetParameter(0) < 0.05 && fitFcn->GetParameter(6) < 0.05 && fitFcn->GetParameter(11) < 0.05 && fitFcn->GetParameter(16) < 0.05 && fitFcn->GetParameter(21) < 0.05)
		{
			this->SaveGraphs(Monostate::Hlist_f5_good);
		}
	}


}


void RootFit::Print_dt_amp()
{
	//функтор
	struct sort_pred
	{
		bool operator()(const std::pair<int, int> &left, const std::pair<int, int> &right)
		{
			return left.first < right.first;
		}
	};


	vector<pair<double, double>> v_pairs;
	pair<double, double> pr;


	if (this->number_of_functions == 1)
	{
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::time_i << setprecision(17) << fitFcn->GetParameter(1) << "\t" << fitFcn->GetParameter(0) << endl;

		Monostate::file_amp << setprecision(17) << fitFcn->GetParameter(0) << endl;


		if (previousIs1e)
		{
			Monostate::file_dt << fitFcn->GetParameter(1) - previous_time << endl;

			if (fitFcn->GetParameter(1) - previous_time > 3000)
			{
				cout << endl << "strange times (ns) is " << previous_time << "\t" << fitFcn->GetParameter(1) << endl;
				cout << endl << "strange times (points) is " << previous_time*5 << "\t" << fitFcn->GetParameter(1)*5 << endl;
				
				int a = int(previous_time * 5) - 300;
				int b = int(fitFcn->GetParameter(1) * 5) + 300;
				
				TGraphErrors *gr_long_dt = new TGraphErrors(b - a, &xv[a], &yv[a], &xverr[a], &yverr[a]);
				gr_long_dt->SetMarkerColor(4);
				gr_long_dt->SetMarkerStyle(kFullCircle);

				TGraph *gr_der_long_dt = new TGraph(b - a, &xv[a], &yv_der[a]);
				for (int i = 0; i < gr_der_long_dt->GetN(); i++) gr_der_long_dt->GetY()[i] *= 50;

				TMultiGraph *mg_long_dt = new TMultiGraph();

				mg_long_dt->Add(gr_long_dt);
				mg_long_dt->Add(gr_der_long_dt);

				Monostate::Hlist_test.Add(mg_long_dt);
				/*for (int k = int(previous_time * 5); k < int(fitFcn->GetParameter(1) * 5); k++)
				{
					Monostate::file_long_dt << xv[k] << "\t" << yv[k] << endl;
				}

				system("pause");*/
				//this->SaveGraphs(Monostate::Hlist_test);
				
				//delete mg_long_dt;
				//delete gr_der_long_dt;				
				//delete gr_long_dt;
								
			}

			previous_time = fitFcn->GetParameter(1);
		}
		else
		{
			previous_time = fitFcn->GetParameter(1);
			previousIs1e = true;
		}

		/*if (!PreviousIsSingle)
		{
		temp_time_i = fitFcn->GetParameter(1);
		}
		else
		{
		Monostate::file_dt << fitFcn->GetParameter(1) - temp_time_i << endl;
		temp_time_i = fitFcn->GetParameter(1);
		}

		PreviousIsSingle = true;*/
	}

	if (this->number_of_functions == 2)
	{
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(6) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;

		pr.first = fitFcn->GetParameter(1);
		pr.second = fitFcn->GetParameter(0);
		v_pairs.push_back(pr);

		pr.first = fitFcn->GetParameter(7);
		pr.second = fitFcn->GetParameter(6);
		v_pairs.push_back(pr);

		sort(v_pairs.begin(), v_pairs.end(), sort_pred());

		Monostate::time_i << v_pairs[0].first << "\t" << v_pairs[0].second << endl;
		Monostate::time_i << v_pairs[1].first << "\t" << v_pairs[1].second << endl;

		Monostate::file_amp << v_pairs[0].second << endl;
		Monostate::file_amp << v_pairs[1].second << endl;

		if (previousIs1e)
		{
			Monostate::file_dt << fitFcn->GetParameter(1) - previous_time << endl;
			Monostate::file_dt << fitFcn->GetParameter(7) - fitFcn->GetParameter(1) << endl;
			previous_time = fitFcn->GetParameter(7);
		}
		else
		{
			Monostate::file_dt << fitFcn->GetParameter(7) - fitFcn->GetParameter(1) << endl;
			previous_time = fitFcn->GetParameter(7);
			previousIs1e = true;
		}

		//Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;

		/*if (!PreviousIsSingle)
			Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
			else
			Monostate::file_dt << v_pairs[0].first - temp_time_i << endl;

			PreviousIsSingle = false;*/
	}

	if (this->number_of_functions == 3)
	{
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(6) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(11) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;

		pr.first = fitFcn->GetParameter(1);
		pr.second = fitFcn->GetParameter(0);
		v_pairs.push_back(pr);

		pr.first = fitFcn->GetParameter(7);
		pr.second = fitFcn->GetParameter(6);
		v_pairs.push_back(pr);

		pr.first = fitFcn->GetParameter(12);
		pr.second = fitFcn->GetParameter(11);
		v_pairs.push_back(pr);

		sort(v_pairs.begin(), v_pairs.end(), sort_pred());

		Monostate::time_i << setprecision(17) << v_pairs[0].first << "\t" << v_pairs[0].second << endl;
		Monostate::time_i << setprecision(17) << v_pairs[1].first << "\t" << v_pairs[1].second << endl;
		Monostate::time_i << setprecision(17) << v_pairs[2].first << "\t" << v_pairs[2].second << endl;

		Monostate::file_amp << v_pairs[0].second << endl;
		Monostate::file_amp << v_pairs[1].second << endl;
		Monostate::file_amp << v_pairs[2].second << endl;

		//Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
		//Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;

		/*if (!PreviousIsSingle)
			Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
			else
			Monostate::file_dt << v_pairs[0].first - temp_time_i << endl;

			PreviousIsSingle = false;*/
	}

	if (this->number_of_functions == 4)
	{
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(6) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(11) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(11 + 5) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;

		pr.first = fitFcn->GetParameter(1);
		pr.second = fitFcn->GetParameter(0);
		v_pairs.push_back(pr);

		pr.first = fitFcn->GetParameter(7);
		pr.second = fitFcn->GetParameter(6);
		v_pairs.push_back(pr);

		pr.first = fitFcn->GetParameter(12);
		pr.second = fitFcn->GetParameter(11);
		v_pairs.push_back(pr);

		pr.first = fitFcn->GetParameter(17);
		pr.second = fitFcn->GetParameter(16);
		v_pairs.push_back(pr);

		sort(v_pairs.begin(), v_pairs.end(), sort_pred());

		Monostate::time_i << setprecision(17) << v_pairs[0].first << "\t" << v_pairs[0].second << endl;
		Monostate::time_i << setprecision(17) << v_pairs[1].first << "\t" << v_pairs[1].second << endl;
		Monostate::time_i << setprecision(17) << v_pairs[2].first << "\t" << v_pairs[2].second << endl;
		Monostate::time_i << setprecision(17) << v_pairs[3].first << "\t" << v_pairs[3].second << endl;

		Monostate::file_amp << v_pairs[0].second << endl;
		Monostate::file_amp << v_pairs[1].second << endl;
		Monostate::file_amp << v_pairs[2].second << endl;
		Monostate::file_amp << v_pairs[3].second << endl;

		//Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
		//Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
		//Monostate::file_dt << v_pairs[3].first - v_pairs[2].first << endl;

		/*if (!PreviousIsSingle)
			Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
			else
			Monostate::file_dt << v_pairs[0].first - temp_time_i << endl;

			PreviousIsSingle = false;*/
	}

	if (this->number_of_functions == 5)
	{
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(6) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(11) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(11 + 5) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(11 + 10) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;

		pr.first = fitFcn->GetParameter(1);
		pr.second = fitFcn->GetParameter(0);
		v_pairs.push_back(pr);

		pr.first = fitFcn->GetParameter(7);
		pr.second = fitFcn->GetParameter(6);
		v_pairs.push_back(pr);

		pr.first = fitFcn->GetParameter(12);
		pr.second = fitFcn->GetParameter(11);
		v_pairs.push_back(pr);

		pr.first = fitFcn->GetParameter(17);
		pr.second = fitFcn->GetParameter(16);
		v_pairs.push_back(pr);

		pr.first = fitFcn->GetParameter(22);
		pr.second = fitFcn->GetParameter(21);
		v_pairs.push_back(pr);

		sort(v_pairs.begin(), v_pairs.end(), sort_pred());

		Monostate::time_i << setprecision(17) << v_pairs[0].first << "\t" << v_pairs[0].second << endl;
		Monostate::time_i << setprecision(17) << v_pairs[1].first << "\t" << v_pairs[1].second << endl;
		Monostate::time_i << setprecision(17) << v_pairs[2].first << "\t" << v_pairs[2].second << endl;
		Monostate::time_i << setprecision(17) << v_pairs[3].first << "\t" << v_pairs[3].second << endl;
		Monostate::time_i << setprecision(17) << v_pairs[4].first << "\t" << v_pairs[4].second << endl;

		Monostate::file_amp << v_pairs[0].second << endl;
		Monostate::file_amp << v_pairs[1].second << endl;
		Monostate::file_amp << v_pairs[2].second << endl;
		Monostate::file_amp << v_pairs[3].second << endl;
		Monostate::file_amp << v_pairs[4].second << endl;

		//Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
		///Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
		//Monostate::file_dt << v_pairs[3].first - v_pairs[2].first << endl;
		//Monostate::file_dt << v_pairs[4].first - v_pairs[3].first << endl;

		/*if (!PreviousIsSingle)
			Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
			else
			Monostate::file_dt << v_pairs[0].first - temp_time_i << endl;

			PreviousIsSingle = false;*/

	}

}

void RootFit::CalculateStaircases_der(double time_dead)
{
	ofstream file_staitcase_der(Monostate::dir_name + "staitcase_der.dat");

	bool flag = 1;
	double x_time = 0;
	int counter = 0;

	for (double th = -0.001; th < 0; th += 0.00001)
	{
		for (unsigned int i = 0; i < xv.size(); i++)
		{
			if ((yv_der[i] < th) && flag)
			{
				x_time = xv[i];
				flag = 0;
				counter++;
			}

			if (yv_der[i] > th && (xv[i] - x_time) > (time_dead) && flag == 0)
			{
				flag = 1;
			}
		}

		file_staitcase_der << th << "\t" << counter << endl;
	}
}

void RootFit::CalculateStaircases_amp(double time_dead)
{
	ofstream file_staitcase_amp(Monostate::dir_name + "staitcase_amp.dat");
	bool flag = 1;
	double x_time = 0;
	int counter = 0;

	for (double th = -0.01; th < 0; th += 0.0001)
	{
		for (unsigned int i = 0; i < xv.size(); i++)
		{
			if ((yv[i] < th) && flag)
			{
				x_time = xv[i];
				flag = 0;
				counter++;
			}

			if (yv[i] > th && (xv[i] - x_time) > (time_dead) && flag == 0)
			{
				flag = 1;
			}
		}

		file_staitcase_amp << th << "\t" << counter << endl;
	}
}


void RootFit::FindStartStop(double time_dead_signal_noise, double time_dead_forward)
{
	cout << endl << "Find start and stop" << endl;

	bool flag = 1;
	//double time_dead_signal_noise = 10 /*5*/; // ns
	//double time_dead_forward = 50; // ns

	double x_time;
	//поиск начала и конца 
	for (unsigned int i = 0; i < xv.size(); i++)
	{
		if ((yv_der[i] < threshold_der && yv[i] < threshold_amp_start) && flag)
		{
			x_time = xv[i];
			flag = 0;
			time_start.push_back(i);
		}

		//разрешить искать сигнал, когда сигнал дойдет до шумов и пройдет 5 нс
		if (yv[i] > threshold_amp && flag == 0 && (xv[i] - x_time) > time_dead_signal_noise)
		{
			bool flag_2 = true;

			//cout << i << "\t" << yv_der.size() << "\t" << i + 50 * 5 << "\t" << bool(yv_der.size() < i + 50 * 5) << endl;

			//разрешить искать сигнал, только если впереди на time_dead_2 [ns] нет нового импульса
			for (int j = i; j < i + time_dead_forward * 5; j++)
			{
				if (yv_der.size() > i + time_dead_forward * 5)
				{
					//cout << yv_der[j] << "\t" << j << "\t" << yv_der.size() << endl;
					if (yv_der[j] < threshold_der)
					{
						flag_2 = false;
					}
				}

			}

			if (flag_2)
			{
				time_finish.push_back(i);
				flag = 1;
			}

		}
	}





}

//найти число сигналов на участке по 2-й производной. Необходимо задать мертвое время в нс.
void RootFit::CalculateNumOfSignals(double time_dead)
{
	bool flag = 1;
	double x_time = 0;
	unsigned int num_of_signals = 0;
	int time_dead_index = time_dead * 5;

	for (unsigned int i = time_start_index; i < time_finish_index; i++)
	{

		if (yv_der[i] < 0)
		{
			if ((yv_der2[i] < threshold_der2) && flag)
			{
				x_time = xv[i];
				flag = 0;
				num_of_signals++;
			}

			if (yv_der2[i] > threshold_der2 && (i - x_time) /* (xv[i] - x_time) > time_dead */ > (time_dead_index) && flag == 0)
			{
				flag = 1;
			}
		}
	}

	cout << "num_of_signals = " << num_of_signals << endl;
}

//вычислить стартовые времена t_i. Необходимо задать мертвое время в нс.
void RootFit::CalculateStartParameters(double time_dead)
{
	if (time_start[current_signal] - time_shit >= 0)
	{
		time_start_index = time_start[current_signal] - time_shit;
	}
	else
	{
		time_start_index = 0;
	}

	time_finish_index = time_finish[current_signal];

	bool flag = 1;
	int x_time = 0;

	//int time_start_index = time_start[current_signal];

	time_front.clear();

	int time_dead_index = time_dead * 5;
	//int shift = 20;

	//найти стартовые параметры для начала сигнала
	for (int j = time_start_index; j < time_finish[current_signal]; j++)
	{

		if (yv_der[j] < threshold_der && flag)
		{
			//int der_min = j;
			//for (int k = j; k < j + time_dead_index; k++)
			//{
			//	if (yv_der[k] < yv_der[j])
			//	{
			//		der_min = k;
			//	}

			//	//cout << k << "\t" << der_min << endl;
			//	cout << yv_der[k] << "\t" << yv_der[der_min] << endl;
			//	cout << xv[k] << "\t" << xv[der_min] << endl;
			//	cout << endl;
			//}

			//it2 = max_element(myVector.begin(), myVector.end());
			//cout << " the max is " << *it2 << endl;

			//cout << *min_element(yv_der.begin(), yv_der.end()) << endl;

			int min_index = min_element(yv_der.begin() + j, yv_der.begin() + j + time_dead_index) - yv_der.begin();
			//int min_index = min_element(yv_der2.begin() + j - shift, yv_der2.begin() + j + time_dead_index) - yv_der2.begin();


			//cout << xv[j] << "\t" << xv[min_index] << "\t" << xv[j + time_dead_index] << "\t" << current_signal << endl;
			//cout << xv[der_min] << endl;

			//cout << "min value is " << *min_element(yv_der.begin() + j, yv_der.begin() + j + time_dead_index) << endl;
			//cout << "min value at " << min_element(yv_der.begin() + j, yv_der.begin() + j + time_dead_index) - yv_der.begin() << endl;

			//system("pause");

			time_front.push_back(min_index);
			flag = 0;
			x_time = j;
		}

		if (yv_der[j] > threshold_der && (j - x_time) > (time_dead_index))
		{
			flag = 1;
		}

	}

	/*for (int i = 0; i < time_front.size(); i++)
	{
	cout << time_front[i] << "\t" << xv[time_front[i]] << endl;
	}

	system("pause");*/
}


void RootFit::CalculateDer(int type, int points)
{


	cout << endl << "Calculate derivative" << endl;
	long int t0_Calculate_derivative = GetTickCount();

	if (type == 0)
	{
		//simple method
		for (unsigned int i = 0; i < xv.size(); i++)
		{
			if (i < points)
			{
				yv_der.push_back(0);
			}
			else
			{
				yv_der.push_back(yv[i] - yv[i - points]);
			}

		}
	}
	else
	{
		CalculateFilterCoeff(points);

		//for (int i = 0; i < C_i_der2.size(); i++)
		//{
		//	cout << C_i_s[i] << endl;
		//}

		//system("pause");


		const int point_half = (points - 1) / 2.0;
		//double value;

		//cout << "yv.size()" << yv.size() << endl;

		yv_der.resize(yv.size());
		yv_der2.resize(yv.size());

#pragma omp parallel for num_threads(8)
		for (int i = 0; i < xv.size(); i++)
		{
			//if (i % 100000 == 0)
			//{
			//	//long int temp_2 = GetTickCount();
			//	cout << "calculate derivative ... " << double(i) / xv.size() * 100 << " %" << endl;
			//}

			if (i < point_half || i >(xv.size() - point_half - 1))
			{
				//yv_s.push_back(0);
				yv_der[i] = 0;
				yv_der2[i] = 0;

			}
			else
			{
				double value = 0;
				for (int j = 0; j < C_i_der.size(); j++)
				{
					value += C_i_der[j] * yv[i - point_half + j];
				}
				yv_der[i] = value;
				

				value = 0;
				for (int j = 0; j < C_i_der.size(); j++)
				{
					value += C_i_der2[j] * yv[i - point_half + j];
				}
				yv_der2[i] = value;
				

			}

		}
	}

	/*ofstream file_raw("D:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\raw\\test_data_raw.txt");
	ofstream file_s("D:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\raw\\test_data_s.txt");
	ofstream file_d("D:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\raw\\test_data_der.txt");
	ofstream file_d2("D:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\raw\\test_data_der2.txt");

	for (int i = 0; i < xv.size(); i++)
	{
	file_raw << xv[i] << "\t" << yv[i] << endl;
	file_s << xv[i] << "\t" << yv_s[i] << endl;
	file_d << xv[i] << "\t" << yv_der[i] << endl;
	file_d2 << xv[i] << "\t" << yv_der2[i] << endl;
	}

	exit(0);*/

	cout << "xv.size() " << xv.size() << endl;
	cout << "yv.size() " << yv.size() << endl;
	cout << "yv_der.size() " << yv_der.size() << endl;
	cout << "yv_der2.size() " << yv_der2.size() << endl;

	string file_d_string = Monostate::dir_name + "der\\run_1.bin";
	FILE *file_d = fopen(file_d_string.c_str(), "wb");

	string file_d2_string = Monostate::dir_name + "der2\\run_1.bin";
	FILE* file_d2 = fopen(file_d2_string.c_str(), "wb");
	
	
	fwrite(&yv_der[0], sizeof(vector<double>::value_type), xv.size(), file_d);
	fwrite(&yv_der2[0], sizeof(vector<double>::value_type), xv.size(), file_d2);
	
	/*for (int i = 0; i < xv.size(); i++)
	{
		file_d << i << "\t" << yv_der[i] << endl;
		file_d2 << i << "\t" << yv_der2[i] << endl;

		if (i % 50000 == 0)
		{
			cout << "progress ... " << (i * 100.0) / xv.size() << endl;
		}
	}*/

	long int t1_Calculate_derivative = GetTickCount();
	cout << "Calculate_derivative time is (in s) " << (t1_Calculate_derivative - t0_Calculate_derivative) / 1000.0 << endl;

	system("pause");
	exit(0);
}

void RootFit::CalculateFilterCoeff(int points)
{
	//cout << endl << "start Calculate filter coefficients" << endl;

	//Savitzky–Golay filter
	//order = 3
	C_i_s.clear();
	C_i_der.clear();
	C_i_der2.clear();

	int m = points;// from 8 to 

	//посчитать коэффициенты  C_i
	for (int i = (1 - m) / 2.0; i <= (m - 1) / 2.0; i++)
	{
		double numerator = 5 * (3 * pow(m, 4.0) - 18 * pow(m, 2.0) + 31)*i - 28 * (3 * pow(m, 2.0) - 7)*pow(i, 3.0);
		double denominator = m * (pow(m, 2.0) - 1) * (3 * pow(m, 4.0) - 39 * pow(m, 2.0) + 108) / 15.0;
		C_i_der.push_back(numerator / denominator);

		//numerator = -(3*pow(m, 2.0) - 7)*i + 20 * pow(i, 3.0);
		//denominator = m * (pow(m, 2.0) - 1)*(3 * pow(m, 4.0) - 39 * pow(m, 2.0) + 108) / 2520;

		numerator = 12 * m * pow(i, 2.0) - m * (pow(m, 2.0) - 1);
		denominator = pow(m, 2.0) * (pow(m, 2.0) - 1)*(pow(m, 2.0) - 4) / 30.0;

		C_i_der2.push_back(numerator / denominator);

		numerator = (3 * pow(m, 2.0) - 7 - 20 * pow(i, 2.0)) / 4.0;
		denominator = m * (pow(m, 2.0) - 4) / 3.0;
		C_i_s.push_back(numerator / denominator);

	}
	//cout << endl << "stop Calculate filter coefficients" << endl;
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

void RootFit::SaveGraphs(TObjArray &Hlist)
{



	TMultiGraph *mg = new TMultiGraph();
	mg->Add(gr);

	mg->Add(gr_front);

	mg->Add(gr_der);
	mg->Add(gr_der2);
	Hlist.Add(mg);

}


void RootFit::SetDispXY(double x, double y)
{
	//вектора с ошибками
	for (unsigned int j = 0; j < xv.size(); j++)
	{
		xverr.push_back(x);
		yverr.push_back(y);
	}
}




//вспомогательная функция
double RootFit::F(double t, double sigma, double tau)
{
	return TMath::Exp((sigma*sigma - 2 * t*tau) / (2 * tau*tau)) * (1 + TMath::Erf((t - sigma*sigma / tau) / (sigma * sqrt(2))));
}

//функция сигнала, без учета базавой линии
double RootFit::fitFunction_nobaseline(double *x, double *par)
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
Double_t RootFit::fitFunction(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0;
}

//сумма двух сигналов
double RootFit::fitFunction_2(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]);
}

//сумма трех сигналов
double RootFit::fitFunction_3(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]) + fitFunction_nobaseline(x, &par[11]);
}

//сумма 4-х сигналов
double RootFit::fitFunction_4(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]) + fitFunction_nobaseline(x, &par[11]) + fitFunction_nobaseline(x, &par[16]);
}

//сумма 5-и сигналов
double RootFit::fitFunction_5(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]) + fitFunction_nobaseline(x, &par[11]) + fitFunction_nobaseline(x, &par[16]) + fitFunction_nobaseline(x, &par[21]);
}

//сумма 6-и сигналов
double RootFit::fitFunction_6(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]) +
		fitFunction_nobaseline(x, &par[11]) + fitFunction_nobaseline(x, &par[16]) +
		fitFunction_nobaseline(x, &par[21]) + fitFunction_nobaseline(x, &par[26]);
}

//создает вектора: везде 0, а в точках time_front[i] - значение функции yv[i]
void  RootFit::CreateFrontGraph()
{

	xv_front.clear();
	yv_front.clear();


	for (int i = time_start_index; i <= time_finish_index; i++)
	{
		bool flag = false;
		for (int j = 0; j < time_front.size(); j++)
		{
			if (i == time_front[j])
			{
				flag = true;
			}
		}

		if (flag)
		{
			yv_front.push_back(yv[i]);
		}
		else
		{
			yv_front.push_back(0);
		}


		xv_front.push_back(xv[i]);

	}

}

void RootFit::CalculateAverageSignal(double time_dead_forward)
{
	cout << endl << "CalculateAverageSignal" << endl;

	ofstream file_amp_average_mode(Monostate::dir_name + "file_amp_average_mode.dat");

	bool flag = 1;
	double x_time;
	int num_of_signals = 0;

	vector<double> yv_average;
	for (int i = 0; i < 2 * int(time_dead_forward * 5); i++)
	{
		yv_average.push_back(0);
	}


	double time_dead_der = 5; // ns
	double threshold_1e = -0.01;
	double threshold_2e = -0.015;
	
	//бежать по времени
	for (unsigned int i = 0; i < xv.size(); i++)
	{
		if ((yv_der[i] < threshold_der /*&& yv[i] < threshold_amp_start*/) && flag && (i + int(time_dead_forward * 5)) < xv.size())
		{
			x_time = xv[i];
			flag = 0;

			bool flag_afp = 1;
			int x_time_afp = i;
			int total_num_pulsing = 0;
			

			//вычисление количества импульсов на отрезке 
			for (int j = i - int(time_dead_forward * 5); j < i + int(time_dead_forward * 5); j++)
			{
				if (yv_der[j] < threshold_der && flag_afp)
				{
					total_num_pulsing++;
					flag_afp = 0;
					x_time_afp = j;
				}

				if (yv_der[j] > threshold_der && (flag_afp == 0) && (xv[j] - xv[x_time_afp]) > time_dead_der)
				{
					flag_afp = 1;
				}
			}

			//cout << "total_num_pulsing " << total_num_pulsing << endl;

			// отбрасывание послеимпульсов
			double amplitude = 5000;
			if (total_num_pulsing == 1) 
			{
				for (int j = /*i - int(time_dead_forward * 5)*/ i; j < i + int(time_dead_forward * 5); j++) // нахождение максимальной амплитуды
				{
					if (yv[j] < amplitude)
						amplitude = yv[j];
				}

				TGraph *gr = new TGraph(int(time_dead_forward * 5) + int(time_dead_forward * 5), &xv[i - int(time_dead_forward * 5)], &yv[i - int(time_dead_forward * 5)]);
				TGraph *gr_der = new TGraph(int(time_dead_forward * 5) + int(time_dead_forward * 5), &xv[i - int(time_dead_forward * 5)], &yv_der[i - int(time_dead_forward * 5)]);
				TGraph *gr_der2 = new TGraph(int(time_dead_forward * 5) + int(time_dead_forward * 5), &xv[i - int(time_dead_forward * 5)], &yv_der2[i - int(time_dead_forward * 5)]);

				gr->SetMarkerColor(4);
				gr->SetMarkerStyle(kFullCircle);

				gr_der2->SetLineColor(7);

				for (int i = 0; i < gr_der->GetN(); i++) gr_der->GetY()[i] *= 50;
				for (int i = 0; i < gr_der2->GetN(); i++) gr_der2->GetY()[i] *= 500;

				TMultiGraph *mg = new TMultiGraph();
				mg->Add(gr);
				mg->Add(gr_der);
				mg->Add(gr_der2);

				
				
				file_amp_average_mode << abs(amplitude) << endl;

				if (amplitude > threshold_2e && amplitude < threshold_1e && yv[i - int(time_dead_forward * 5)] > (threshold_1e * 0.5) ) // проверка, что импульс одноэлектронный
				{

					Monostate::Hlist_reco_time_good.Add(mg);

					num_of_signals++;
					for (int j = -int(time_dead_forward * 5); j < int(time_dead_forward * 5); j++)// запись значений в вектор среднего
					{
						yv_average[j + int(time_dead_forward * 5)] += yv[i + j];
					}
				}
				else
				{
					Monostate::Hlist_reco_time_bad.Add(mg);
				}

			}

		}

		//разрешить искать сигнал, когда пройдет time_dead_forward нс после запуска
		if (yv_der[i] < threshold_der && flag == 0 && (xv[i] - x_time) > time_dead_forward)
		{
			flag = 1;
		}
	}

	ofstream file_average(Monostate::dir_name + "file_average.dat");
	for (int i = 0; i < 2 * int(time_dead_forward * 5); i++)
	{
		file_average << i*0.2 << "\t" << yv_average[i] / num_of_signals << endl;
	}

	cout << "number_of_pulsing = \t" << num_of_signals << endl;
}