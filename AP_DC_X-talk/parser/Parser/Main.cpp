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

//vector<double> xv;
//vector<double> yv;


int RootFit::time_start_index;
int RootFit::time_finish_index;

int RootFit::time_shit;

double RootFit::threshold_der2;
double RootFit::threshold_der;
double RootFit::threshold_amp;
double RootFit::threshold_amp_start;

vector<double> RootFit::xv;
vector<double> RootFit::yv;

vector<int> RootFit::time_start;
vector<int> RootFit::time_finish;
vector<int> RootFit::time_front;

int RootFit::current_signal;

double *yv_heap;

//> "D:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\fit_log.txt"

int main()
{
	long int before = GetTickCount();

	ofstream file_test(Monostate::dir_name + "test.dat");

	Double_t x, y;
	FILE *f = fopen(Monostate::raw_name.c_str(), "r");
	FILE *f_der = fopen(Monostate::der_name.c_str(), "rb");
	FILE *f_der2 = fopen(Monostate::der2_name.c_str(), "rb");

	int counter = 0;
	int counter_rec_length = 0;

	if (f == NULL)
	{
		cout << "Can't open this file: " << Monostate::raw_name << endl;
		system("pause");
	}

	//читать файл
	int yv_size;
	fread(&yv_size, sizeof(int), 1, f);
	cout << "file length = " << yv_size << endl;

	/*yv_heap = new double[yv_size];
	double *dP = &yv_heap[0];
	
	const int N = 10000;
	while (!feof(f))
	{
		fread(dP, sizeof(double), N, f);
		dP += N;
	}

	for (int i = 0; i < yv_size; i++)
	{
		xv.push_back(i * 0.2);
		yv.push_back(yv_heap[i]);
	}*/

	int yv_size_new = yv_size * 0.92;
	cout << "file length cutted= " << yv_size_new << endl;

	RootFit::yv.resize(yv_size_new);
	fread(&RootFit::yv[0], sizeof(vector<double>::value_type), yv_size_new, f);
	
	RootFit::xv.resize(yv_size_new);
	for (int i = 0; i < yv_size_new; i++)
	{
		RootFit::xv[i] = 0.2 * i;
	}


		////показать прогресс
		//if (counter % 50000 == 0)
		//{
		//	long int read_file = GetTickCount();
		//	cout << "read file " << counter / double(2E7) * 100 << "speed is \t " << counter / ((read_file - before) / 1000.0) << " lines / s" << endl;
		//}

		// обработать записанную информацию. Нужно из-за большого размера файла
		//if (xv.size() % Monostate::rec_lenght == 0)
		{
			
			long int t1_read_file = GetTickCount();
			cout << endl << "Time of file reading is (in s) " << (t1_read_file - before) / 1000.0 << endl;
			
			/*ofstream file_shift(Monostate::dir_name + "file_shift.dat");
			int time_shift = 10 * 5; 
			double A = 1;
			for (int i = time_shift; i < xv.size(); i++)
			{
				file_shift << xv[i] << "\t" << yv[i] - A * yv[i - time_shift] << endl;
			}

			exit(0);*/
			
			//RootFit::xv = xv; // передать вектора длины rec_lenght в класс RootFit
			//RootFit::yv = yv;
			//RootFit::temp_time_i = 0;

			RootFit::yv_der.resize(yv_size_new);
			RootFit::yv_der2.resize(yv_size_new);
			fread(&RootFit::yv_der[0], sizeof(vector<double>::value_type), yv_size_new, f_der);
			fread(&RootFit::yv_der2[0], sizeof(vector<double>::value_type), yv_size_new, f_der2);
			//RootFit::CalculateDer(1, 51); // посчитать производную по данным (число точек должно быть нечетным)
			
			//RootFit::CalculateStaircases_der(5); //time_dead in ns
			//RootFit::CalculateStaircases_amp(10); //time_dead in ns
			//exit(0);

			RootFit::threshold_der2 = -1E-5;
			RootFit::threshold_der = -2E-4;
			RootFit::threshold_amp = -0.001;
			RootFit::threshold_amp_start = -0.005;

			//RootFit::CalculateAverageSignal(200); //time_dead in ns

			RootFit::FindStartStop(5, 20); // найти начало и конец суммы сигналов
			RootFit::SetDispXY(0, 0.00113151);// записать вектора длины rec_lenght xverr и yverr значениеми ошибок
			RootFit::time_shit = 100; // задать смещение по времени для учета базовой линии (в точках)

			RootFit::previousIs1e = true;

			for (unsigned int i = 0; i < RootFit::time_finish.size(); i++)
			{
				long int temp = GetTickCount();
				cout << endl << "calculate fit ... " << i + 1 << " run time is (in s) \t" << (temp - before) / 1000.0 << " part: " << (i*100.0) / RootFit::time_finish.size() << " %" << endl;

				
				
				RootFit::current_signal = i;
				RootFit::CalculateStartParameters(5/*5*/);//вычислить стартовые параметры. Параметр - мертвое время производной в нс	
				RootFit::CalculateNumOfSignals(3);
				RootFit::CreateFrontGraph();

				RootFit *Fit_single = new RootFit(1);
				Fit_single->SetParameters();
				Fit_single->DoFit();
				Fit_single->SaveAllGraphs();


				if (Fit_single->GetChi2PerDof() > Monostate::chi2_per_dof_th)
				{
					cout << "\t double ... " << endl;

					RootFit *Fit_double = new RootFit(2);
					Fit_double->SetParameters();
					Fit_double->DoFit();
					Fit_double->SaveAllGraphs();


					if (Fit_double->GetChi2PerDof() > Monostate::chi2_per_dof_th)
					{
						cout << "\t \t triple ... " << endl;

						RootFit *Fit_triple = new RootFit(3);
						Fit_triple->SetParameters();
						Fit_triple->DoFit();
						Fit_triple->SaveAllGraphs();

						if (Fit_triple->GetChi2PerDof() > Monostate::chi2_per_dof_th)
						{
							cout << "\t \t \t quadruple ... " << endl;
							RootFit *Fit_quadruple = new RootFit(4);
							Fit_quadruple->SetParameters();
							Fit_quadruple->DoFit();
							Fit_quadruple->SaveAllGraphs();

							if (Fit_quadruple->GetChi2PerDof() > Monostate::chi2_per_dof_th)
							{
								cout << "\t \t \t \t quintuple ... " << endl;
								RootFit *Fit_quintuple = new RootFit(5);
								Fit_quintuple->SetParameters();
								Fit_quintuple->DoFit();
								Fit_quintuple->SaveAllGraphs();

							}

						}

					}


				}				


			}

			//RootFit::Print_dt_amp();


		}


	Monostate::SaveHlists();

	long int after = GetTickCount();
	cout << endl << "run time (in ms) \t " << after - before << endl;
	cout << endl << "run time (in s) \t " << (after - before) / 1000.0 << endl;
	cout << endl << "run time (in m) \t " << (after - before) / (1000.0 * 60) << endl;

	system("pause");
	return 0;
}