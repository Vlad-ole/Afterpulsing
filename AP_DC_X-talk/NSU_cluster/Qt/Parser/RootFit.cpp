//Root cern
#include "RootFit.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TMath.h"
#include "TFile.h"
#include "Math/MinimizerOptions.h"

//base
//#include <mpi.h>
#include <sstream>
//#include <chrono>
#include <iomanip>
//#include <Windows.h>

//my
#include "Monostate.h"

using namespace std;

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

bool RootFit::previousIs1e;
double RootFit::previous_time;

//bool RootFit::PreviousIsSingle;
//double RootFit::temp_time_i;

RootFit::RootFit()
{
	gr = new TGraphErrors();
}

RootFit::RootFit(short int number_of_functions)
{
	this->number_of_functions = number_of_functions;

	gr = new TGraphErrors(time_finish_index - time_start_index, &xv[time_start_index], &yv[time_start_index], &xverr[time_start_index], &yverr[time_start_index]);
	gr->SetMarkerColor(4);
	gr->SetMarkerStyle(kFullCircle);

	gr_der = new TGraph(time_finish_index - time_start_index, &xv[time_start_index], &yv_der[time_start_index]);
	//gr_der2 = new TGraph(time_finish_index - time_start_index, &xv[time_start_index], &yv_der2[time_start_index]);

     for (int i = 0; i < gr_der->GetN(); i++) gr_der->GetY()[i] *= 50;
	//for (int i = 0; i < gr_der2->GetN(); i++) gr_der2->GetY()[i] *= 500;

	//gr_der2->SetLineColor(7);

	gr_front = new TGraph(time_finish_index - time_start_index, &xv_front[0], &yv_front[0]);
	gr_front->SetMarkerColor(6);
	gr_front->SetMarkerStyle(kFullCircle);
	gr_front->SetMarkerSize(1);

	
	//if (!RecoveryTimeTwoComponents)
	//{
	//	if (number_of_functions == 1)
	//		fitFcn = new TF1("fitFcn", fitFunction, xv[time_start_index], xv[time_finish_index], 6);
	//	if (number_of_functions == 2)
	//		fitFcn = new TF1("fitFcn", fitFunction_2, xv[time_start_index], xv[time_finish_index], 6 + 5);
	//	if (number_of_functions == 3)
	//		fitFcn = new TF1("fitFcn", fitFunction_3, xv[time_start_index], xv[time_finish_index], 6 + 10);
	//	if (number_of_functions == 4)
	//		fitFcn = new TF1("fitFcn", fitFunction_4, xv[time_start_index], xv[time_finish_index], 6 + 15);
	//	if (number_of_functions == 5)
	//		fitFcn = new TF1("fitFcn", fitFunction_5, xv[time_start_index], xv[time_finish_index], 6 + 20);
	//}
	//else
	//{
	//	if (number_of_functions == 1)
	//		fitFcn = new TF1("fitFcn", fitFunctionTwoComp, xv[time_start_index], xv[time_finish_index], 8 + 7*0);
	//	if (number_of_functions == 2)
	//		fitFcn = new TF1("fitFcn", fitFunction_2_TwoComp, xv[time_start_index], xv[time_finish_index], 8 + 7*1);
	//	if (number_of_functions == 3)
	//		fitFcn = new TF1("fitFcn", fitFunction_3_TwoComp, xv[time_start_index], xv[time_finish_index], 8 + 7*2);
	//	if (number_of_functions == 4)
	//		fitFcn = new TF1("fitFcn", fitFunction_4, xv[time_start_index], xv[time_finish_index], 8 + 7*4);
	//	if (number_of_functions == 5)
	//		fitFcn = new TF1("fitFcn", fitFunction_5, xv[time_start_index], xv[time_finish_index], 8 + 7*5);
	//}


	if (number_of_functions == 1)
		fitFcn = new TF1("fitFcn", fitFunctionOneComp, xv[time_start_index], xv[time_finish_index], 4 + 1);
	if (number_of_functions == 2)
		fitFcn = new TF1("fitFcn", fitFunction_2_OneComp, xv[time_start_index], xv[time_finish_index], 4*2 + 1);



	//ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit", "Simplex");
	//ROOT::Math::MinimizerOptions::SetDefaultPrecision(17);
	ROOT::Math::MinimizerOptions::SetDefaultStrategy(2);
}


RootFit::~RootFit()
{
	delete fitFcn;
        delete gr_front;
	//delete gr_der2;
        delete gr_der;
	delete gr;
}

void RootFit::ReadFiles(const bool ReadDerivative, const int file_run, const double part_or_file)
{
	//double t_01 = MPI_Wtime();
	clock_t t_01 = clock();
	//читать файл
	//----------------------------------------------------
	ostringstream f_oss;
	//f_oss << Monostate::dir_name << "raw\\binary\\run_" << file_run << ".bin"; //my PC
     f_oss << Monostate::dir_name << "raw/binary/run_" << file_run << ".bin"; //NSU Claster

	FILE *f = fopen(f_oss.str().c_str(), "rb");

	if (f == NULL)
	{
		cout << "Can't open this file: " << f_oss.str().c_str() << endl;
		exit(1);
		//system("pause");
	}

	int yv_size = 5000 * 1000;
	//fread(&yv_size, sizeof(int), 1, f);
	cout << "file length = " << yv_size << endl;

	int yv_size_new = yv_size * part_or_file; // выбрать долю от всех данных. Памяти хватит только на 0.92 максимум
	cout << "file length cutted= " << yv_size_new << endl;

	RootFit::yv.resize(yv_size_new);
	fread(&RootFit::yv[0], sizeof(vector<double>::value_type), yv_size_new, f);

	RootFit::xv.resize(yv_size_new);
	for (int i = 0; i < yv_size_new; i++)
	{
		RootFit::xv[i] = 0.2 * i;
	}

	ostringstream f_der_oss;
	f_der_oss << Monostate::dir_name << "der/run_" << file_run << ".bin";

	ostringstream f_der2_oss;
	f_der2_oss << Monostate::dir_name << "der2/run_" << file_run << ".bin";

	Monostate::der_name = f_der_oss.str();
	Monostate::der2_name = f_der2_oss.str();

	if (ReadDerivative)
	{
		//прочитать первую и вторую производную из файла
		FILE *f_der = fopen(Monostate::der_name.c_str(), "rb");
		//FILE *f_der2 = fopen(Monostate::der2_name.c_str(), "rb");

		if (f_der == NULL /*|| f_der2 == NULL*/)
		{
			cout << "Can't open this file: " << Monostate::der_name.c_str() << endl;
			//cout << "Can't open this file: " << Monostate::der2_name.c_str() << endl;
			//system("pause");
		}

		RootFit::yv_der.resize(yv_size_new);
		//RootFit::yv_der2.resize(yv_size_new);
		fread(&RootFit::yv_der[0], sizeof(vector<double>::value_type), yv_size_new, f_der);
		//fread(&RootFit::yv_der2[0], sizeof(vector<double>::value_type), yv_size_new, f_der2);
	}
	//----------------------------------------------------
	cout << endl << "Time of file reading is (in s) " << (double)(clock() - t_01) / CLOCKS_PER_SEC << endl;

	if (!ReadDerivative)
		RootFit::CalculateDer(1, 51); // посчитать производную по данным (число точек должно быть нечетным)
}


void RootFit::SetParameters()
{
	if (!RecoveryTimeTwoComponents)
	{
		const double A_start = 0.04;
		const double A_limit_l = 0.001;
		const double A_limit_h = 1;

		const double baseline_limit = 0.006;
		const double sigma = 1.64932;

		const double time_first = xv[time_front[0]];

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
	}
	else
	{
		//SetParametersTwoComp();
	}

	//fitFcn->Modify();
	//fitFcn->ReleaseParameter();
	//fitFcn->Update();

}


void RootFit::SetParametersTwoComp_fit1(const double * const A_start, const double * const A_limit_l, const double * const A_limit_h)
{
	const double tau_rec_fast = 2.21513;
	const double tau_rise = 5.47845;

	const double baseline_limit = 0.006;
	const double sigma = 1.78391;

	const double tau_rec_slow = 43.0645;
	const double R_slow = 0.254653;

	const double time_first = xv[time_front[0]];
	//const double time_first = xv[time_start[current_signal]];


		// A
		fitFcn->SetParameter(0, A_start[0]);
		fitFcn->SetParLimits(0, A_limit_l[0], A_limit_h[0]);

		//t_0
          fitFcn->SetParameter(1, time_first);
          fitFcn->SetParLimits(1, xv[time_start_index], xv[time_finish_index]); //simple variant
          //fitFcn->SetParLimits(1, xv[time_start_index] + 5, time_first); // more complex variant

          //fitFcn->SetParameter(1, time_first + 5);
          //fitFcn->SetParLimits(1, xv[time_start_index] + 5, time_first+10); // slava variant

		// tau_rec
		fitFcn->SetParameter(2, tau_rec_fast);
		fitFcn->SetParLimits(2, tau_rec_fast, tau_rec_fast);

		// tau_rise
		fitFcn->SetParameter(3, tau_rise);
		fitFcn->SetParLimits(3, tau_rise, tau_rise);

		//sigma
		fitFcn->SetParameter(4, sigma);
		fitFcn->SetParLimits(4, sigma, sigma);

		//tau_rec_slow
		fitFcn->SetParameter(5, tau_rec_slow);
		fitFcn->SetParLimits(5, tau_rec_slow, tau_rec_slow);

		//R_slow
		fitFcn->SetParameter(6, R_slow);
		fitFcn->SetParLimits(6, R_slow, R_slow);

		//baseline
		fitFcn->SetParameter(7, 0);
		fitFcn->SetParLimits(7, -baseline_limit, baseline_limit);

}


void RootFit::SetParametersOneComp_fit1(const double * const A_start, const double * const A_limit_l, const double * const A_limit_h)
{
        const double tau_rec = 42;
        const double tau_rise = 3;

	const double baseline_limit = 0.006;

	const double time_first = xv[time_front[0]];
	//const double time_first = xv[time_start[current_signal]];


	// A
	fitFcn->SetParameter(0, A_start[0]);
	fitFcn->SetParLimits(0, A_limit_l[0], A_limit_h[0]);

	//t_0
	fitFcn->SetParameter(1, time_first);
	fitFcn->SetParLimits(1, xv[time_start_index], xv[time_finish_index]); //simple variant
	//fitFcn->SetParLimits(1, xv[time_start_index] + 5, time_first); // more complex variant

	//fitFcn->SetParameter(1, time_first + 5);
	//fitFcn->SetParLimits(1, xv[time_start_index] + 5, time_first+10); // slava variant

	// tau_rec
	fitFcn->SetParameter(2, tau_rec);
	fitFcn->SetParLimits(2, tau_rec, tau_rec);

	// tau_rise
	fitFcn->SetParameter(3, tau_rise);
	fitFcn->SetParLimits(3, tau_rise, tau_rise);

	//baseline
	fitFcn->SetParameter(4, 0);
	fitFcn->SetParLimits(4, -baseline_limit, baseline_limit);

}


void RootFit::SetParametersTwoComp_fit2(const double * const A_start, const double * const A_limit_l, const double * const A_limit_h)
{
	const double tau_rec_fast = 2.21513;
	const double tau_rise = 5.47845;

	const double baseline_limit = 0.006;
	const double sigma = 1.78391;

	const double tau_rec_slow = 43.0645;
	const double R_slow = 0.254653;

	const double time_first = xv[time_front[0]];
	//const double time_first = xv[time_start[current_signal]];

		// A
		fitFcn->SetParameter(0, A_start[0]);
		fitFcn->SetParLimits(0, A_limit_l[0], A_limit_h[0]);

		//t_0
		fitFcn->SetParameter(1, time_first);
		fitFcn->SetParLimits(1, xv[time_start_index], xv[time_finish_index]);

		// tau_rec
		fitFcn->SetParameter(2, tau_rec_fast);
		fitFcn->SetParLimits(2, tau_rec_fast, tau_rec_fast);

		// tau_rise
		fitFcn->SetParameter(3, tau_rise);
		fitFcn->SetParLimits(3, tau_rise, tau_rise);

		//sigma
		fitFcn->SetParameter(4, sigma);
		fitFcn->SetParLimits(4, sigma, sigma);

		//tau_rec_slow
		fitFcn->SetParameter(5, tau_rec_slow);
		fitFcn->SetParLimits(5, tau_rec_slow, tau_rec_slow);

		//R_slow
		fitFcn->SetParameter(6, R_slow);
		fitFcn->SetParLimits(6, R_slow, R_slow);

		//baseline
		fitFcn->SetParameter(7, 0);
		fitFcn->SetParLimits(7, -baseline_limit, baseline_limit);

		//double time_second = xv[time_start[current_signal]];
		
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
		fitFcn->SetParameter(8, A_start[1]);
		fitFcn->SetParLimits(8, A_limit_l[1], A_limit_h[1]);

		//t_0
		fitFcn->SetParameter(9, time_second);
		fitFcn->SetParLimits(9, xv[time_start_index], xv[time_finish_index]);

		// tau_rec
		fitFcn->SetParameter(10, tau_rec_fast);
		fitFcn->SetParLimits(10, tau_rec_fast, tau_rec_fast);

		// tau_rise
		fitFcn->SetParameter(11, tau_rise);
		fitFcn->SetParLimits(11, tau_rise, tau_rise);

		//sigma
		fitFcn->SetParameter(12, sigma);
		fitFcn->SetParLimits(12, sigma, sigma);

		//tau_rec_slow
		fitFcn->SetParameter(13, tau_rec_slow);
		fitFcn->SetParLimits(13, tau_rec_slow, tau_rec_slow);

		//R_slow
		fitFcn->SetParameter(14, R_slow);
		fitFcn->SetParLimits(14, R_slow, R_slow);

	
}


void RootFit::SetParametersOneComp_fit2(const double * const A_start, const double * const A_limit_l, const double * const A_limit_h)
{
        const double tau_rec = 42;
        const double tau_rise = 3;

	const double baseline_limit = 0.006;

	const double time_first = xv[time_front[0]];
	//const double time_first = xv[time_start[current_signal]];


	// A
	fitFcn->SetParameter(0, A_start[0]);
	fitFcn->SetParLimits(0, A_limit_l[0], A_limit_h[0]);

	//t_0
	fitFcn->SetParameter(1, time_first);
	fitFcn->SetParLimits(1, xv[time_start_index], xv[time_finish_index]); //simple variant
	//fitFcn->SetParLimits(1, xv[time_start_index] + 5, time_first); // more complex variant

	//fitFcn->SetParameter(1, time_first + 5);
	//fitFcn->SetParLimits(1, xv[time_start_index] + 5, time_first+10); // slava variant

	// tau_rec
	fitFcn->SetParameter(2, tau_rec);
	fitFcn->SetParLimits(2, tau_rec, tau_rec);

	// tau_rise
	fitFcn->SetParameter(3, tau_rise);
	fitFcn->SetParLimits(3, tau_rise, tau_rise);

	//baseline
	fitFcn->SetParameter(4, 0);
	fitFcn->SetParLimits(4, -baseline_limit, baseline_limit);

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
	fitFcn->SetParameter(5, A_start[1]);
	fitFcn->SetParLimits(5, A_limit_l[1], A_limit_h[1]);

	//t_0
	fitFcn->SetParameter(6, time_second);
	fitFcn->SetParLimits(6, xv[time_start_index], xv[time_finish_index]);

	// tau_rec
	fitFcn->SetParameter(7, tau_rec);
	fitFcn->SetParLimits(7, tau_rec, tau_rec);

	// tau_rise
	fitFcn->SetParameter(8, tau_rise);
	fitFcn->SetParLimits(8, tau_rise, tau_rise);

}

void RootFit::SetParametersTwoComp_fit3(const double * const A_start, const double * const A_limit_l, const double * const A_limit_h)
{
	const double tau_rec_fast = 2.21513;
	const double tau_rise = 5.47845;

	const double baseline_limit = 0.006;
	const double sigma = 1.78391;

	const double tau_rec_slow = 43.0645;
	const double R_slow = 0.254653;

	//const double time_first = xv[time_front[0]];
	const double time_first = xv[time_start[current_signal]];

	// A
	fitFcn->SetParameter(0, A_start[0]);
	fitFcn->SetParLimits(0, A_limit_l[0], A_limit_h[0]);

	//t_0
	fitFcn->SetParameter(1, time_first);
	fitFcn->SetParLimits(1, xv[time_start_index], xv[time_finish_index]);

	// tau_rec
	fitFcn->SetParameter(2, tau_rec_fast);
	fitFcn->SetParLimits(2, tau_rec_fast, tau_rec_fast);

	// tau_rise
	fitFcn->SetParameter(3, tau_rise);
	fitFcn->SetParLimits(3, tau_rise, tau_rise);

	//sigma
	fitFcn->SetParameter(4, sigma);
	fitFcn->SetParLimits(4, sigma, sigma);

	//tau_rec_slow
	fitFcn->SetParameter(5, tau_rec_slow);
	fitFcn->SetParLimits(5, tau_rec_slow, tau_rec_slow);

	//R_slow
	fitFcn->SetParameter(6, R_slow);
	fitFcn->SetParLimits(6, R_slow, R_slow);

	//baseline
	fitFcn->SetParameter(7, 0);
	fitFcn->SetParLimits(7, -baseline_limit, baseline_limit);

	//double time_second;
	//if (time_front.size() > 1)
	//{
	//	time_second = xv[time_front[1]];
	//}
	//else
	//{
	//	time_second = xv[time_front[0]];
	//}

	const double time_second = xv[time_start[current_signal]];


	// A
	fitFcn->SetParameter(8, A_start[1]);
	fitFcn->SetParLimits(8, A_limit_l[1], A_limit_h[1]);

	//t_0
	fitFcn->SetParameter(9, time_second);
	fitFcn->SetParLimits(9, xv[time_start_index], xv[time_finish_index]);

	// tau_rec
	fitFcn->SetParameter(10, tau_rec_fast);
	fitFcn->SetParLimits(10, tau_rec_fast, tau_rec_fast);

	// tau_rise
	fitFcn->SetParameter(11, tau_rise);
	fitFcn->SetParLimits(11, tau_rise, tau_rise);

	//sigma
	fitFcn->SetParameter(12, sigma);
	fitFcn->SetParLimits(12, sigma, sigma);

	//tau_rec_slow
	fitFcn->SetParameter(13, tau_rec_slow);
	fitFcn->SetParLimits(13, tau_rec_slow, tau_rec_slow);

	//R_slow
	fitFcn->SetParameter(14, R_slow);
	fitFcn->SetParLimits(14, R_slow, R_slow);


	//double time_third;
	//if (time_front.size() > 2)
	//{
	//	time_second = xv[time_front[2]];
	//}
	//else
	//{
	//	time_second = xv[time_front[0]];
	//}

	const double time_third = xv[time_start[current_signal]];

	// A
	fitFcn->SetParameter(15, A_start[2]);
	fitFcn->SetParLimits(15, A_limit_l[2], A_limit_h[2]);

	//t_0
	fitFcn->SetParameter(16, time_second);
	fitFcn->SetParLimits(16, xv[time_start_index], xv[time_finish_index]);

	// tau_rec
	fitFcn->SetParameter(17, tau_rec_fast);
	fitFcn->SetParLimits(17, tau_rec_fast, tau_rec_fast);

	// tau_rise
	fitFcn->SetParameter(18, tau_rise);
	fitFcn->SetParLimits(18, tau_rise, tau_rise);

	//sigma
	fitFcn->SetParameter(19, sigma);
	fitFcn->SetParLimits(19, sigma, sigma);

	//tau_rec_slow
	fitFcn->SetParameter(20, tau_rec_slow);
	fitFcn->SetParLimits(20, tau_rec_slow, tau_rec_slow);

	//R_slow
	fitFcn->SetParameter(21, R_slow);
	fitFcn->SetParLimits(21, R_slow, R_slow);

}


//void RootFit::SaveAllGraphs()
//{
//
//	if (this->number_of_functions == 1)
//	{
//		////записать графики с условием отбора
//		//bool flag_temp = Fit_single->fitFcn->GetParameter(0) < 0.03;
//		//if (flag_temp && Fit_single->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//		//{
//		//	Fit_single->SaveGraphs(Monostate::Hlist_test);
//		//}		
//
//
//		if (SaveGraphsBool)
//		{
//			//записать все графики
//			this->SaveGraphs(Monostate::Hlist_f1);
//
//			//записать графики с плохим Chi2 после фита одной функцией
//			if (this->GetChi2PerDof() > Monostate::chi2_per_dof_th)
//			{
//				this->SaveGraphs(Monostate::Hlist_f1_bad);
//			}
//
//
//			if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//			{
//				//записать графики с хорошим Chi2 после фита одной функцией
//				this->SaveGraphs(Monostate::Hlist_f1_good);
//
//				//записать все графики после фита одной функцией
//				this->SaveGraphs(Monostate::Hlist_f2);
//			}
//
//
//
//			if (this->GetChi2PerDof() < 6 && this->GetChi2PerDof() > 2 && fitFcn->GetParameter(0) > 0.06 && fitFcn->GetParameter(0) < 0.1)
//			{
//				this->SaveGraphs(Monostate::Hlist_test_2);
//			}
//		}
//
//		//записать корреляцию амплитуда - chi2 / dof для всех сигналов
//		//Monostate::amp_chi2_fnc1_all_signals << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//
//
//		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//		{
//			if (only_1e)
//			{
//				//печатать в файл амплитуду и время успешно фитированных импульсов
//				bool condition_0 = fitFcn->GetParameter(0) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(0) < RootFit::threshold_1e_A_high;
//				double a = fitFcn->GetParameter(0);
//				if (condition_0)
//					this->Print_dt_amp();
//				else
//					previousIs1e = false;				
//			}
//			else
//			{
//				this->Print_dt_amp();
//			}
//		}
//
//	}
//
//	if (this->number_of_functions == 2)
//	{
//		////записать графики с условием отбора
//		//bool flag_temp = Fit_double->fitFcn->GetParameter(0) < 0.03 || Fit_double->fitFcn->GetParameter(6) < 0.03;
//		//if (flag_temp && Fit_double->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//		//{
//		//	Fit_double->SaveGraphs(Monostate::Hlist_test);
//		//}
//
//		if (SaveGraphsBool)
//		{
//			//записать все графики после фита одной функцией
//			this->SaveGraphs(Monostate::Hlist_f2);
//
//			//записать графики с плохим Chi2 после фита двумя функциями
//			if (this->GetChi2PerDof() > Monostate::chi2_per_dof_th)
//			{
//				this->SaveGraphs(Monostate::Hlist_f2_bad);
//			}
//
//
//
//			if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//			{
//				//записать графики с хорошим Chi2 после фита двумя функциями
//				this->SaveGraphs(Monostate::Hlist_f2_good);
//			}
//
//
//			
//
//			//if (this->GetChi2PerDof() < 4 && ((fitFcn->GetParameter(0) + fitFcn->GetParameter(8)) > 0.25) && ((fitFcn->GetParameter(0) + fitFcn->GetParameter(8)) < 0.45))
//			//{
//			//	this->SaveGraphs(Monostate::Hlist_chi2_amp_cut_2);
//			//	double dt = fabs(fitFcn->GetParameter(9) - fitFcn->GetParameter(1));
//			//	Monostate::amp_chi2_dt_fnc2_all_signals << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetParameter(8) << "\t" << dt << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			//}
//		}
//
//		bool SummAmp = true;
//		if (!SummAmp)
//		{
//			//записать корреляцию амплитуда - chi2 / dof для всех сигналов
//			Monostate::amp_chi2_fnc2_all_signals << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc2_all_signals << setprecision(17) << fitFcn->GetParameter(8) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//		}
//		else
//		{
//			Monostate::amp_chi2_fnc2_all_signals << setprecision(17) << fitFcn->GetParameter(0) + fitFcn->GetParameter(8) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//
//		}
//
//
//
//		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//		{
//			if (only_1e)
//			{
//				//печатать в файл амплитуду и время успешно фитированных импульсов
//				bool condition_0 = fitFcn->GetParameter(0) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(0) < RootFit::threshold_1e_A_high;
//				bool condition_6;
//				if(!RecoveryTimeTwoComponents)
//					condition_6 = fitFcn->GetParameter(6) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(6) < RootFit::threshold_1e_A_high;
//				else
//					condition_6 = fitFcn->GetParameter(8) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(8) < RootFit::threshold_1e_A_high;
//
//				if (condition_0 && condition_6)
//					this->Print_dt_amp();
//				else
//					previousIs1e = false;
//				
//			}
//			else
//			{
//				this->Print_dt_amp();
//			}
//		}
//
//
//	}
//
//	if (this->number_of_functions == 3)
//	{
//		//записать графики с условием отбора
//		/*bool flag_temp = this->fitFcn->GetParameter(0) < 0.03 || this->fitFcn->GetParameter(6) < 0.03 || this->fitFcn->GetParameter(11) < 0.03;
//		if (flag_temp && this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//		{
//		this->SaveGraphs(Monostate::Hlist_test);
//		}*/
//
//		if (SaveGraphsBool)
//		{
//			//записать графики, после фита тремя функциями
//			this->SaveGraphs(Monostate::Hlist_f3);
//
//			//записать графики с плохим Chi2 после фита тремя функциями
//			if (this->GetChi2PerDof() > Monostate::chi2_per_dof_th)
//			{
//				this->SaveGraphs(Monostate::Hlist_f3_bad);
//			}
//
//			//записать графики с хорошим Chi2 после фита тремя функциями
//			if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//			{
//				this->SaveGraphs(Monostate::Hlist_f3_good);
//			}
//		}
//
//
//
//		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//		{
//			if (only_1e)
//			{
//				bool condition_0 = fitFcn->GetParameter(0) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(0) < RootFit::threshold_1e_A_high;
//				bool condition_6;
//				bool condition_11;
//
//				if (!RecoveryTimeTwoComponents)
//				{
//					condition_6 = fitFcn->GetParameter(6) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(6) < RootFit::threshold_1e_A_high;
//					condition_11 = fitFcn->GetParameter(11) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(11) < RootFit::threshold_1e_A_high;
//				}
//				else
//				{
//					condition_6 = fitFcn->GetParameter(8) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(8) < RootFit::threshold_1e_A_high;
//					condition_11 = fitFcn->GetParameter(15) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(15) < RootFit::threshold_1e_A_high;
//				}
//				//печатать в файл амплитуду и время успешно фитированных импульсов
//
//				if (condition_0 && condition_6 && condition_11)
//					this->Print_dt_amp();
//				else
//					previousIs1e = false;
//			}
//			else
//			{
//				this->Print_dt_amp();
//			}
//		}
//
//		//if (fitFcn->GetParameter(0) > 0.02)
//		//{
//		//	
//		//	time_test << fitFcn_fnc3->GetParameter(1) << "\t" << fitFcn_fnc3->GetParameter(7) << "\t" << fitFcn_fnc3->GetParameter(13) << endl;
//
//		//} 
//	}
//
//
//	if (this->number_of_functions == 4)
//	{
//		
//		if (SaveGraphsBool)
//		{
//			//записать графики с плохим Chi2 после фита 4 функциями
//			if (this->GetChi2PerDof() > Monostate::chi2_per_dof_th)
//			{
//				this->SaveGraphs(Monostate::Hlist_f4_bad);
//			}
//
//			//записать графики с хорошим Chi2 после фита 4 функциями
//			if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//			{
//				this->SaveGraphs(Monostate::Hlist_f4_good);
//			}
//		}
//
//		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//		{
//			if (only_1e)
//			{
//				bool condition_0 = fitFcn->GetParameter(0) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(0) < RootFit::threshold_1e_A_high;
//				bool condition_6;
//				bool condition_11;
//				bool condition_16;
//
//				if (!RecoveryTimeTwoComponents)
//				{
//					condition_6 = fitFcn->GetParameter(6) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(6) < RootFit::threshold_1e_A_high;
//					condition_11 = fitFcn->GetParameter(11) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(11) < RootFit::threshold_1e_A_high;
//					condition_16 = fitFcn->GetParameter(16) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(16) < RootFit::threshold_1e_A_high;
//				}
//				else
//				{
//					condition_6 = fitFcn->GetParameter(8) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(8) < RootFit::threshold_1e_A_high;
//					condition_11 = fitFcn->GetParameter(15) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(15) < RootFit::threshold_1e_A_high;
//					condition_16 = fitFcn->GetParameter(22) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(22) < RootFit::threshold_1e_A_high;
//				}
//
//				//печатать в файл амплитуду и время успешно фитированных импульсов
//				if (condition_0 && condition_6 && condition_11 && condition_16)
//					this->Print_dt_amp();
//				else
//					previousIs1e = false;
//			}
//			else
//			{
//				this->Print_dt_amp();
//			}
//		}
//	}
//
//	if (this->number_of_functions == 5)
//	{
//		
//		if (SaveGraphsBool)
//		{
//			//записать графики с хорошим Chi2 после фита 5 функциями
//			if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//			{
//				this->SaveGraphs(Monostate::Hlist_f5_good);
//			}
//
//			//записать графики с плохим Chi2 после фита 5 функциями
//			if (this->GetChi2PerDof() > Monostate::chi2_per_dof_th)
//			{
//				this->SaveGraphs(Monostate::Hlist_f5_bad);
//			}
//		}
//
//		if (this->GetChi2PerDof() < Monostate::chi2_per_dof_th)
//		{
//			if (only_1e)
//			{
//				//записать графики с хорошим Chi2 после фита 5 функциями
//				bool condition_0 = fitFcn->GetParameter(0) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(0) < RootFit::threshold_1e_A_high;
//				bool condition_6;
//				bool condition_11;
//				bool condition_16;
//				bool condition_21;
//
//				if (!RecoveryTimeTwoComponents)
//				{
//					condition_6 = fitFcn->GetParameter(6) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(6) < RootFit::threshold_1e_A_high;
//					condition_11 = fitFcn->GetParameter(11) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(11) < RootFit::threshold_1e_A_high;
//					condition_16 = fitFcn->GetParameter(16) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(16) < RootFit::threshold_1e_A_high;
//					condition_21 = fitFcn->GetParameter(21) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(21) < RootFit::threshold_1e_A_high;
//				}
//				else
//				{
//					condition_6 = fitFcn->GetParameter(8) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(8) < RootFit::threshold_1e_A_high;
//					condition_11 = fitFcn->GetParameter(15) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(15) < RootFit::threshold_1e_A_high;
//					condition_16 = fitFcn->GetParameter(22) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(22) < RootFit::threshold_1e_A_high;
//					condition_21 = fitFcn->GetParameter(29) > RootFit::threshold_1e_A_low && fitFcn->GetParameter(29) < RootFit::threshold_1e_A_high;
//				}
//
//				if (condition_0 && condition_6 && condition_11 && condition_16 && condition_21)
//					this->Print_dt_amp();
//				else
//					previousIs1e = false;
//			}
//			else
//			{
//				this->Print_dt_amp();
//			}
//		}
//		else
//		{
//			previousIs1e = false;
//		}
//	}
//
//
//}

//функтор
struct sort_pred
{
	bool operator()(const std::pair<int, int> &left, const std::pair<int, int> &right)
	{
		return left.first < right.first;
	}
};


//void RootFit::Print_dt_amp()
//{
//        vector<pair<double, double> > v_pairs;
//	pair<double, double> pr;
//
//
//	if (this->number_of_functions == 1)
//	{
//		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//		Monostate::time_i << setprecision(17) << fitFcn->GetParameter(1) << "\t" << fitFcn->GetParameter(0) << endl;
//
//		Monostate::file_amp << setprecision(17) << fitFcn->GetParameter(0) << endl;
//
//
//		if (only_1e)
//		{
//			if (previousIs1e)
//			{
//				Monostate::file_dt << fitFcn->GetParameter(1) - previous_time << endl;
//
//				//if (fitFcn->GetParameter(1) - previous_time > 3000)
//				//{
//				//	cout << endl << "strange times (ns) is " << previous_time << "\t" << fitFcn->GetParameter(1) << endl;
//				//	cout << endl << "strange times (points) is " << previous_time * 5 << "\t" << fitFcn->GetParameter(1) * 5 << endl;
//
//				//	int a = int(previous_time * 5) - 300;
//				//	int b = int(fitFcn->GetParameter(1) * 5) + 300;
//
//				//	TGraphErrors *gr_long_dt = new TGraphErrors(b - a, &xv[a], &yv[a], &xverr[a], &yverr[a]);
//				//	gr_long_dt->SetMarkerColor(4);
//				//	gr_long_dt->SetMarkerStyle(kFullCircle);
//
//				//	TGraph *gr_der_long_dt = new TGraph(b - a, &xv[a], &yv_der[a]);
//				//	for (int i = 0; i < gr_der_long_dt->GetN(); i++) gr_der_long_dt->GetY()[i] *= 50;
//
//				//	TMultiGraph *mg_long_dt = new TMultiGraph();
//
//				//	mg_long_dt->Add(gr_long_dt);
//				//	mg_long_dt->Add(gr_der_long_dt);
//
//				//	Monostate::Hlist_test.Add(mg_long_dt);
//				//	/*for (int k = int(previous_time * 5); k < int(fitFcn->GetParameter(1) * 5); k++)
//				//	{
//				//	Monostate::file_long_dt << xv[k] << "\t" << yv[k] << endl;
//				//	}
//
//				//	system("pause");*/
//				//	//this->SaveGraphs(Monostate::Hlist_test);
//
//				//	//delete mg_long_dt;
//				//	//delete gr_der_long_dt;				
//				//	//delete gr_long_dt;
//
//				//}
//
//				previous_time = fitFcn->GetParameter(1);
//			}
//			else
//			{
//				previous_time = fitFcn->GetParameter(1);
//				previousIs1e = true;
//			}
//		}
//		else
//		{
//			Monostate::file_dt << fitFcn->GetParameter(1) - previous_time << endl;
//			previous_time = fitFcn->GetParameter(1);
//		}
//
//		/*if (!PreviousIsSingle)
//		{
//		temp_time_i = fitFcn->GetParameter(1);
//		}
//		else
//		{
//		Monostate::file_dt << fitFcn->GetParameter(1) - temp_time_i << endl;
//		temp_time_i = fitFcn->GetParameter(1);
//		}
//
//		PreviousIsSingle = true;*/
//	}
//
//	if (this->number_of_functions == 2)
//	{
//		
//		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//		if (!RecoveryTimeTwoComponents)
//		{
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(6) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//		}
//		else
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(8) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//
//		pr.first = fitFcn->GetParameter(1);
//		pr.second = fitFcn->GetParameter(0);
//		v_pairs.push_back(pr);
//
//		if (!RecoveryTimeTwoComponents)
//		{
//			pr.first = fitFcn->GetParameter(7);
//			pr.second = fitFcn->GetParameter(6);
//			v_pairs.push_back(pr);
//		}
//		else
//		{
//			pr.first = fitFcn->GetParameter(9);
//			pr.second = fitFcn->GetParameter(8);
//			v_pairs.push_back(pr);
//		}
//
//		sort(v_pairs.begin(), v_pairs.end(), sort_pred());
//
//		Monostate::time_i << v_pairs[0].first << "\t" << v_pairs[0].second << endl;
//		Monostate::time_i << v_pairs[1].first << "\t" << v_pairs[1].second << endl;
//
//		Monostate::file_amp << v_pairs[0].second << endl;
//		Monostate::file_amp << v_pairs[1].second << endl;
//
//		if (v_pairs[1].first - v_pairs[0].first < 100)
//		{
//			int a = int(v_pairs[0].first * 5) - 300;
//			int b = int(v_pairs[1].first * 5) + 300;
//
//			TGraphErrors *gr_long_dt = new TGraphErrors(b - a, &xv[a], &yv[a], &xverr[a], &yverr[a]);
//			gr_long_dt->SetMarkerColor(4);
//			gr_long_dt->SetMarkerStyle(kFullCircle);
//
//			TGraph *gr_der_long_dt = new TGraph(b - a, &xv[a], &yv_der[a]);
//			for (int i = 0; i < gr_der_long_dt->GetN(); i++) gr_der_long_dt->GetY()[i] *= 50;
//
//			TMultiGraph *mg_long_dt = new TMultiGraph();
//
//			
//			mg_long_dt->Add(gr_long_dt);
//			mg_long_dt->Add(gr_der_long_dt);
//			mg_long_dt->Add(gr);
//
//			Monostate::Hlist_test.Add(mg_long_dt);
//			/*for (int k = int(previous_time * 5); k < int(fitFcn->GetParameter(1) * 5); k++)
//			{
//			Monostate::file_long_dt << xv[k] << "\t" << yv[k] << endl;
//			}
//
//			system("pause");*/
//			//this->SaveGraphs(Monostate::Hlist_test);
//
//			//delete mg_long_dt;
//			//delete gr_der_long_dt;				
//			//delete gr_long_dt;
//
//		}
//
//		if (only_1e)
//		{
//			if (previousIs1e)
//			{
//				Monostate::file_dt << v_pairs[0].first - previous_time << endl;
//				Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//				previous_time = v_pairs[1].first;
//			}
//			else
//			{
//				Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//				previous_time = v_pairs[1].first;
//				previousIs1e = true;
//			}
//		}
//		else
//		{
//			Monostate::file_dt << v_pairs[0].first - previous_time << endl;
//			Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//			previous_time = v_pairs[1].first;
//		}
//
//	}
//
//	if (this->number_of_functions == 3)
//	{
//		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//
//		if (!RecoveryTimeTwoComponents)
//		{
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(6) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(11) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//		}
//		else
//		{
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(8) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(15) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//		}
//
//		pr.first = fitFcn->GetParameter(1);
//		pr.second = fitFcn->GetParameter(0);
//		v_pairs.push_back(pr);
//
//		if (!RecoveryTimeTwoComponents)
//		{
//			pr.first = fitFcn->GetParameter(7);
//			pr.second = fitFcn->GetParameter(6);
//			v_pairs.push_back(pr);
//
//			pr.first = fitFcn->GetParameter(12);
//			pr.second = fitFcn->GetParameter(11);
//			v_pairs.push_back(pr);
//		}
//		else
//		{
//			pr.first = fitFcn->GetParameter(9);
//			pr.second = fitFcn->GetParameter(8);
//			v_pairs.push_back(pr);
//
//			pr.first = fitFcn->GetParameter(16);
//			pr.second = fitFcn->GetParameter(15);
//			v_pairs.push_back(pr);
//		}
//
//		sort(v_pairs.begin(), v_pairs.end(), sort_pred());
//
//		Monostate::time_i << setprecision(17) << v_pairs[0].first << "\t" << v_pairs[0].second << endl;
//		Monostate::time_i << setprecision(17) << v_pairs[1].first << "\t" << v_pairs[1].second << endl;
//		Monostate::time_i << setprecision(17) << v_pairs[2].first << "\t" << v_pairs[2].second << endl;
//
//		Monostate::file_amp << v_pairs[0].second << endl;
//		Monostate::file_amp << v_pairs[1].second << endl;
//		Monostate::file_amp << v_pairs[2].second << endl;
//
//		if (only_1e)
//		{
//			if (previousIs1e)
//			{
//				Monostate::file_dt << v_pairs[0].first - previous_time << endl;
//				Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//				Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
//				previous_time = v_pairs[2].first;
//			}
//			else
//			{
//				Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//				Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
//				previous_time = v_pairs[2].first;
//				previousIs1e = true;
//			}
//		}
//		else
//		{
//			Monostate::file_dt << v_pairs[0].first - previous_time << endl;
//			Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//			Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
//			previous_time = v_pairs[2].first;
//		}
//
//		//Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//		//Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
//
//		/*if (!PreviousIsSingle)
//			Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//			else
//			Monostate::file_dt << v_pairs[0].first - temp_time_i << endl;
//
//			PreviousIsSingle = false;*/
//	}
//
//	if (this->number_of_functions == 4)
//	{
//		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//
//
//		if (!RecoveryTimeTwoComponents)
//		{
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(6) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(11) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(11 + 5) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//		}
//		else
//		{
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(8) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(15) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(22) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//		}
//
//		pr.first = fitFcn->GetParameter(1);
//		pr.second = fitFcn->GetParameter(0);
//		v_pairs.push_back(pr);
//
//
//
//		if (!RecoveryTimeTwoComponents)
//		{
//			pr.first = fitFcn->GetParameter(7);
//			pr.second = fitFcn->GetParameter(6);
//			v_pairs.push_back(pr);
//
//			pr.first = fitFcn->GetParameter(12);
//			pr.second = fitFcn->GetParameter(11);
//			v_pairs.push_back(pr);
//
//			pr.first = fitFcn->GetParameter(17);
//			pr.second = fitFcn->GetParameter(16);
//			v_pairs.push_back(pr);
//		}
//		else
//		{
//			pr.first = fitFcn->GetParameter(9);
//			pr.second = fitFcn->GetParameter(8);
//			v_pairs.push_back(pr);
//
//			pr.first = fitFcn->GetParameter(16);
//			pr.second = fitFcn->GetParameter(15);
//			v_pairs.push_back(pr);
//
//			pr.first = fitFcn->GetParameter(23);
//			pr.second = fitFcn->GetParameter(22);
//			v_pairs.push_back(pr);
//		}
//
//		sort(v_pairs.begin(), v_pairs.end(), sort_pred());
//
//		Monostate::time_i << setprecision(17) << v_pairs[0].first << "\t" << v_pairs[0].second << endl;
//		Monostate::time_i << setprecision(17) << v_pairs[1].first << "\t" << v_pairs[1].second << endl;
//		Monostate::time_i << setprecision(17) << v_pairs[2].first << "\t" << v_pairs[2].second << endl;
//		Monostate::time_i << setprecision(17) << v_pairs[3].first << "\t" << v_pairs[3].second << endl;
//
//		Monostate::file_amp << v_pairs[0].second << endl;
//		Monostate::file_amp << v_pairs[1].second << endl;
//		Monostate::file_amp << v_pairs[2].second << endl;
//		Monostate::file_amp << v_pairs[3].second << endl;
//
//		if (only_1e)
//		{
//			if (previousIs1e)
//			{
//				Monostate::file_dt << v_pairs[0].first - previous_time << endl;
//				Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//				Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
//				Monostate::file_dt << v_pairs[3].first - v_pairs[2].first << endl;
//				previous_time = v_pairs[3].first;
//			}
//			else
//			{
//				Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//				Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
//				Monostate::file_dt << v_pairs[3].first - v_pairs[2].first << endl;
//				previous_time = v_pairs[3].first;
//				previousIs1e = true;
//			}
//		}
//		else
//		{
//			Monostate::file_dt << v_pairs[0].first - previous_time << endl;
//			Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//			Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
//			Monostate::file_dt << v_pairs[3].first - v_pairs[2].first << endl;
//			previous_time = v_pairs[3].first;
//		}
//
//		//Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//		//Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
//		//Monostate::file_dt << v_pairs[3].first - v_pairs[2].first << endl;
//
//		/*if (!PreviousIsSingle)
//			Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//			else
//			Monostate::file_dt << v_pairs[0].first - temp_time_i << endl;
//
//			PreviousIsSingle = false;*/
//	}
//
//	if (this->number_of_functions == 5)
//	{
//		Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(0) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//
//		if (!RecoveryTimeTwoComponents)
//		{
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(6) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(11) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(11 + 5) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(11 + 10) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//		}
//		else
//		{
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(8) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(15) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(22) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//			Monostate::amp_chi2_fnc1 << setprecision(17) << fitFcn->GetParameter(29) << "\t" << fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
//		}
//
//		pr.first = fitFcn->GetParameter(1);
//		pr.second = fitFcn->GetParameter(0);
//		v_pairs.push_back(pr);
//
//		if (!RecoveryTimeTwoComponents)
//		{
//			pr.first = fitFcn->GetParameter(7);
//			pr.second = fitFcn->GetParameter(6);
//			v_pairs.push_back(pr);
//
//			pr.first = fitFcn->GetParameter(12);
//			pr.second = fitFcn->GetParameter(11);
//			v_pairs.push_back(pr);
//
//			pr.first = fitFcn->GetParameter(17);
//			pr.second = fitFcn->GetParameter(16);
//			v_pairs.push_back(pr);
//
//			pr.first = fitFcn->GetParameter(22);
//			pr.second = fitFcn->GetParameter(21);
//			v_pairs.push_back(pr);
//		}
//		else
//		{
//			pr.first = fitFcn->GetParameter(9);
//			pr.second = fitFcn->GetParameter(8);
//			v_pairs.push_back(pr);
//
//			pr.first = fitFcn->GetParameter(16);
//			pr.second = fitFcn->GetParameter(15);
//			v_pairs.push_back(pr);
//
//			pr.first = fitFcn->GetParameter(23);
//			pr.second = fitFcn->GetParameter(22);
//			v_pairs.push_back(pr);
//
//			pr.first = fitFcn->GetParameter(30);
//			pr.second = fitFcn->GetParameter(29);
//			v_pairs.push_back(pr);
//		}
//
//		sort(v_pairs.begin(), v_pairs.end(), sort_pred());
//
//		Monostate::time_i << setprecision(17) << v_pairs[0].first << "\t" << v_pairs[0].second << endl;
//		Monostate::time_i << setprecision(17) << v_pairs[1].first << "\t" << v_pairs[1].second << endl;
//		Monostate::time_i << setprecision(17) << v_pairs[2].first << "\t" << v_pairs[2].second << endl;
//		Monostate::time_i << setprecision(17) << v_pairs[3].first << "\t" << v_pairs[3].second << endl;
//		Monostate::time_i << setprecision(17) << v_pairs[4].first << "\t" << v_pairs[4].second << endl;
//
//		Monostate::file_amp << v_pairs[0].second << endl;
//		Monostate::file_amp << v_pairs[1].second << endl;
//		Monostate::file_amp << v_pairs[2].second << endl;
//		Monostate::file_amp << v_pairs[3].second << endl;
//		Monostate::file_amp << v_pairs[4].second << endl;
//
//		if (only_1e)
//		{
//			if (previousIs1e)
//			{
//				Monostate::file_dt << v_pairs[0].first - previous_time << endl;
//				Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//				Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
//				Monostate::file_dt << v_pairs[3].first - v_pairs[2].first << endl;
//				Monostate::file_dt << v_pairs[4].first - v_pairs[3].first << endl;
//				previous_time = v_pairs[4].first;
//			}
//			else
//			{
//				Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//				Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
//				Monostate::file_dt << v_pairs[3].first - v_pairs[2].first << endl;
//				Monostate::file_dt << v_pairs[4].first - v_pairs[3].first << endl;
//				previous_time = v_pairs[4].first;
//				previousIs1e = true;
//			}
//		}
//		else
//		{
//			Monostate::file_dt << v_pairs[0].first - previous_time << endl;
//			Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//			Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
//			Monostate::file_dt << v_pairs[3].first - v_pairs[2].first << endl;
//			Monostate::file_dt << v_pairs[4].first - v_pairs[3].first << endl;
//			previous_time = v_pairs[4].first;
//		}
//
//		//Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//		///Monostate::file_dt << v_pairs[2].first - v_pairs[1].first << endl;
//		//Monostate::file_dt << v_pairs[3].first - v_pairs[2].first << endl;
//		//Monostate::file_dt << v_pairs[4].first - v_pairs[3].first << endl;
//
//		/*if (!PreviousIsSingle)
//			Monostate::file_dt << v_pairs[1].first - v_pairs[0].first << endl;
//			else
//			Monostate::file_dt << v_pairs[0].first - temp_time_i << endl;
//
//			PreviousIsSingle = false;*/
//
//	}
//
//}

void RootFit::CalculateStaircases_der(double time_dead)
{
	
	cout << "CalculateStaircases_der " << endl;
      string temp = Monostate::dir_name + "staitcase_der.dat";
	ofstream file_staitcase_der(temp.c_str());

	bool flag = 1;
	double x_time = 0;
	int counter = 0;
	int conter_temp = 0;

	for (double th = -0.001; th < 0; th += 0.00001)
	{
		counter = 0;
		for (unsigned int i = 0; i < xv.size(); i++)
		{
			if ((yv_der[i] < th) && flag)
			{
				x_time = xv[i];
				flag = 0;
				counter++;
			}

			if (yv_der[i] > th && (xv[i] - x_time) > (time_dead) && flag == 0)
			{
				flag = 1;
			}
		}

		if (conter_temp % 10 == 0)
		{
			cout << "counter = " << conter_temp << endl;
		}
		file_staitcase_der << th << "\t" << counter << endl;
		conter_temp++;
	}

	//system("pause");
	exit(0);
}

void RootFit::CalculateStaircases_amp(double time_dead)
{
        string temp = Monostate::dir_name + "staitcase_amp.dat";
    ofstream file_staitcase_amp(temp.c_str());
	bool flag = 1;
	double x_time = 0;
	int conter_temp = 0;

	for (double th = -0.01; th < 0; th += 0.0001)
	{
		int counter = 0;
		for (unsigned int i = 0; i < xv.size(); i++)
		{
			if ((yv[i] < th) && flag)
			{
				x_time = xv[i];
				flag = 0;
				counter++;
			}

			if (yv[i] > th && (xv[i] - x_time) > (time_dead) && flag == 0)
			{
				flag = 1;
			}
		}

		if (conter_temp % 10 == 0)
		{
			cout << "counter = " << conter_temp << endl;
		}
		conter_temp++;

		file_staitcase_amp << th << "\t" << counter << endl;
	}

	//system("pause");
	exit(0);
}


void RootFit::FindStartStop(double time_dead_signal_noise, double time_dead_forward)
{
	cout << endl << "Find start and stop" << endl;

	bool flag = 1;
	//double time_dead_signal_noise = 10 /*5*/; // ns
	//double time_dead_forward = 50; // ns

	double x_time;
	//поиск начала и конца 
	for (unsigned int i = 0; i < xv.size(); i++)
	{
		if ((/*yv_der[i] < threshold_der &&*/ yv[i] < threshold_amp_start) && flag)
		{
			x_time = xv[i];
			flag = 0;
			time_start.push_back(i);
		}

		//разрешить искать сигнал, когда сигнал дойдет до шумов и пройдет 5 нс
		if (yv[i] > threshold_amp && flag == 0 && (xv[i] - x_time) > time_dead_signal_noise)
		{
			bool flag_2 = true;

			//cout << i << "\t" << yv_der.size() << "\t" << i + 50 * 5 << "\t" << bool(yv_der.size() < i + 50 * 5) << endl;

			//разрешить искать сигнал, только если впереди на time_dead_2 [ns] нет нового импульса
			for (int j = i; j < i + time_dead_forward * 5; j++)
			{
				if (yv.size() > i + time_dead_forward * 5)
				{
					//cout << yv_der[j] << "\t" << j << "\t" << yv_der.size() << endl;
					if (yv[j] < threshold_amp)
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

//найти число сигналов на участке по 2-й производной. Необходимо задать мертвое время в нс.
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

			if (yv_der2[i] > threshold_der2 && (i - x_time) /* (xv[i] - x_time) > time_dead */ > (time_dead_index) && flag == 0)
			{
				flag = 1;
			}
		}
	}

	cout << "num_of_signals = " << num_of_signals << endl;
}

//вычислить стартовые времена t_i. Необходимо задать мертвое время в нс.
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

	int time_dead_index = time_dead * 5;
	//int shift = 20;

	//найти стартовые параметры для начала сигнала
	for (int j = time_start_index; j < time_finish[current_signal]; j++)
	{

          if (yv_der[j] < threshold_der && flag && yv[j] < (-0.007)*(0.25) ) //modification
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

       if (time_front.size() == 0)
       {
            cout << "time_front.size() == 0" << endl;
       }
       else
       {
              cout << "time_front.size() = " << time_front.size() << endl;
       }

	/*for (int i = 0; i < time_front.size(); i++)
	{
	cout << time_front[i] << "\t" << xv[time_front[i]] << endl;
	}

	system("pause");*/
}


void RootFit::CalculateDer(int type, int points)
{
	cout << endl << "Calculate derivative" << endl;
        //long int t0_Calculate_derivative = GetTickCount();

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


		const int point_half = (points - 1) / 2.0;
		//double value;

		//cout << "yv.size()" << yv.size() << endl;

		yv_der.resize(yv.size());
		yv_der2.resize(yv.size());

#pragma omp parallel for num_threads(8)
		for (int i = 0; i < xv.size(); i++)
		{
			//if (i % 100000 == 0)
			//{
			//	//long int temp_2 = GetTickCount();
			//	cout << "calculate derivative ... " << double(i) / xv.size() * 100 << " %" << endl;
			//}

			if (i < point_half || i >(xv.size() - point_half - 1))
			{
				//yv_s.push_back(0);
				yv_der[i] = 0;
				yv_der2[i] = 0;

			}
			else
			{
				double value = 0;
				for (int j = 0; j < C_i_der.size(); j++)
				{
					value += C_i_der[j] * yv[i - point_half + j];
				}
				yv_der[i] = value;


				value = 0;
				for (int j = 0; j < C_i_der.size(); j++)
				{
					value += C_i_der2[j] * yv[i - point_half + j];
				}
				yv_der2[i] = value;


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

	cout << "xv.size() " << xv.size() << endl;
	cout << "yv.size() " << yv.size() << endl;
	cout << "yv_der.size() " << yv_der.size() << endl;
	cout << "yv_der2.size() " << yv_der2.size() << endl;


	FILE *file_d = fopen(Monostate::der_name.c_str(), "wb");
	FILE* file_d2 = fopen(Monostate::der2_name.c_str(), "wb");

	if (file_d == NULL)
	{
		cout << "can't open this file " << Monostate::der_name << endl;
		//system("pause");
		exit(0);
	}

	fwrite(&yv_der[0], sizeof(vector<double>::value_type), xv.size(), file_d);
	fwrite(&yv_der2[0], sizeof(vector<double>::value_type), xv.size(), file_d2);

	fclose(file_d);
	fclose(file_d2);

	/*for (int i = 0; i < xv.size(); i++)
	{
	file_d << i << "\t" << yv_der[i] << endl;
	file_d2 << i << "\t" << yv_der2[i] << endl;

	if (i % 50000 == 0)
	{
	cout << "progress ... " << (i * 100.0) / xv.size() << endl;
	}
	}*/

        //long int t1_Calculate_derivative = GetTickCount();
        //cout << "Calculate_derivative time is (in s) " << (t1_Calculate_derivative - t0_Calculate_derivative) / 1000.0 << endl;

	//system("pause");
	//exit(0);
}

void RootFit::CalculateFilterCoeff(int points)
{
	//cout << endl << "start Calculate filter coefficients" << endl;

	//Savitzky–Golay filter
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
	//cout << endl << "stop Calculate filter coefficients" << endl;
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
	//TMultiGraph *mg = new TMultiGraph();
	//mg->Add(gr);

	//mg->Add(gr_front);

	//mg->Add(gr_der);
	//mg->Add(gr_der2);
	//Hlist.Add(mg);

	TMultiGraph *mg = new TMultiGraph();
	mg->Add(new TGraphErrors(*gr));

	mg->Add(new TGraph(*gr_front));

	mg->Add(new TGraph(*gr_der));
	mg->Add(new TGraph(*gr_der2));
	Hlist.Add(mg);

}


void RootFit::SetDispXY(const double x, const double y)
{
	xverr.resize( xv.size() );
	yverr.resize( xv.size() );
	
	//вектора с ошибками
	for (unsigned int j = 0; j < xv.size(); j++)
	{
		xverr[j] = x;
		yverr[j] = y;
	}
}




//вспомогательная функция
double RootFit::F(double t, double sigma, double tau)
{
	return TMath::Exp((sigma*sigma - 2 * t*tau) / (2 * tau*tau)) * (1 + TMath::Erf((t - sigma*sigma / tau) / (sigma * sqrt(2))));
}

//функция сигнала для быстрой компоненты времени восстановления без учета базовой линии
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

//функция сигнала для быстрой компоненты времени восстановления без учета базовой линии (sigma = 0)
double RootFit::fitFunction_nobaseline_exp(double *x, double *par)
{
	//основные параметры
	const double A = par[0];
	const double t_0 = par[1];
	const double tau_rec = par[2];
	const double tau_rise = par[3];

	double time = x[0] - t_0;

	if (time > 0)
	{
		return -A * TMath::Exp(-time / tau_rec) * (1 - TMath::Exp(-time / tau_rise));
	}
	else
	{
		return 0;
	}
	
}



//функция сигнала для быстрой и медленной компоненты времени восстановления без учета базовой линии
double RootFit::fitFunction_nobaseline_fast_slow(double *x, double *par)
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

	const double tau_rec_slow = par[5];
	const double tau_total_slow = (tau_rec_slow * tau_rise) / (tau_rec_slow + tau_rise);
	const double R_slow = par[6];

	return -(A / 2) * ((F(t, sigma, tau_rec_fast) - F(t, sigma, tau_total_fast)) + ( F(t, sigma, tau_rec_slow) - F(t, sigma, tau_total_slow) ) * R_slow);
}


//функция, которой буду фитировать
Double_t RootFit::fitFunction(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0;
}

Double_t RootFit::fitFunctionTwoComp(Double_t *x, Double_t *par)
{
	const double V_0 = par[7];
	return fitFunction_nobaseline_fast_slow(x, par) + V_0;
}

Double_t RootFit::fitFunctionOneComp(Double_t *x, Double_t *par)
{
	const double V_0 = par[4];

	return fitFunction_nobaseline_exp(x, par) + V_0;

}


//сумма двух сигналов
double RootFit::fitFunction_2(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]);
}

double RootFit::fitFunction_2_TwoComp(Double_t *x, Double_t *par)
{
	const double V_0 = par[7];
	return fitFunction_nobaseline_fast_slow(x, par) + V_0 + fitFunction_nobaseline_fast_slow(x, &par[8]);
}

double RootFit::fitFunction_2_OneComp(Double_t *x, Double_t *par)
{
	const double V_0 = par[4];

	return fitFunction_nobaseline_exp(x, par) + V_0 + fitFunction_nobaseline_exp(x, &par[5]);
}


//сумма трех сигналов
double RootFit::fitFunction_3(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]) + fitFunction_nobaseline(x, &par[11]);
}


double RootFit::fitFunction_3_TwoComp(Double_t *x, Double_t *par)
{
	const double V_0 = par[7];
	return fitFunction_nobaseline_fast_slow(x, par) + V_0 + fitFunction_nobaseline_fast_slow(x, &par[8]) + fitFunction_nobaseline_fast_slow(x, &par[15]);
}

//сумма 4-х сигналов
double RootFit::fitFunction_4(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]) + fitFunction_nobaseline(x, &par[11]) + fitFunction_nobaseline(x, &par[16]);
}

double RootFit::fitFunction_4_TwoComp(Double_t *x, Double_t *par)
{
	const double V_0 = par[7];
	return fitFunction_nobaseline_fast_slow(x, par) + V_0 + fitFunction_nobaseline_fast_slow(x, &par[8]) + fitFunction_nobaseline_fast_slow(x, &par[15]) + fitFunction_nobaseline_fast_slow(x, &par[22]);
}

//сумма 5-и сигналов
double RootFit::fitFunction_5(Double_t *x, Double_t *par)
{
	const double V_0 = par[5];
	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]) + fitFunction_nobaseline(x, &par[11]) + fitFunction_nobaseline(x, &par[16]) + fitFunction_nobaseline(x, &par[21]);
}

double RootFit::fitFunction_5_TwoComp(Double_t *x, Double_t *par)
{
	const double V_0 = par[7];
	return fitFunction_nobaseline_fast_slow(x, par) + V_0 + fitFunction_nobaseline_fast_slow(x, &par[8]) + fitFunction_nobaseline_fast_slow(x, &par[15]) + fitFunction_nobaseline_fast_slow(x, &par[22]) + fitFunction_nobaseline_fast_slow(x, &par[29]);
}


//сумма 6-и сигналов
//double RootFit::fitFunction_6(Double_t *x, Double_t *par)
//{
//	const double V_0 = par[5];
//	return fitFunction_nobaseline(x, par) + V_0 + fitFunction_nobaseline(x, &par[6]) +
//		fitFunction_nobaseline(x, &par[11]) + fitFunction_nobaseline(x, &par[16]) +
//		fitFunction_nobaseline(x, &par[21]) + fitFunction_nobaseline(x, &par[26]);
//}

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

void RootFit::CalculateAverageSignal(double time_dead_forward)
{
	cout << endl << "CalculateAverageSignal" << endl;

      string temp = Monostate::dir_name + "file_amp_average_mode.dat";
      ofstream file_amp_average_mode(temp.c_str());

	bool flag = 1;
	double x_time;
	int num_of_signals = 0;

	vector<double> yv_average;
	for (int i = 0; i < 2 * int(time_dead_forward * 5); i++)
	{
		yv_average.push_back(0);
	}


	double time_dead_der = 5; // ns
	double threshold_1e = -RootFit::threshold_1e_low;
	double threshold_2e = -RootFit::threshold_1e_high;

	//бежать по времени
	for (unsigned int i = 0; i < xv.size(); i++)
	{
		if ((yv_der[i] < threshold_der /*&& yv[i] < threshold_amp_start*/) && flag && (i + int(time_dead_forward * 5)) < xv.size())
		{
			x_time = xv[i];
			flag = 0;

			bool flag_afp = 1;
			int x_time_afp = i;
			int total_num_pulsing = 0;


			//вычисление количества импульсов на отрезке 
			for (int j = i - int(time_dead_forward * 5); j < i + int(time_dead_forward * 5); j++)
			{
				if (yv_der[j] < threshold_der && flag_afp)
				{
					total_num_pulsing++;
					flag_afp = 0;
					x_time_afp = j;
				}

				if (yv_der[j] > threshold_der && (flag_afp == 0) && (xv[j] - xv[x_time_afp]) > time_dead_der)
				{
					flag_afp = 1;
				}
			}

			//cout << "total_num_pulsing " << total_num_pulsing << endl;

			// отбрасывание послеимпульсов
			double amplitude = 5000;
			if (total_num_pulsing == 1)
			{
				for (int j = /*i - int(time_dead_forward * 5)*/ i; j < i + int(time_dead_forward * 5); j++) // нахождение максимальной амплитуды
				{
					if (yv[j] < amplitude)
						amplitude = yv[j];
				}

				TGraph *gr = new TGraph(int(time_dead_forward * 5) + int(time_dead_forward * 5), &xv[i - int(time_dead_forward * 5)], &yv[i - int(time_dead_forward * 5)]);
				TGraph *gr_der = new TGraph(int(time_dead_forward * 5) + int(time_dead_forward * 5), &xv[i - int(time_dead_forward * 5)], &yv_der[i - int(time_dead_forward * 5)]);
				TGraph *gr_der2 = new TGraph(int(time_dead_forward * 5) + int(time_dead_forward * 5), &xv[i - int(time_dead_forward * 5)], &yv_der2[i - int(time_dead_forward * 5)]);

				gr->SetMarkerColor(4);
				gr->SetMarkerStyle(kFullCircle);

				gr_der2->SetLineColor(7);

				for (int i = 0; i < gr_der->GetN(); i++) gr_der->GetY()[i] *= 50;
				for (int i = 0; i < gr_der2->GetN(); i++) gr_der2->GetY()[i] *= 500;

				TMultiGraph *mg = new TMultiGraph();
				mg->Add(gr);
				mg->Add(gr_der);
				mg->Add(gr_der2);



				file_amp_average_mode << abs(amplitude) << endl;

				if (amplitude > threshold_2e && amplitude < threshold_1e && yv[i - int(time_dead_forward * 5)] >(threshold_1e * 0.5)) // проверка, что импульс одноэлектронный
				{

					Monostate::Hlist_reco_time_good.Add(mg);

					num_of_signals++;
					for (int j = -int(time_dead_forward * 5); j < int(time_dead_forward * 5); j++)// запись значений в вектор среднего
					{
						yv_average[j + int(time_dead_forward * 5)] += yv[i + j];
					}
				}
				else
				{
					Monostate::Hlist_reco_time_bad.Add(mg);
				}

			}

		}

		//разрешить искать сигнал, когда пройдет time_dead_forward нс после запуска
		if (yv_der[i] < threshold_der && flag == 0 && (xv[i] - x_time) > time_dead_forward)
		{
			flag = 1;
		}
	}

	//average
	string s_Hlist_good_time = Monostate::dir_name + "Hlist_good_time.root";
	string s_Hlist_bad_time = Monostate::dir_name + "Hlist_bad_time.root";
	
	//average
	TFile ofile_Hlist_good_time(s_Hlist_good_time.c_str(), "RECREATE");
	Monostate::Hlist_reco_time_good.Write();
	ofile_Hlist_good_time.Close();

	TFile ofile_Hlist_bad_time(s_Hlist_bad_time.c_str(), "RECREATE");
	Monostate::Hlist_reco_time_bad.Write();
	ofile_Hlist_bad_time.Close();

      string temp_2 = Monostate::dir_name + "file_average.dat";
      ofstream file_average(temp_2.c_str());
	for (int i = 0; i < 2 * int(time_dead_forward * 5); i++)
	{
		file_average << i*0.2 << "\t" << yv_average[i] / num_of_signals << endl;
	}

	cout << "number_of_pulsing = \t" << num_of_signals << endl;
	//system("pause");
	exit(0);
}

void RootFit::SetAlgorithmParameters()
{
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
	RootFit::time_shit = 20*5; // задать смещение по времени для учета базовой линии (в точках)	
	//-------------------------------------------------------
}

void RootFit::ReserveVectors()
{
	RootFit::xv.reserve(5 * 1000 * 1000);
	RootFit::yv.reserve(5 * 1000 * 1000);
	RootFit::yv_der.reserve(5 * 1000 * 1000);
	//RootFit::yv_der2.reserve(20 * 1000 * 1000);
	RootFit::xverr.reserve(5 * 1000 * 1000);
	RootFit::yverr.reserve(5 * 1000 * 1000);
}
