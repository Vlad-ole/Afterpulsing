//base
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <Windows.h>
#include <mpi.h>
#include <sstream>
#include <algorithm>

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
	RootFit::SetAlgorithmParameters();

	cout << "number of arguments = " << argc << endl;
	if (argc == 1)
	{
		//dir_name = "D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\275K\\68_60V\\";
		cout << "Please, input dir_name " << endl;
		cout << argv[0] << endl;
		system("pause");
		return 0;
	}
	else if (argc == 6)
	{
		
		cout << argv[0] << endl;
		cout << argv[1] << endl;
		cout << argv[2] << endl;
		cout << argv[3] << endl;
		cout << argv[4] << endl;
		cout << argv[5] << endl;

		Monostate::dir_name = argv[1];
		RootFit::threshold_amp_start = -atof(argv[2]);
		RootFit::threshold_der = -atof(argv[3]);


		
	}
	else
	{
		cout << "too many or too few arguments! " << endl;
		return 0;
	}

	cout << "dir_name = " <<Monostate::dir_name << endl;
	cout << "threshold_amp_start = " << RootFit::threshold_amp_start << endl;
	cout << "threshold_der = " << RootFit::threshold_der << endl;

	int file_i_start = atoi(argv[4]);
	int file_i_stop = atoi(argv[5]);


	// Initialise MPI
	//-----------------------------------------------
	MPI_Init(&argc, &argv);
	int n, myid, numprocs;
	double t_0, t2;
	t_0 = MPI_Wtime();
	//---------------------------------------------------

	RootFit::ReserveVectors();
	

	const bool CalculateAvgSignal = false;
	RootFit::SaveGraphsBool = false;

	Double_t x, y;
	
	int counter = 0;
	int counter_rec_length = 0;

	TNtuple ntuple("ntuple", "fit results", "a_1:chi_1:a_2:b_2:chi_2:dt_2_ab");


	//string string_tree = Monostate::dir_name + "tree.root";
	

	int global_counter = 0;

	for (int file_i = file_i_start; file_i <= file_i_stop; file_i++) // цикл по файлам
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

		RootFit::ReadFiles(RootFit::ReadDerivative, file_i, 1);
		
		ostringstream file_tree_oss;
		file_tree_oss << Monostate::dir_name << "trees\\run_" << file_i << ".root";
		TFile f_tree(file_tree_oss.str().c_str(), "RECREATE");



		//------------------------------------------------------------
		//tree
		TTree tree("t1", "Parser tree fit_1 & fit_2");

		TMultiGraph *multi_graph_fit1 = new TMultiGraph();
		TGraphErrors *gr_tree_fit1 = new TGraphErrors();
		TGraph *gr_front_tree_fit1 = new TGraph();
		TGraph *gr_der_tree_fit1 = new TGraph();
		TGraph *gr_der2_tree_fit1 = new TGraph();

		TMultiGraph *multi_graph_fit2 = new TMultiGraph();
		TGraphErrors *gr_tree_fit2 = new TGraphErrors();
		TGraph *gr_front_tree_fit2 = new TGraph();
		TGraph *gr_der_tree_fit2 = new TGraph();
		TGraph *gr_der2_tree_fit2 = new TGraph();

		TMultiGraph *multi_graph_fit3 = new TMultiGraph();
		TGraphErrors *gr_tree_fit3 = new TGraphErrors();
		TGraph *gr_front_tree_fit3 = new TGraph();
		TGraph *gr_der_tree_fit3 = new TGraph();
		TGraph *gr_der2_tree_fit3 = new TGraph();

		double a_1, chi_1, time_1; // fit_1
		double a_2, b_2, chi_2, dt_2_ab, time_2_a, time_2_b; // fit_2
		double a_3, b_3, c_3, chi_3, dt_3_ab, dt_3_bc, time_3_a, time_3_b, time_3_c; // fit_3

		// fit_1
		tree.Branch("a_1", &a_1, "a_1/D");
		tree.Branch("chi_1", &chi_1, "chi_1/D");
		tree.Branch("time_1", &time_1, "time_1/D");

		// fit_2
		tree.Branch("a_2", &a_2, "a_2/D");
		tree.Branch("b_2", &b_2, "b_2/D");
		tree.Branch("chi_2", &chi_2, "chi_2/D");
		tree.Branch("dt_2_ab", &dt_2_ab, "dt_2_ab/D");
		tree.Branch("time_2_a", &time_2_a, "time_2_a/D");
		tree.Branch("time_2_b", &time_2_b, "time_2_b/D");

		// fit_3
		tree.Branch("a_3", &a_3, "a_3/D");
		tree.Branch("b_3", &b_3, "b_3/D");
		tree.Branch("c_3", &b_3, "c_3/D");
		tree.Branch("chi_3", &chi_3, "chi_3/D");
		tree.Branch("dt_3_ab", &dt_3_ab, "dt_3_ab/D");
		tree.Branch("dt_3_bc", &dt_3_bc, "dt_3_bc/D");

		tree.Branch("time_3_a", &time_3_a, "time_3_a/D");
		tree.Branch("time_3_b", &time_3_b, "time_3_b/D");
		tree.Branch("time_3_c", &time_3_c, "time_3_c/D");


		multi_graph_fit1->Add(gr_tree_fit1);
		multi_graph_fit1->Add(gr_front_tree_fit1);
		multi_graph_fit1->Add(gr_der_tree_fit1);
		multi_graph_fit1->Add(gr_der2_tree_fit1);

		multi_graph_fit2->Add(gr_tree_fit2);
		multi_graph_fit2->Add(gr_front_tree_fit2);
		multi_graph_fit2->Add(gr_der_tree_fit2);
		multi_graph_fit2->Add(gr_der2_tree_fit2);

		multi_graph_fit3->Add(gr_tree_fit3);
		multi_graph_fit3->Add(gr_front_tree_fit3);
		multi_graph_fit3->Add(gr_der_tree_fit3);
		multi_graph_fit3->Add(gr_der2_tree_fit3);

		tree.Branch("gr_fit1", "TMultiGraph", &multi_graph_fit1, 128000, 0);
		tree.Branch("gr_fit2", "TMultiGraph", &multi_graph_fit2, 128000, 0);
		tree.Branch("gr_fit3", "TMultiGraph", &multi_graph_fit3, 128000, 0);
		//------------------------------------------------------------


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
				global_counter++;
				cout << endl << "calculate fit ... " << i + 1 << " run time is (in s) " << MPI_Wtime() - t_0 << " part: " << (i*100.0) / RootFit::time_finish.size() << " % file є " << file_i << endl;
				cout << "speed of fit = " << (MPI_Wtime() - t_0) / global_counter << endl << endl;

				bool rigid_boundaries = true;

				RootFit::current_signal = i;
				RootFit::CalculateStartParameters(5/*5*/);//вычислить стартовые параметры. ѕараметр - мертвое врем€ производной в нс	
				RootFit::CalculateNumOfSignals(3);
				RootFit::CreateFrontGraph();

				cout << "\t single step 1 ... " << endl;
				RootFit *Fit_single = new RootFit(1);
				
				double A_start[] = { 0.2 };
				double A_limit_l[] = { 0.01 };
				double A_limit_h[] = { 10 };

				if (rigid_boundaries)
				{
					A_start[0] = { 0.2 };
					A_limit_l[0] = { 0.1 };
					A_limit_h[0] = { 0.3 };
				}

				Fit_single->SetParametersTwoComp_fit1(A_start, A_limit_l, A_limit_h);
				Fit_single->DoFit();
				//Fit_single->SaveAllGraphs();

				if (rigid_boundaries)
				{
					if (Fit_single->GetChi2PerDof() > 4)
					{
						cout << "\t single step 2 ... " << endl;
						A_start[0] = { 0.4 };
						A_limit_l[0] = { 0.3 };
						A_limit_h[0] = { 0.5 };
						Fit_single->SetParametersTwoComp_fit1(A_start, A_limit_l, A_limit_h);
						Fit_single->DoFit();
					}

					//if (Fit_single->GetChi2PerDof() > 4)
					//{
					//	cout << "\t single step 3 ... " << endl;
					//	A_start[0] = { 0.5 };
					//	A_limit_l[0] = { 0.43 };
					//	A_limit_h[0] = { 0.55 };
					//	Fit_single->SetParametersTwoComp_fit1(A_start, A_limit_l, A_limit_h);
					//	Fit_single->DoFit();
					//}

					if (Fit_single->GetChi2PerDof() > 4)
					{
						cout << "\t single step 4 ... " << endl;
						A_start[0] = { 0.6 };
						A_limit_l[0] = { 0.5 };
						A_limit_h[0] = { 0.7 };
						Fit_single->SetParametersTwoComp_fit1(A_start, A_limit_l, A_limit_h);
						Fit_single->DoFit();
					}
				}



				a_1 = Fit_single->fitFcn->GetParameter(0);
				chi_1 = Fit_single->GetChi2PerDof();
				time_1 = Fit_single->fitFcn->GetParameter(1);

				a_2 = -100;
				b_2 = -100;
				chi_2 = -100;
				dt_2_ab = -100;
				time_2_a = -100;
				time_2_b = -100;

				a_3 = -100;
				b_3 = -100;
				c_3 = -100;
				chi_3 = -100;
				dt_3_ab = -100;
				dt_3_bc = -100;
				time_3_a = -100;
				time_3_b = -100;
				time_3_c = -100;

				*gr_tree_fit1 = *(Fit_single->gr);
				*gr_front_tree_fit1 = *(Fit_single->gr_front);
				*gr_der_tree_fit1 = *(Fit_single->gr_der);
				*gr_der2_tree_fit1 = *(Fit_single->gr_der2);


				if (/*Fit_single->GetChi2PerDof() > 3  || Fit_single->fitFcn->GetParameter(0) > 0.065*/ true)
				{
					rigid_boundaries = false;
					
					cout << "\t double step 1  ... " << endl;

					RootFit *Fit_double = new RootFit(2);

					double A_start[] = { 0.2, 0.2 };
					double A_limit_l[] = { 0.001, 0.001 };
					double A_limit_h[] = { 10, 10 };
					
					if (rigid_boundaries)
					{
						double A_start[] = { 0.25, 0.25 };
						double A_limit_l[] = { 0.2, 0.03 };
						double A_limit_h[] = { 0.33, 0.35 };
					}


					Fit_double->SetParametersTwoComp_fit2(A_start, A_limit_l, A_limit_h);
					Fit_double->DoFit();
					//Fit_double->SaveAllGraphs();

					if (rigid_boundaries)
					{					
						if (Fit_double->GetChi2PerDof() > 4)
						{
							cout << "\t double step 2  ... " << endl;
							double A_start[] = { 0.2, 0.2 };
							double A_limit_l[] = { 0.001, 0.001 };
							double A_limit_h[] = { 10, 10 };
							Fit_double->SetParametersTwoComp_fit2(A_start, A_limit_l, A_limit_h);
							Fit_double->DoFit();
						}
					}	

					//if (fabs(Fit_double->fitFcn->GetParameter(9) - Fit_double->fitFcn->GetParameter(1)) < 1)
					//{
					//	cout << "\t double step 2 ... " << endl;
					//	A_start[0] = { 0.2 };
					//	A_limit_l[0] = { 0.1 };
					//	A_limit_h[0] = { 10 };
					//	Fit_double->SetParametersTwoComp_fit2(A_start, A_limit_l, A_limit_h);
					//	Fit_double->DoFit();
					//}
					ntuple.Fill(a_1, chi_1, a_2, b_2, chi_2, dt_2_ab);	

					a_2 = Fit_double->fitFcn->GetParameter(0);
					b_2 = Fit_double->fitFcn->GetParameter(8);
					chi_2 = Fit_double->GetChi2PerDof();
					dt_2_ab = fabs(Fit_double->fitFcn->GetParameter(9) - Fit_double->fitFcn->GetParameter(1));
					time_2_a = min(Fit_double->fitFcn->GetParameter(1), Fit_double->fitFcn->GetParameter(9));
					time_2_b = max(Fit_double->fitFcn->GetParameter(1), Fit_double->fitFcn->GetParameter(9));

					*gr_tree_fit2 = *(Fit_double->gr);
					*gr_front_tree_fit2 = *(Fit_double->gr_front);
					*gr_der_tree_fit2 = *(Fit_double->gr_der);
					*gr_der2_tree_fit2 = *(Fit_double->gr_der2);

					////fill tree
					//////////////////////////////////////////////////
					//a_1 = Fit_single->fitFcn->GetParameter(0);
					//chi_1 = Fit_single->GetChi2PerDof();
					//a_2 = Fit_double->fitFcn->GetParameter(0);
					//b_2 = Fit_double->fitFcn->GetParameter(8);
					//chi_2 = Fit_double->GetChi2PerDof();
					//dt_2_ab = fabs(Fit_double->fitFcn->GetParameter(9) - Fit_double->fitFcn->GetParameter(1));
					//
					//
					//
					//*gr_tree_fit1 = *(Fit_single->gr);
					//*gr_front_tree_fit1 = *(Fit_single->gr_front);
					//*gr_der_tree_fit1 = *(Fit_single->gr_der);
					//*gr_der2_tree_fit1 = *(Fit_single->gr_der2);

					//*gr_tree_fit2 = *(Fit_double->gr);
					//*gr_front_tree_fit2 = *(Fit_double->gr_front);
					//*gr_der_tree_fit2 = *(Fit_double->gr_der);
					//*gr_der2_tree_fit2 = *(Fit_double->gr_der2);

					//tree.Fill();
					//if ((i + 1) % 100 == 1) tree.AutoSave("SaveSelf");
					////fill tree end
					//////////////////////////////////////////////////
					
					if (/*Fit_double->GetChi2PerDof() > 3 && Fit_single->GetChi2PerDof() > 3 */ false)
					{
						cout << "\t \t triple ... " << endl;

						RootFit *Fit_triple = new RootFit(3);
						double A_start[] = { 0.2, 0.2, 0.2 };
						double A_limit_l[] = { 0.001, 0.001, 0.001};
						double A_limit_h[] = { 10, 10, 10 };
						Fit_triple->SetParametersTwoComp_fit3(A_start, A_limit_l, A_limit_h);
						Fit_triple->DoFit();
						Fit_triple->SaveAllGraphs();

						//fill tree
						a_3 = Fit_triple->fitFcn->GetParameter(0);
						b_3 = Fit_triple->fitFcn->GetParameter(8);
						c_3 = Fit_triple->fitFcn->GetParameter(15);
						chi_3 = Fit_triple->GetChi2PerDof();
						vector<double> time_abc = { Fit_double->fitFcn->GetParameter(1), Fit_double->fitFcn->GetParameter(9), Fit_double->fitFcn->GetParameter(16) };
						sort(time_abc.begin(), time_abc.end());
						dt_3_ab = time_abc[1] - time_abc[0];
						dt_3_bc = time_abc[2] - time_abc[1];
						time_3_a = time_abc[0];
						time_3_b = time_abc[1];
						time_3_c = time_abc[2];

						*gr_tree_fit3 = *(Fit_triple->gr);
						*gr_front_tree_fit3 = *(Fit_triple->gr_front);
						*gr_der_tree_fit3 = *(Fit_triple->gr_der);
						*gr_der2_tree_fit3 = *(Fit_triple->gr_der2);




						if (Fit_triple->GetChi2PerDof() > Monostate::chi2_per_dof_th && false)
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
						

						delete Fit_triple;

					}

					delete Fit_double;

				}

				delete Fit_single;
				
				tree.Fill();
				//if ((i + 1) % 100 == 1) tree.AutoSave("SaveSelf");

			}



		}

		//tree.Fill();
		tree.Write();
		//f_tree.Close();

		delete multi_graph_fit1;
		delete gr_tree_fit1;
		delete gr_front_tree_fit1;
		delete gr_der_tree_fit1;
		delete gr_der2_tree_fit1;

		delete multi_graph_fit2;
		delete gr_tree_fit2;
		delete gr_front_tree_fit2;
		delete gr_der_tree_fit2;
		delete gr_der2_tree_fit2;

		delete multi_graph_fit3;
		delete gr_tree_fit3;
		delete gr_front_tree_fit3;
		delete gr_der_tree_fit3;
		delete gr_der2_tree_fit3;

	}//end for files
		
	//f_tree.Close();

	if (RootFit::ReadDerivative)
	{
		string string_ntuple = Monostate::dir_name + "ntuple.root";
		TFile f_ntuple(string_ntuple.c_str(), "RECREATE");
		ntuple.Write();
		f_ntuple.Close();
	}


	//Monostate::SaveHlists();

	cout << "Total execution time is (in s): " << MPI_Wtime() - t_0 << endl;

	//system("pause");

	// Finalize MPI
	//MPI_Finalize();

	return 0;
}