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
	t_0 = MPI_Wtime();
	//---------------------------------------------------

	RootFit::ReserveVectors();
	RootFit::SetAlgorithmParameters();

	const bool CalculateAvgSignal = false;
	RootFit::SaveGraphsBool = false;

	Double_t x, y;
	
	int counter = 0;
	int counter_rec_length = 0;

	TNtuple ntuple("ntuple", "fit results", "a_1:chi_1:a_2:b_2:chi_2:dt_2_ab");
	TTree tree("t1", "Parser tree");

	TMultiGraph *multi_graph_fit2 = new TMultiGraph();
	TGraphErrors *gr_tree_fit2 = new TGraphErrors();
	TGraph *gr_front_tree_fit2 = new TGraph();
	TGraph *gr_der_tree_fit2 = new TGraph();
	TGraph *gr_der2_tree_fit2 = new TGraph();

	TMultiGraph *multi_graph_fit1 = new TMultiGraph();
	TGraphErrors *gr_tree_fit1 = new TGraphErrors();
	TGraph *gr_front_tree_fit1 = new TGraph();
	TGraph *gr_der_tree_fit1 = new TGraph();
	TGraph *gr_der2_tree_fit1 = new TGraph();

	double a_1, chi_1, a_2, b_2, chi_2, dt_2_ab;

	tree.Branch("a_1", &a_1, "a_1/D");
	tree.Branch("chi_1", &chi_1, "chi_1/D");
	tree.Branch("a_2", &a_2, "a_2/D");
	tree.Branch("b_2", &b_2, "b_2/D");
	tree.Branch("chi_2", &chi_2, "chi_2/D");
	tree.Branch("dt_2_ab", &dt_2_ab, "dt_2_ab/D");

	
	multi_graph_fit2->Add(gr_tree_fit2);
	multi_graph_fit2->Add(gr_front_tree_fit2);
	multi_graph_fit2->Add(gr_der_tree_fit2);
	multi_graph_fit2->Add(gr_der2_tree_fit2);

	multi_graph_fit1->Add(gr_tree_fit1);
	multi_graph_fit1->Add(gr_front_tree_fit1);
	multi_graph_fit1->Add(gr_der_tree_fit1);
	multi_graph_fit1->Add(gr_der2_tree_fit1);

	//multi_graph->Add(new TGraph(*Fit_double->gr_front));
	//multi_graph->Add(new TGraph(*Fit_double->gr_der));
	//multi_graph->Add(new TGraph(*Fit_double->gr_der2));


	//tree.Branch("multi_graph", "TMultiGraph", &multi_graph);	
	tree.Branch("gr_fit2", "TMultiGraph", &multi_graph_fit2, 128000, 0);
	tree.Branch("gr_fit1", "TMultiGraph", &multi_graph_fit1, 128000, 0);

	string string_tree = Monostate::dir_name + "tree.root";
	TFile f_tree(string_tree.c_str(), "RECREATE");

	for (int file_i = 1; file_i <= 20; file_i++) // цикл по файлам
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
				cout << endl << "calculate fit ... " << i + 1 << " run time is (in s) " << MPI_Wtime() - t_0 << " part: " << (i*100.0) / RootFit::time_finish.size() << " % file є " << file_i << endl;



				RootFit::current_signal = i;
				RootFit::CalculateStartParameters(5/*5*/);//вычислить стартовые параметры. ѕараметр - мертвое врем€ производной в нс	
				RootFit::CalculateNumOfSignals(3);
				RootFit::CreateFrontGraph();

				cout << "\t single step 1 ... " << endl;
				RootFit *Fit_single = new RootFit(1);
				Fit_single->SetParametersTwoComp(0.2, 0.13, 0.28);
				Fit_single->DoFit();
				//Fit_single->SaveAllGraphs();

				if (Fit_single->GetChi2PerDof() > 4)
				{
					cout << "\t single step 2 ... " << endl;
					Fit_single->SetParametersTwoComp(0.2 + 0.2, 0.13 + 0.2, 0.28 + 0.2);
					Fit_single->DoFit();
				}

				if (Fit_single->GetChi2PerDof() > 4)
				{
					cout << "\t single step 3 ... " << endl;
					Fit_single->SetParametersTwoComp(0.2, 0.1, 10);
					Fit_single->DoFit();
				}

				if (true/*Fit_single->GetChi2PerDof() > 4 || Fit_single->fitFcn->GetParameter(0) > 0.065*/)
				{
					cout << "\t double ... " << endl;

					RootFit *Fit_double = new RootFit(2);
					Fit_double->SetParametersTwoComp(0.2, 0.001, 10);
					Fit_double->DoFit();
					//Fit_double->SaveAllGraphs();
										
					a_1 = Fit_single->fitFcn->GetParameter(0);
					chi_1 = Fit_single->GetChi2PerDof();
					a_2 = Fit_double->fitFcn->GetParameter(0);
					b_2 = Fit_double->fitFcn->GetParameter(8);
					chi_2 = Fit_double->GetChi2PerDof();
					dt_2_ab = fabs(Fit_double->fitFcn->GetParameter(9) - Fit_double->fitFcn->GetParameter(1));
					
					ntuple.Fill(a_1, chi_1, a_2, b_2, chi_2, dt_2_ab);
					
					*gr_tree_fit1 = *(Fit_single->gr);
					*gr_front_tree_fit1 = *(Fit_single->gr_front);
					*gr_der_tree_fit1 = *(Fit_single->gr_der);
					*gr_der2_tree_fit1 = *(Fit_single->gr_der2);

					*gr_tree_fit2 = *(Fit_double->gr);
					*gr_front_tree_fit2 = *(Fit_double->gr_front);
					*gr_der_tree_fit2 = *(Fit_double->gr_der);
					*gr_der2_tree_fit2 = *(Fit_double->gr_der2);

					tree.Fill();
					if ((i + 1) % 100 == 1) tree.AutoSave("SaveSelf");
					
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


	tree.Write();
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

	system("pause");

	// Finalize MPI
	//MPI_Finalize();

	return 0;
}