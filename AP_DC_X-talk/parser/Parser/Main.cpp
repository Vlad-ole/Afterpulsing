#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <Windows.h>

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

double RootFit::threshold_der2;
double RootFit::threshold_der;
double RootFit::threshold_amp;

vector<double> RootFit::xv;
vector<double> RootFit::yv;

vector<int> RootFit::time_start;
vector<int> RootFit::time_finish;
vector<int> RootFit::time_front;

int RootFit::current_signal;

//> "D:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\fit_log.txt"

int main()
{
	long int before = GetTickCount();
	
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

	//������ ����
	while (!feof(f))
	{
		fscanf(f, "%lf %lf\n", &x, &y);
		xv.push_back(counter*0.2); // in ns
		yv.push_back(y);

		counter++;

		//�������� ��������
		if (counter % 10000 == 0)
		{
			cout << "read file " << counter / double(2E7) * 100 << endl;
		}

		// ���������� ���������� ����������. ����� ��-�� �������� ������� �����
		if (xv.size() % Monostate::rec_lenght == 0)
		{

			RootFit::xv = xv; // �������� ������� ����� rec_lenght � ����� RootFit
			RootFit::yv = yv;
			
			RootFit::CalculateDer(1, 51); // ��������� ����������� �� ������ (����� ����� ������ ���� ��������)
			
			RootFit::threshold_der2 = -1E-5;
			RootFit::threshold_der = -2E-4;
			RootFit::threshold_amp = -0.001;
			RootFit::FindStartStop(); // ����� ������ � ����� ����� ��������
			
			
			RootFit::SetDispXY(0, 0.00113151);// �������� ������� ����� rec_lenght xverr � yverr ���������� ������

			RootFit::time_shit = 100; // ������ �������� �� ������� ��� ����� ������� ����� (� ������)

			for (unsigned int i = 0; i < RootFit::time_finish.size(); i++)
			{
				long int temp = GetTickCount();
				cout << endl << "calculate fit ... " << i + 1 << " run time is (in s) \t" << (temp - before) / 1000.0 << endl;
								
				RootFit::current_signal = i;	
				RootFit::CalculateStartParameters(5/*5*/);//��������� ��������� ���������. �������� - ������� ����� ����������� � ��	
				RootFit::CalculateNumOfSignals(3);
				RootFit::CreateFrontGraph();

				RootFit *Fit_single = new RootFit(1);
				Fit_single->SetParameters();
				Fit_single->DoFit();
	
				////�������� ������� � �������� ������
				//bool flag_temp = Fit_single->fitFcn->GetParameter(0) < 0.03;
				//if (flag_temp && Fit_single->GetChi2PerDof() < Monostate::chi2_per_dof_th)
				//{
				//	Fit_single->SaveGraphs(Monostate::Hlist_test);
				//}
				
				
				//�������� ��� �������
				Fit_single->SaveGraphs(Monostate::Hlist_f1);


				//�������� ������� � ������ Chi2 ����� ���� ����� ��������
				if (Fit_single->GetChi2PerDof() > Monostate::chi2_per_dof_th)
				{
					Fit_single->SaveGraphs(Monostate::Hlist_f1_bad);
				}
				
				
				if (Fit_single->GetChi2PerDof() < Monostate::chi2_per_dof_th)
				{
					//�������� ������� � ������� Chi2 ����� ���� ����� ��������
					Fit_single->SaveGraphs(Monostate::Hlist_f1_good);

					//�������� ��� ������� ����� ���� ����� ��������
					Fit_single->SaveGraphs(Monostate::Hlist_f2);
				}

				if (Fit_single->GetChi2PerDof() < Monostate::chi2_per_dof_th)
					Fit_single->Print_dt_amp();
				

				if (Fit_single->GetChi2PerDof() > Monostate::chi2_per_dof_th)
				{
					cout << "\t double ... " << endl;

					RootFit *Fit_double = new RootFit(2);
					Fit_double->SetParameters();
					Fit_double->DoFit();


					////�������� ������� � �������� ������
					//bool flag_temp = Fit_double->fitFcn->GetParameter(0) < 0.03 || Fit_double->fitFcn->GetParameter(6) < 0.03;
					//if (flag_temp && Fit_double->GetChi2PerDof() < Monostate::chi2_per_dof_th)
					//{
					//	Fit_double->SaveGraphs(Monostate::Hlist_test);
					//}

					//�������� ��� ������� ����� ���� ����� ��������
						Fit_double->SaveGraphs(Monostate::Hlist_f2);

					//�������� ������� � ������ Chi2 ����� ���� ����� ���������
					if (Fit_double->GetChi2PerDof() > Monostate::chi2_per_dof_th)
					{
						Fit_double->SaveGraphs(Monostate::Hlist_f2_bad);
					}

					
					if (Fit_double->GetChi2PerDof() < Monostate::chi2_per_dof_th)
					{
						//�������� ������� � ������� Chi2 ����� ���� ����� ���������
						Fit_double->SaveGraphs(Monostate::Hlist_f2_good);						
					}

									
					////������� ��������� ��� � ������ ���������� ����������� � �������� �������
					//if (Fit_double->GetChi2PerDof() > Monostate::chi2_per_dof_th && RootFit::time_front.size() > 2)
					//{
					//	cout << "\t double new time " << endl;

					//	Fit_double->SaveGraphs(Monostate::Hlist_f2_bad_old_time);

					//	Fit_double->SetParameters(xv[RootFit::time_front[0]], xv[RootFit::time_front[2]]);
					//	Fit_double->DoFit();
					//							
					//	Fit_double->SaveGraphs(Monostate::Hlist_f2_bad_new_time);
					//}
					if (Fit_double->GetChi2PerDof() < Monostate::chi2_per_dof_th)
						Fit_double->Print_dt_amp();


					if (Fit_double->GetChi2PerDof() > Monostate::chi2_per_dof_th)
					{
						cout << "\t \t triple ... " << endl;
				
						RootFit *Fit_triple = new RootFit(3);
						Fit_triple->SetParameters();
						Fit_triple->DoFit();

						////������� ��������� ��� � ������ ���������� �����������
						//if (Fit_triple->GetChi2PerDof() > Monostate::chi2_per_dof_th)
						//{
						//	if (RootFit::time_front.size() == 1)
						//	{
						//		/*RootFit *Fit_triple_new_param = new RootFit(time_start_index, time_finish[i], xv, yv, xverr, yverr, 3, time_front_ns);
						//		Fit_triple_new_param->SetParameters(time_front_ns[0], time_front_ns[2], time_front_ns[3]);
						//		Fit_triple_new_param->DoFit();
						//		Fit_triple_new_param->SaveGraphs(Monostate::Hlist_f3_bad_new_time);*/

						//		
						//		Fit_triple->SetParameters(xv[RootFit::time_front[0]], xv[RootFit::time_front[0]], xv[RootFit::time_front[0]]);
						//		Fit_triple->DoFit();

						//		Fit_triple->SaveGraphs(Monostate::Hlist_f3_bad_new_time);

						//	}
						//}

						//�������� ������� � �������� ������
						bool flag_temp = Fit_triple->fitFcn->GetParameter(0) < 0.03 || Fit_triple->fitFcn->GetParameter(6) < 0.03 || Fit_triple->fitFcn->GetParameter(11) < 0.03;
						if (flag_temp && Fit_triple->GetChi2PerDof() < Monostate::chi2_per_dof_th)
						{
							Fit_triple->SaveGraphs(Monostate::Hlist_test);
						}

						//�������� �������, ����� ���� ����� ���������
						Fit_triple->SaveGraphs(Monostate::Hlist_f3);

						//�������� ������� � ������ Chi2 ����� ���� ����� ���������
						if (Fit_triple->GetChi2PerDof() > Monostate::chi2_per_dof_th)
						{
							Fit_triple->SaveGraphs(Monostate::Hlist_f3_bad);
						}

						//�������� ������� � ������� Chi2 ����� ���� ����� ���������
						if (Fit_triple->GetChi2PerDof() < Monostate::chi2_per_dof_th)
						{
							Fit_triple->SaveGraphs(Monostate::Hlist_f3_good);
						}

						if (Fit_triple->GetChi2PerDof() < Monostate::chi2_per_dof_th)
							Fit_triple->Print_dt_amp();

						//if (fitFcn->GetParameter(0) > 0.02)
						//{
						//	
						//	time_test << fitFcn_fnc3->GetParameter(1) << "\t" << fitFcn_fnc3->GetParameter(7) << "\t" << fitFcn_fnc3->GetParameter(13) << endl;

						//}

						if (Fit_triple->GetChi2PerDof() > Monostate::chi2_per_dof_th)
						{
							cout << "\t \t \t quadruple ... " << endl;
							RootFit *Fit_quadruple = new RootFit(4);
							Fit_quadruple->SetParameters();
							Fit_quadruple->DoFit();

							


							//�������� ������� � ������ Chi2 ����� ���� 4 ���������
							if (Fit_quadruple->GetChi2PerDof() > Monostate::chi2_per_dof_th)
							{
								Fit_quadruple->SaveGraphs(Monostate::Hlist_f4_bad);
							}

							//�������� ������� � ������� Chi2 ����� ���� 4 ���������
							if (Fit_quadruple->GetChi2PerDof() < Monostate::chi2_per_dof_th)
							{
								Fit_quadruple->SaveGraphs(Monostate::Hlist_f4_good);
							}

							if (Fit_quadruple->GetChi2PerDof() < Monostate::chi2_per_dof_th)
								Fit_quadruple->Print_dt_amp();


							if (Fit_quadruple->GetChi2PerDof() > Monostate::chi2_per_dof_th)
							{
								cout << "\t \t \t \t quintuple ... " << endl;
								RootFit *Fit_quintuple = new RootFit(5);
								Fit_quintuple->SetParameters();
								Fit_quintuple->DoFit();

								//if (Fit_quintuple->GetChi2PerDof() < Monostate::chi2_per_dof_th)
									Fit_quintuple->Print_dt_amp();


								//�������� ������� � ������ Chi2 ����� ���� 5 ���������
								if (Fit_quintuple->GetChi2PerDof() > Monostate::chi2_per_dof_th)
								{
									Fit_quintuple->SaveGraphs(Monostate::Hlist_f5_bad);
								}

								//�������� ������� � ������� Chi2 ����� ���� 5 ���������
								if (Fit_quintuple->GetChi2PerDof() < Monostate::chi2_per_dof_th)
								{
									Fit_quintuple->SaveGraphs(Monostate::Hlist_f5_good);
								}


							}

						}

					}


				}


			}

			//RootFit::Print_dt_amp();

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


	
	ofstream time_delta(Monostate::dir_name + "time_delta.dat");
	ofstream file_dt(Monostate::dir_name + "dt.dat");
	ofstream file_amp(Monostate::dir_name + "amp.dat");

	string string_time_i = Monostate::dir_name + "time_i.dat";
	FILE *f2 = fopen(string_time_i.c_str(), "r");

	double x_old;
	bool flag = 0;
	while (!feof(f2))
	{
		fscanf(f2, "%lf %lf\n", &x, &y);

		if (flag)
		{
			time_delta << x - x_old << "\t" << y << endl;
			file_dt << x - x_old << endl;
			file_amp << y << endl;
		}

		x_old = x;

		flag = 1;
	}
		
	long int after = GetTickCount();
	cout << endl << "run time (in ms) \t " <<after - before << endl;
	cout << endl << "run time (in s) \t " << (after - before) / 1000.0 << endl;
	cout << endl << "run time (in m) \t " << (after - before) / (1000.0 * 60) << endl;

	system("pause");
	return 0;
}