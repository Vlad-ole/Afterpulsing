#include "RootFit.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TMath.h"
#include "Monostate.h"
#include <omp.h> 
#include "Math/MinimizerOptions.h"

vector<double> RootFit::yv_s;
vector<double> RootFit::yv_der;
vector<double> RootFit::yv_der2;

vector<double> RootFit::xverr;
vector<double> RootFit::yverr;
//vector<double> RootFit::yv_front;
//vector<double> RootFit::xv_front;

vector<double> RootFit::C_i_s;
vector<double> RootFit::C_i_der;
vector<double> RootFit::C_i_der2;


RootFit::RootFit()
{
	
}

void RootFit::Initialize(short int number_of_functions)
{
	this->number_of_functions = number_of_functions;
	
	gr = new TGraphErrors(time_finish_index - time_start_index, &xv[time_start_index], &yv[time_start_index], &xverr[time_start_index], &yverr[time_start_index]);
	gr->SetMarkerColor(4);
	gr->SetMarkerStyle(kFullCircle);

	gr_der = new TGraph(time_finish_index - time_start_index, &xv[time_start_index], &yv_der[time_start_index]);
	gr_der2 = new TGraph(time_finish_index - time_start_index, &xv[time_start_index], &yv_der2[time_start_index]);

	for (int i = 0; i<gr_der->GetN(); i++) gr_der->GetY()[i] *= 50;
	for (int i = 0; i<gr_der2->GetN(); i++) gr_der2->GetY()[i] *= 500;

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
	ROOT::Math::MinimizerOptions::SetDefaultStrategy(2);
}


RootFit::~RootFit()
{

}


void RootFit::SetParameters()
{
	const double A_start = 0.04;
	const double A_limit_l = 0.001;
	const double A_limit_h = 1;

	const double baseline_limit = 0.003;
	const double sigma = 1.64932;

	double time_first = xv[time_front[0]];

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




void RootFit::Print_dt_amp()
{
	//�������
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
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::time_i << fitFcn->GetParameter(1) << "\t" << fitFcn->GetParameter(0) << endl;
	}

	if (this->number_of_functions == 2)
	{
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(6) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;

		pr.first = fitFcn->GetParameter(1);
		pr.second = fitFcn->GetParameter(0);
		v_pairs.push_back(pr);
		
		pr.first = fitFcn->GetParameter(7);
		pr.second = fitFcn->GetParameter(6);
		v_pairs.push_back(pr);

		sort(v_pairs.begin(), v_pairs.end(), sort_pred());

		Monostate::time_i << v_pairs[0].first << "\t" << v_pairs[0].second << endl;
		Monostate::time_i << v_pairs[1].first << "\t" << v_pairs[1].second << endl;
		
		/*if (fitFcn->GetParameter(1) < fitFcn->GetParameter(7))
		{
			Monostate::time_i << fitFcn->GetParameter(1) << "\t" << fitFcn->GetParameter(0) << endl;
			Monostate::time_i << fitFcn->GetParameter(7) << "\t" << fitFcn->GetParameter(6) << endl;
		}
		else
		{
			Monostate::time_i << fitFcn->GetParameter(7) << "\t" << fitFcn->GetParameter(6) << endl;
			Monostate::time_i << fitFcn->GetParameter(1) << "\t" << fitFcn->GetParameter(0) << endl;
		}*/

	}

	if (this->number_of_functions == 3)
	{
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(6) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(11) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;	

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

		Monostate::time_i << v_pairs[0].first << "\t" << v_pairs[0].second << endl;
		Monostate::time_i << v_pairs[1].first << "\t" << v_pairs[1].second << endl;
		Monostate::time_i << v_pairs[2].first << "\t" << v_pairs[2].second << endl;

	}

	if (this->number_of_functions == 4)
	{
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(6) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(11) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(11 + 5) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;

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

		Monostate::time_i << v_pairs[0].first << "\t" << v_pairs[0].second << endl;
		Monostate::time_i << v_pairs[1].first << "\t" << v_pairs[1].second << endl;
		Monostate::time_i << v_pairs[2].first << "\t" << v_pairs[2].second << endl;
		Monostate::time_i << v_pairs[3].first << "\t" << v_pairs[3].second << endl;

	}

	if (this->number_of_functions == 5)
	{
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(6) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(11) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(11 + 5) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(11 + 10) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;

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

		Monostate::time_i << v_pairs[0].first << "\t" << v_pairs[0].second << endl;
		Monostate::time_i << v_pairs[1].first << "\t" << v_pairs[1].second << endl;
		Monostate::time_i << v_pairs[2].first << "\t" << v_pairs[2].second << endl;
		Monostate::time_i << v_pairs[3].first << "\t" << v_pairs[3].second << endl;
		Monostate::time_i << v_pairs[4].first << "\t" << v_pairs[4].second << endl;

	}
	
	//Monostate::amp_chi2_fnc1 << ->GetParameter(0) << "\t" << fitFcn_fnc2->GetChisquare() / (time_finish[i] - time_start_index) << endl;
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

	

}


void RootFit::FindStartStop()
{
	cout << endl << "Find start and stop" << endl;
	
	time_start.clear();
	time_finish.clear();
	
	bool flag = 1;
	double time_dead_1 = 5; // ns
	double time_dead_2 = 50; // ns

	double x_time;
	//����� ������ � ����� 
	for (unsigned int i = 0; i < xv.size(); i++)
	{
		if ((yv_der[i] < threshold_der) && flag)
		{
			x_time = xv[i];
			flag = 0;
			time_start.push_back(i);
		}

		//��������� ������ ������, ����� ������ ������ �� ����� � ������� 5 ��
		if (yv[i] > threshold_amp && flag == 0 && (xv[i] - x_time) > time_dead_1)
		{
			bool flag_2 = true;

			//cout << i << "\t" << yv_der.size() << "\t" << i + 50 * 5 << "\t" << bool(yv_der.size() < i + 50 * 5) << endl;

			//��������� ������ ������, ������ ���� ������� �� time_dead_2 [ns] ��� ������ ��������
			for (int j = i; j < i + time_dead_2 * 5; j++)
			{
				if (yv_der.size() > i + time_dead_2 * 5)
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

//����� ����� �������� �� ������� �� 2-� �����������. ���������� ������ ������� ����� � ��.
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

			if (yv_der2[i] > threshold_der2 && (i - x_time) > (time_dead_index) && flag == 0)
			{
				flag = 1;
			}
		}
	}

	cout << "num_of_signals = " << num_of_signals << endl;
}

//��������� ��������� ������� t_i. ���������� ������ ������� ����� � ��.
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
	
	//printf("thread %d \n \n", omp_get_thread_num());

	time_finish_index = time_finish[current_signal];

	bool flag = 1;
	int x_time = 0;

	//printf("thread %d iterration %d flag %d \n \n", omp_get_thread_num(), current_signal, flag);

	time_front.clear();

	int time_dead_index = time_dead * 5;


	//����� ��������� ��������� ��� ������ �������
	for (int j = time_start_index; j < time_finish[current_signal]; j++)
	{

		if (yv_der[j] < threshold_der && flag)
		{
			int min_index = min_element(yv_der.begin() + j, yv_der.begin() + j + time_dead_index) - yv_der.begin();

			time_front.push_back(min_index);
			flag = 0;
			x_time = j;
		}

		if (yv_der[j] > threshold_der && (j - x_time) > (time_dead_index))
		{
			flag = 1;
		}

	}
}


void RootFit::CalculateDer(int type, int points)
{
	cout << endl << "Calculate derivative" << endl;
	yv_der.clear();
	yv_der2.clear();
	yv_s.clear();


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


		int point_half = (points - 1) / 2.0;
		double value;
		for (unsigned int i = 0; i < xv.size(); i++)
		{
			if (i % 100000 == 0)
			{
				cout << "calculate derivative ... " << double(i) / xv.size() * 100 << " %" << endl;
			}
			
			if (i < point_half || i >(xv.size() - point_half - 1))
			{
				yv_s.push_back(0);
				yv_der.push_back(0);
				yv_der2.push_back(0);
			}
			else
			{
				
				value = 0;
				for (int j = 0; j < C_i_s.size(); j++)
				{
					value += C_i_s[j] * yv[i - point_half + j];
				}
				yv_s.push_back(value);
				
				
				value = 0;
				for (int j = 0; j < C_i_der.size(); j++)
				{
					value += C_i_der[j] * yv[i - point_half + j];
				}
				yv_der.push_back(value);

				value = 0;
				for (int j = 0; j < C_i_der2.size(); j++)
				{
					value += 2 * C_i_der2[j] * yv[i - point_half + j];
				}
				yv_der2.push_back(value);

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
}

void RootFit::CalculateFilterCoeff(int points)
{
	//cout << endl << "start Calculate filter coefficients" << endl;
	
	//Savitzky�Golay filter
	//order = 3
	C_i_s.clear();
	C_i_der.clear();
	C_i_der2.clear();

	int m = points;// from 8 to 

	//��������� ������������  C_i
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
	printf("thread %d ; %d\n \n", omp_get_thread_num(), &gr);
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
	//printf("thread %d SaveGraphs itteration \n \n", omp_get_thread_num());

	TMultiGraph *mg = new TMultiGraph();
	mg->Add(gr);

	mg->Add(gr_front);

	mg->Add(gr_der);
	mg->Add(gr_der2);
	Hlist.Add(mg);

}


void RootFit::SetDispXY(double x, double y)
{
	//������� � ��������
	for (unsigned int j = 0; j < xv.size(); j++)
	{
		xverr.push_back(x);
		yverr.push_back(y);
	}
}




//��������������� �������
double RootFit::F(double t, double sigma, double tau)
{
	return TMath::Exp((sigma*sigma - 2 * t*tau) / (2 * tau*tau)) * (1 + TMath::Erf((t - sigma*sigma / tau) / (sigma * sqrt(2))));
}

//������� �������, ��� ����� ������� �����
double RootFit::fitFunction_nobaseline(double *x, double *par)
{
	//�������� ���������
	const double A = par[0];
	const double t_0 = par[1];
	const double tau_rec_fast = par[2];
	const double tau_rise = par[3];
	const double sigma = par[4];
	
	//��������������� ����������
	const double t = x[0] - par[1];
	const double tau_total_fast = (tau_rec_fast * tau_rise) / (tau_rec_fast + tau_rise);
	
	return -(A / 2) * (F(t, sigma, tau_rec_fast) - F(t, sigma, tau_total_fast));
}

//�������, ������� ���� ����������
Double_t RootFit::fitFunction(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0;
}

//����� ���� ��������
double RootFit::fitFunction_2(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]);
}

//����� ���� ��������
double RootFit::fitFunction_3(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]) + fitFunction_nobaseline(x, &par[11]);
}

//����� 4-� ��������
double RootFit::fitFunction_4(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]) + fitFunction_nobaseline(x, &par[11]) + fitFunction_nobaseline(x, &par[16]);
}

//����� 5-� ��������
double RootFit::fitFunction_5(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]) + fitFunction_nobaseline(x, &par[11]) + fitFunction_nobaseline(x, &par[16]) + fitFunction_nobaseline(x, &par[21]);
}

//����� 6-� ��������
double RootFit::fitFunction_6(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]) + 
		fitFunction_nobaseline(x, &par[11]) + fitFunction_nobaseline(x, &par[16]) + 
		fitFunction_nobaseline(x, &par[21]) + fitFunction_nobaseline(x, &par[26]);
}

//������� �������: ����� 0, � � ������ time_front[i] - �������� ������� yv[i]
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