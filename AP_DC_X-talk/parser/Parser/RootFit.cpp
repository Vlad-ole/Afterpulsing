#include "RootFit.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TMath.h"
#include "Monostate.h"

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

RootFit::RootFit(short int number_of_functions)
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

	
	
	//cout << xv_front.size() << "\t" << yv_front.size() << endl;

	
	//for (int i = 0; i < xv_front.size(); i++)
	//{
	//	cout << xv_front[i] << "\t" << yv_front[i] << "\t" << xv_front.size() << endl;
	//}

	//system("pause");

	gr_front = new TGraph(time_finish_index - time_start_index, &xv_front[0], &yv_front[0]);// problem
	gr_front->SetMarkerColor(6);
	gr_front->SetMarkerStyle(kFullCircle);
	gr_front->SetMarkerSize(1);

	if (number_of_functions == 1)
		fitFcn = new TF1("fitFcn", fitFunction, xv[time_start_index], xv[time_finish_index], 6);
	if (number_of_functions == 2)
		fitFcn = new TF1("fitFcn", fitFunction_2, xv[time_start_index], xv[time_finish_index], 6 + 5);
	if (number_of_functions == 3)
		fitFcn = new TF1("fitFcn", fitFunction_3, xv[time_start_index], xv[time_finish_index], 6 + 5 + 5);
}


RootFit::~RootFit()
{

}


void RootFit::SetParameters(double time_first, double time_second, double time_third)
{
	const double A_start = 0.04;
	const double A_limit_l = 0.001;
	const double A_limit_h = 1;

	const double baseline_limit = 0.003;
	const double sigma = 1.64932;
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
		}

	}

	//fitFcn->Modify();
	//fitFcn->ReleaseParameter();
	//fitFcn->Update();

}

void RootFit::Print_dt_amp()
{
	
	if (fitFcn->GetChisquare() / fitFcn->GetNDF() < Monostate::chi2_per_dof_th)
	{
		Monostate::amp_chi2_fnc1 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF()<< endl;
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



void RootFit::FindStartStop()
{
	bool flag = 1;


	double x_time;
	//поиск начала и конца 
	for (unsigned int i = 0; i < xv.size(); i++)
	{
		if ((yv_der[i] < threshold_der) && flag)
		{
			x_time = xv[i];
			flag = 0;
			time_start.push_back(i);
		}

		//разрешить искать сигнал, когда сигнал дойдет до шумов
		if (yv[i] > threshold_amp && flag == 0 && (xv[i] - x_time) > 5)
		{
			time_finish.push_back(i);
			flag = 1;
		}
	}
}

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

	int time_dead_index = time_dead / 0.2;

	//найти стартовые параметры дл€ начала сигнала
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
		file_d << xv[i] << "\t" << yv_der[i] * 10 << endl;
		file_d2 << xv[i] << "\t" << yv_der2[i] * 50 << endl;
	}

	exit(0);*/
}

void RootFit::CalculateFilterCoeff(int points)
{
	//SavitzkyЦGolay filter
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




//вспомогательна€ функци€
double RootFit::F(double t, double sigma, double tau)
{
	return TMath::Exp((sigma*sigma - 2 * t*tau) / (2 * tau*tau)) * (1 + TMath::Erf((t - sigma*sigma / tau) / (sigma * sqrt(2))));
}

//функци€ сигнала, без учета базавой линии
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

//функци€, которой буду фитировать
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