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
#include <mpi.h>

using namespace std;


int RootFit::time_start_index;
int RootFit::time_finish_index;

int RootFit::time_shit;

double RootFit::threshold_1e_low;
double RootFit::threshold_1e_high;

double RootFit::threshold_1e_A_low;
double RootFit::threshold_1e_A_high;

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

bool RootFit::only_1e;
bool RootFit::RecoveryTimeTwoComponents;

//> "D:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\fit_log.txt"

int main(int argc, char *argv[])
{
	// Initialise MPI
	//-----------------------------------------------
	MPI_Init(&argc, &argv);
	int n, myid, numprocs, i;
	double t_0, t2;

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//-----------------------------------------------



	//serial region 1 start
	//-----------------------------------------------
	if (myid == 0)
	{
		t_0 = MPI_Wtime();

		const bool ReadDerivative = true;

		ofstream file_test(Monostate::dir_name + "test.dat");

		Double_t x, y;
		FILE *f = fopen(Monostate::raw_name.c_str(), "rb");
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
		//----------------------------------------------------
		int yv_size;
		fread(&yv_size, sizeof(int), 1, f);
		cout << "file length = " << yv_size << endl;

		int yv_size_new = yv_size * 0.92; // выбрать долю от всех данных. ѕам€ти хватит только на 0.92 максимум
		cout << "file length cutted= " << yv_size_new << endl;

		RootFit::yv.resize(yv_size_new);
		fread(&RootFit::yv[0], sizeof(vector<double>::value_type), yv_size_new, f);

		RootFit::xv.resize(yv_size_new);
		for (int i = 0; i < yv_size_new; i++)
		{
			RootFit::xv[i] = 0.2 * i;
		}

		if (ReadDerivative)
		{
			//прочитать первую и вторую производную из файла
			RootFit::yv_der.resize(yv_size_new);
			RootFit::yv_der2.resize(yv_size_new);
			fread(&RootFit::yv_der[0], sizeof(vector<double>::value_type), yv_size_new, f_der);
			fread(&RootFit::yv_der2[0], sizeof(vector<double>::value_type), yv_size_new, f_der2);
		}
		//----------------------------------------------------

		cout << endl << "Time of file reading is (in s) " << MPI_Wtime() - t_0 << endl;

		if (!ReadDerivative)
			RootFit::CalculateDer(1, 51); // посчитать производную по данным (число точек должно быть нечетным)

		//RootFit::CalculateStaircases_der(5); //time_dead in ns
		//RootFit::CalculateStaircases_amp(10); //time_dead in ns
		
		RootFit::threshold_der2 = -1E-5;
		RootFit::threshold_der =
			/*-2E-4 (hamamatsu 33 4.5 OV)*/
			/*-0.06  MPPC_S10362-11-100C */
			-2E-4;

		RootFit::RecoveryTimeTwoComponents = true;
		RootFit::only_1e = true; // рассматривать импульсы с послеимпульсами только с одной €чейки
		RootFit::previousIs1e = true; // вспомогательное условие

		RootFit::threshold_amp = -0.001; // амплитудный порог, наход€щийс€ на уровне шума
		RootFit::threshold_amp_start = -0.005; // амплитудный порог дл€ запуска 

		RootFit::threshold_1e_low = 0.008; // пороги по амплитуде (в ¬ольтах) дл€ правильного нахождени€ средней формы сигнала
		RootFit::threshold_1e_high = 0.016;
		//RootFit::CalculateAverageSignal(200); //time_dead in ns

		RootFit::threshold_1e_A_low = 0.02;
		RootFit::threshold_1e_A_high = 0.07;
		
		RootFit::FindStartStop(5, 20); // найти начало и конец суммы сигналов
		RootFit::SetDispXY(0, 0.00113151);// записать вектора xverr и yverr значениеми ошибок
		RootFit::time_shit = 100; // задать смещение по времени дл€ учета базовой линии (в точках)		

	}
	//serial region 1 end
	//-----------------------------------------------




	//parallel region start
	//-----------------------------------------------
	//for (i = myid + 1; i <= 50000; i += numprocs)
	for (unsigned int i = 0; i < RootFit::time_finish.size(); i++)
	{
		cout << endl << "calculate fit ... " << i + 1 << " run time is (in s) \t" << MPI_Wtime() - t_0 << " part: " << (i*100.0) / RootFit::time_finish.size() << " %" << endl;



		RootFit::current_signal = i;
		RootFit::CalculateStartParameters(5/*5*/);//вычислить стартовые параметры. ѕараметр - мертвое врем€ производной в нс	
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
	//parallel region end
	//-----------------------------------------------



	//serial region 2 start
	//-----------------------------------------------
	if (myid == 0)
	{
		Monostate::SaveHlists();

		cout << "Total execution time is (in s): " << MPI_Wtime() - t_0 << endl;

		system("pause");
	}
	//serial region 2 end
	//-----------------------------------------------



	// Finalize MPI
	MPI_Finalize();
	
	return 0;
}