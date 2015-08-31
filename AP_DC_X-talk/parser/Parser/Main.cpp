#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "TRandom.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TFile.h"
//#include  "gStyle.h"
#include "RootFit.h"
#include "Monostate.h"
//#include "Math.h"

using namespace std;

vector<double> xv;
vector<double> yv;



int RootFit::time_start_index;
int RootFit::time_finish_index;

int RootFit::time_shit;

double RootFit::threshold_der;
double RootFit::threshold_amp;

vector<double> RootFit::xv;
vector<double> RootFit::yv;

vector<int> RootFit::time_start;
vector<int> RootFit::time_finish;
vector<int> RootFit::time_front;

int RootFit::current_signal;

int main()
{
	//RootFit::fitFunction();
	ofstream file_test(Monostate::dir_name + "test.dat");
	
	Double_t x, y;
	FILE *f = fopen(Monostate::raw_name.c_str(), "r");

	int counter = 0;
	int counter_rec_length = 0;

	if (f == NULL)
	{
		cout << "Can't open this file: " << Monostate::raw_name << endl;
		system("pause");
	}

	//читать файл
	while (!feof(f))
	{
		fscanf(f, "%lf %lf\n", &x, &y);
		xv.push_back(counter*0.2); // in ns
		yv.push_back(y);

		counter++;

		//показать прогресс
		if (counter % 10000 == 0)
		{
			cout << "read file " << counter / double(2E7) * 100 << endl;
		}

		// обработать записанную информацию. Нужно из-за большого размера файла
		if (xv.size() % Monostate::rec_lenght == 0)
		{

			RootFit::xv = xv; // передать вектора длины rec_lenght в класс RootFit
			RootFit::yv = yv;
			
			RootFit::CalculateDer(1, 9); // посчитать производную по данным
			
			
			RootFit::threshold_der = (-2E-4);
			RootFit::threshold_amp = -0.001;
			RootFit::FindStartStop(); // найти начало и конец суммы сигналов
			
			
			RootFit::SetDispXY(0, 0.00113151);// записать вектора длины rec_lenght xverr и yverr значениеми ошибок

			RootFit::time_shit = 100; // задать смещение по времени для учета базовой линии (в точках)

			for (unsigned int i = 0; i < RootFit::time_finish.size(); i++)
			{
				cout << "calculate fit ... " << i << endl;
								
				RootFit::current_signal = i;	
				RootFit::CalculateStartParameters(5);//вычислить стартовые параметры. Параметр - мертвое время производной в нс				
				RootFit::CreateFrontGraph();

				RootFit *Fit_single = new RootFit(1);
				Fit_single->SetParameters(xv[RootFit::time_front[0]]);
				Fit_single->DoFit();
	
				//записать все графики
				Fit_single->SaveGraphs(Monostate::Hlist_f1);

				//записать графики с плохим Chi2 после фита одной функцией
				if (Fit_single->GetChi2PerDof() > Monostate::chi2_per_dof_th)
				{
					Fit_single->SaveGraphs(Monostate::Hlist_f1_bad);
				}
				
				//записать графики с хорошим Chi2 после фита одной функцией
				if (Fit_single->GetChi2PerDof() < Monostate::chi2_per_dof_th)
				{
					Fit_single->SaveGraphs(Monostate::Hlist_f1_good);
				}

	/*
				if (fitFcn->GetParameter(0) > 0.02 && fitFcn->GetChisquare() / (time_finish[i] - time_start_index) < chi2_per_dof)
				{
					amp_chi2_fnc2 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / (time_finish[i] - time_start_index) << endl;
					amp_chi2_fnc3 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / (time_finish[i] - time_start_index) << endl;
					time_i << fitFcn->GetParameter(1) << "\t" << fitFcn->GetParameter(0) << endl;
					Hlist_amp_cut.Add(gr);
				}*/

				if (Fit_single->GetChi2PerDof() > Monostate::chi2_per_dof_th)
				{
					cout << "\t double ... " << endl;

					RootFit *Fit_double = new RootFit(2);
					if (RootFit::time_front.size() > 1)
						Fit_double->SetParameters(xv[RootFit::time_front[0]], xv[RootFit::time_front[1]]);
					else
						Fit_double->SetParameters(xv[RootFit::time_front[0]], xv[RootFit::time_front[0]]);

					Fit_double->DoFit();

					//записать графики, после фита двумя функциями
					Fit_double->SaveGraphs(Monostate::Hlist_f2);

					//записать графики с плохим Chi2 после фита двумя функциями
					if (Fit_double->GetChi2PerDof() > Monostate::chi2_per_dof_th)
					{
						Fit_double->SaveGraphs(Monostate::Hlist_f2_bad);
					}

					//записать графики с хорошим Chi2 после фита двумя функциями
					if (Fit_double->GetChi2PerDof() < Monostate::chi2_per_dof_th)
					{
						Fit_double->SaveGraphs(Monostate::Hlist_f2_good);
					}


									
					//сделать повторный фит с новыми начальными параметрами и записать графики
					if (Fit_double->GetChi2PerDof() > Monostate::chi2_per_dof_th && RootFit::time_front.size() > 2)
					{
						cout << "\t double new time " << endl;

						Fit_double->SaveGraphs(Monostate::Hlist_f2_bad_old_time);

						Fit_double->SetParameters(xv[RootFit::time_front[0]], xv[RootFit::time_front[2]]);
						Fit_double->DoFit();
												
						Fit_double->SaveGraphs(Monostate::Hlist_f2_bad_new_time);
					}


					if (Fit_double->GetChi2PerDof() > Monostate::chi2_per_dof_th)
					{
						cout << "\t \t triple ... " << endl;
				
						RootFit *Fit_triple = new RootFit(3);
						if (RootFit::time_front.size() > 2)
						{
							Fit_triple->SetParameters(xv[RootFit::time_front[0]], xv[RootFit::time_front[1]], xv[RootFit::time_front[2]]);
						}

						if (RootFit::time_front.size() == 2)
						{
							Fit_triple->SetParameters(xv[RootFit::time_front[0]], xv[RootFit::time_front[0]], xv[RootFit::time_front[0]]);
						}
						
						if (RootFit::time_front.size() == 1)
						{
							Fit_triple->SetParameters(xv[RootFit::time_front[0]], xv[RootFit::time_front[0]], xv[RootFit::time_front[0]]);
						}

						Fit_triple->DoFit();

						//сделать повторный фит с новыми начальными параметрами
						if (Fit_triple->GetChi2PerDof() > Monostate::chi2_per_dof_th)
						{
							if (RootFit::time_front.size() == 1)
							{
								/*RootFit *Fit_triple_new_param = new RootFit(time_start_index, time_finish[i], xv, yv, xverr, yverr, 3, time_front_ns);
								Fit_triple_new_param->SetParameters(time_front_ns[0], time_front_ns[2], time_front_ns[3]);
								Fit_triple_new_param->DoFit();
								Fit_triple_new_param->SaveGraphs(Monostate::Hlist_f3_bad_new_time);*/

								
								Fit_triple->SetParameters(xv[RootFit::time_front[0]], xv[RootFit::time_front[0]], xv[RootFit::time_front[0]]);
								Fit_triple->DoFit();
								Fit_triple->SaveGraphs(Monostate::Hlist_f3_bad_new_time);

							}
						}


						//записать графики, после фита тремя функциями
						Fit_triple->SaveGraphs(Monostate::Hlist_f3);

						//записать графики с плохим Chi2 после фита тремя функциями
						if (Fit_triple->GetChi2PerDof() > Monostate::chi2_per_dof_th)
						{
							Fit_triple->SaveGraphs(Monostate::Hlist_f3_bad);
						}

						//записать графики с хорошим Chi2 после фита тремя функциями
						if (Fit_triple->GetChi2PerDof() < Monostate::chi2_per_dof_th)
						{
							Fit_triple->SaveGraphs(Monostate::Hlist_f3_good);
						}

						//if (fitFcn->GetParameter(0) > 0.02)
						//{
						//	
						//	time_test << fitFcn_fnc3->GetParameter(1) << "\t" << fitFcn_fnc3->GetParameter(7) << "\t" << fitFcn_fnc3->GetParameter(13) << endl;

						//}

						//if (Fit_triple->GetChi2PerDof() > chi2_per_dof)
						//{
						//	cout << "\t \t \t quadruple ... " << endl;

						//	//записать графики, с большИм Chi2/Dof после фита тремя функциями
						//	Fit_triple->SaveGraphs(Monostate::Hlist_4);
						//}

					}


				}


			}

			xv.clear();
			yv.clear();

			//yv_der.clear();


			counter_rec_length++;
			cout << "counter_rec_length = " << counter_rec_length << endl;
		}

		if (counter_rec_length == 1)
			break;

	}

	Monostate::SaveHlists();
	

	system("pause");
	return 0;
}