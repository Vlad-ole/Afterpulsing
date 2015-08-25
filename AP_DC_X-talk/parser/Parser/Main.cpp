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


using namespace std;


int main()
{
	//RootFit::fitFunction();
	vector<double> xv;
	vector<double> yv;

	vector<double> xverr;
	vector<double> yverr;

	vector<double> xv_der_position;
	vector<double> yv_der_local;

	vector<double> yv_der;

	Double_t x, y;
	FILE *f = fopen(Monostate::raw_name.c_str(), "r");

	int counter = 0;
	int counter_rec_length = 0;

	double x_time;

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

			//--------------------------------------------------------------------
			//caclulate derivative
			for (unsigned int i = 0; i < xv.size(); i++)
			{
				if (i < Monostate::step)
				{
					yv_der.push_back(0);
				}
				else
				{
					yv_der.push_back(yv[i] - yv[i - Monostate::step]);
				}

			}
			//--------------------------------------------------------------------


			bool flag = 1;

			vector<int> time_start;
			vector<int> time_finish;

			//поиск начала и конца 
			for (unsigned int i = 0; i < xv.size(); i++)
			{
				if ((yv_der[i] < Monostate::threshold_der) && flag)
				{
					xv_der_position.clear();
					x_time = xv[i];
					flag = 0;
					time_start.push_back(i);
				}

				//разрешить искать сигнал, когда сигнал дойдет до шумов
				if (yv[i] > Monostate::threshold && flag == 0 && (xv[i] - x_time) > 5)
				{
					time_finish.push_back(i);
					flag = 1;
				}
			}			

			//вектора с ошибками
			for (unsigned int j = 0; j < xv.size(); j++)
			{
				xverr.push_back(0);
				yverr.push_back(Monostate::disp);
			}



			//задать параметры отбора и фита
			//---------------------------------------------
			//double A_start = 0.05;
			double chi2_per_dof = 3;

			/*int avg_points = 50;
			for (int j = i - time_pre; j < (i - time_pre + avg_points); j++)
			{
			base_line += yv[j];
			}

			base_line /= double(avg_points);*/
			//---------------------------------------------

			flag = 1;
			vector<double> time_front_ns;
			for (unsigned int i = 0; i < time_finish.size(); i++)
			{
				cout << "calculate fit ... " << i << endl;

				time_front_ns.clear();

				int baseline_shit = 50; // 10 ns
				int time_start_index = time_start[i] - baseline_shit;

				//найти стартовые параметры для начала сигнала
				for (int j = time_start_index; j < time_finish[i]; j++)
				{
					if (yv_der[j] < Monostate::threshold_der && flag)
					{
						time_front_ns.push_back(xv[j]);
						flag = 0;
						x_time = xv[j];
					}

					if (yv_der[j] > Monostate::threshold_der && (xv[j] - x_time) > 5)
					{
						flag = 1;
					}

				}				
				
				RootFit *Fit_single = new RootFit(time_start_index, time_finish[i], xv, yv, xverr, yverr, 1);
				Fit_single->SetParameters(xv[time_start_index], xv[time_finish[i]], time_front_ns);
				Fit_single->DoFit();
	
				//записать все графики
				Fit_single->SaveGraphs(Monostate::Hlist_f1);

				//записать графики с плохим Chi2 после фита одной функцией
				if (Fit_single->GetChi2PerDof() > chi2_per_dof)
				{
					Fit_single->SaveGraphs(Monostate::Hlist_f1_bad);
				}
				
				

	/*
				if (fitFcn->GetParameter(0) > 0.02 && fitFcn->GetChisquare() / (time_finish[i] - time_start_index) < chi2_per_dof)
				{
					amp_chi2_fnc2 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / (time_finish[i] - time_start_index) << endl;
					amp_chi2_fnc3 << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / (time_finish[i] - time_start_index) << endl;
					time_i << fitFcn->GetParameter(1) << "\t" << fitFcn->GetParameter(0) << endl;
					Hlist_amp_cut.Add(gr);
				}*/

				if (Fit_single->GetChi2PerDof() > chi2_per_dof)
				{
					cout << "\t double ... " << endl;

					RootFit *Fit_double = new RootFit(time_start_index, time_finish[i], xv, yv, xverr, yverr, 2);
					Fit_double->SetParameters(xv[time_start_index], xv[time_finish[i]], time_front_ns);
					Fit_double->DoFit();

					//записать графики, после фита двумя функциями
					Fit_double->SaveGraphs(Monostate::Hlist_f2);

					//записать графики с плохим Chi2 после фита двумя функциями
					if (Fit_double->GetChi2PerDof() > chi2_per_dof)
					{
						Fit_double->SaveGraphs(Monostate::Hlist_f2_bad);
					}

					if (Fit_double->GetChi2PerDof() > chi2_per_dof)
					{
						cout << "\t \t triple ... " << endl;
				
						RootFit *Fit_triple = new RootFit(time_start_index, time_finish[i], xv, yv, xverr, yverr, 3);
						Fit_triple->SetParameters(xv[time_start_index], xv[time_finish[i]], time_front_ns);
						Fit_triple->DoFit();

						//записать графики, после фита тремя функциями
						Fit_triple->SaveGraphs(Monostate::Hlist_f3);

						//записать графики с плохим Chi2 после фита тремя функциями
						if (Fit_double->GetChi2PerDof() > chi2_per_dof)
						{
							Fit_double->SaveGraphs(Monostate::Hlist_f3_bad);
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

			xverr.clear();
			yverr.clear();

			yv_der.clear();


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