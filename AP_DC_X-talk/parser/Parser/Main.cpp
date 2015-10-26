//base
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <Windows.h>
#include <mpi.h>
#include <sstream>

//root cern
#include "TRandom.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TFile.h"
//#include  "gStyle.h"
//#include "Math.h"

//my
#include "RootFit.h"
#include "Monostate.h"




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
bool RootFit::SaveGraphsBool;
bool RootFit::ReadDerivative;

//> "D:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\fit_log.txt"

int main(int argc, char *argv[])
{
	
	// Initialise MPI
	//-----------------------------------------------
	MPI_Init(&argc, &argv);
	int n, myid, numprocs;
	double t_0, t2;

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//-----------------------------------------------
	t_0 = MPI_Wtime();

	RootFit::xv.reserve(20*1000*1000);
	RootFit::yv.reserve(20 * 1000 * 1000);
	RootFit::yv_der.reserve(20 * 1000 * 1000);
	RootFit::yv_der2.reserve(20 * 1000 * 1000);
	RootFit::xverr.reserve(20 * 1000 * 1000);
	RootFit::yverr.reserve(20 * 1000 * 1000);

	// задать параметры алгоритма
	RootFit::threshold_der2 = -1E-5;
	RootFit::threshold_der =
		/*-2E-4 (hamamatsu 33 4.5 OV)*/
		/*-0.06  MPPC_S10362-11-100C */
		-4E-4;

	RootFit::RecoveryTimeTwoComponents = true;
	RootFit::only_1e = true; // рассматривать импульсы с послеимпульсами только с одной ячейки
	RootFit::ReadDerivative = true;

	RootFit::threshold_amp = -0.002; // амплитудный порог, находящийся на уровне шума
	RootFit::threshold_amp_start = -0.01; // амплитудный порог для запуска 

	RootFit::threshold_1e_low = 0.03; // пороги по амплитуде (в Вольтах) для правильного нахождения средней формы сигнала
	RootFit::threshold_1e_high = 0.05;

	RootFit::threshold_1e_A_low = 0.02;
	RootFit::threshold_1e_A_high = 0.07;
	RootFit::time_shit = 100; // задать смещение по времени для учета базовой линии (в точках)	
	//-------------------------------------------------------
		

	const bool CalculateAvgSignal = false;
	RootFit::SaveGraphsBool = false;

	ofstream file_test(Monostate::dir_name + "test.dat");

	Double_t x, y;
	
	int counter = 0;
	int counter_rec_length = 0;

	TNtuple ntuple("ntuple", "fit results", "a_1:chi_1:a_2:b_2:chi_2:dt_2_ab");
	
	for (int file_i = 11; file_i <= 20; file_i++) // цикл по файлам
	{

		RootFit::time_start.clear();
		RootFit::time_finish.clear();
		RootFit::time_front.clear();

		RootFit::xv_front.clear();
		RootFit::yv_front.clear();

		if (file_i == 1)
			RootFit::previousIs1e = true; // вспомогательное условие
		else
			RootFit::previousIs1e = false;

		RootFit::ReadFiles(RootFit::ReadDerivative, file_i);
		

		if (CalculateAvgSignal)
		{
			
			RootFit::CalculateAverageSignal(200); //time_dead in ns
		}

		//RootFit::CalculateStaircases_der(5); //time_dead in ns
		//RootFit::CalculateStaircases_amp(10); //time_dead in ns

		
		if (!CalculateAvgSignal && RootFit::ReadDerivative)
		{
			RootFit::FindStartStop(5, 20); // найти начало и конец суммы сигналов
			RootFit::SetDispXY(0, /*0.001131518*/ 0.00186664);// записать вектора xverr и yverr значениеми ошибок


			//fit signal
			for (unsigned int i = 0; i < RootFit::time_finish.size(); i++)
			{
				cout << endl << "calculate fit ... " << i + 1 << " run time is (in s) " << MPI_Wtime() - t_0 << " part: " << (i*100.0) / RootFit::time_finish.size() << " % file № " << file_i << endl;



				RootFit::current_signal = i;
				RootFit::CalculateStartParameters(5/*5*/);//вычислить стартовые параметры. Параметр - мертвое время производной в нс	
				RootFit::CalculateNumOfSignals(3);
				RootFit::CreateFrontGraph();

				RootFit *Fit_single = new RootFit(1);
				Fit_single->SetParameters();
				Fit_single->DoFit();
				Fit_single->SaveAllGraphs();


				if (true/*Fit_single->GetChi2PerDof() > 4 || Fit_single->fitFcn->GetParameter(0) > 0.065*/)
				{
					cout << "\t double ... " << endl;

					RootFit *Fit_double = new RootFit(2);
					Fit_double->SetParameters();
					Fit_double->DoFit();
					Fit_double->SaveAllGraphs();

					ntuple.Fill(Fit_single->fitFcn->GetParameter(0), Fit_single->GetChi2PerDof(), Fit_double->fitFcn->GetParameter(0), Fit_double->fitFcn->GetParameter(8), Fit_double->GetChi2PerDof(), fabs(Fit_double->fitFcn->GetParameter(9) - Fit_double->fitFcn->GetParameter(1)) );

					if (Fit_double->GetChi2PerDof() > Monostate::chi2_per_dof_th && false)
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

								//delete Fit_quintuple;

							}

							//delete Fit_quadruple;

						}

						//delete Fit_triple;

					}

					delete Fit_double;

				}

				delete Fit_single;
				

			}
		}


	}


	if (RootFit::ReadDerivative)
	{
		string string_ntuple = Monostate::dir_name + "ntuple.root";
		TFile f_ntuple(string_ntuple.c_str(), "RECREATE");
		ntuple.Write();
		f_ntuple.Close();
	}


	//Monostate::SaveHlists();

	cout << "Total execution time is (in s): " << MPI_Wtime() - t_0 << endl;

	system("pause");

	// Finalize MPI
	MPI_Finalize();

	return 0;
}