//#pragma ones

#ifndef _RootFit_
#define _RootFit_


#include <vector>
#include "TObjArray.h"
#include  "TF1.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"

using namespace std;

class RootFit
{
public:
	RootFit();
	RootFit(short int number_of_function);
	~RootFit();	

	void SetParameters();
	void SetParametersTwoComp(const double A_start, const double A_limit_l, const double A_limit_h);
	
	void SetParametersTwoComp_fit1(const double * const A_start, const double * const A_limit_l, const double * const A_limit_h);
	void SetParametersTwoComp_fit2(const double * const A_start, const double * const A_limit_l, const double * const A_limit_h);
	void SetParametersTwoComp_fit3(const double * const A_start, const double * const A_limit_l, const double * const A_limit_h);

	void SetParametersOneComp_fit1(const double * const A_start, const double * const A_limit_l, const double * const A_limit_h);
	void SetParametersOneComp_fit2(const double * const A_start, const double * const A_limit_l, const double * const A_limit_h);

	void DoFit();
	double GetChi2PerDof();
	double GetAmplitude();

	//void Print_dt_amp();
	void SaveGraphs(TObjArray &Hlist);	
	//void SaveAllGraphs();

	static void ReadFiles(const bool ReadDerivative, const int file_run, const double part_or_file);
	static void CalculateDer(int type, int points);
	static void CalculateStaircases_der(double time_dead);
	static void CalculateStaircases_amp(double time_dead);
	static void FindStartStop(double time_dead_signal_noise, double time_dead_forward);
	static void CalculateStartParameters(double time_dead); 
	static void CalculateNumOfSignals(double time_dead);//найти число сигналов на участке по 2-й производной. Необходимо задать мертвое время в нс.
	static void CreateFrontGraph();
	static void SetDispXY(const double x = 0, const double y = 0);
	static void CalculateAverageSignal(double time_dead);
	static void SetAlgorithmParameters();


	short int number_of_functions;

	static bool only_1e;
	static bool RecoveryTimeTwoComponents;
	static bool SaveGraphsBool;
	static bool ReadDerivative;
	
	static vector<double> xv;
	static vector<double> yv;	

	static vector<double> yv_der2;
	static vector<double> yv_der;
	static vector<double> yv_s;	

	static vector<int> time_start;
	static vector<int> time_finish;
	static vector<int> time_front;	
	
	static int time_shit;	
	static double threshold_der;
	static double threshold_der2;
	static double threshold_amp;
	static double threshold_amp_start;

	static double threshold_1e_low;
	static double threshold_1e_high;
	
	static double threshold_1e_A_low;
	static double threshold_1e_A_high;
	
	static int current_signal;
	
	//static bool PreviousIsSingle;
	static bool previousIs1e;
	static double previous_time;
	//static double temp_time_i;

//private:
	
	static double fitFunction_nobaseline(double *x, double *par);
	static double fitFunction_nobaseline_fast_slow(double *x, double *par);
	static double fitFunction_nobaseline_exp(double *x, double *par);

	static Double_t fitFunction(Double_t *x, Double_t *par);
	static double F(double t, double sigma, double tau);	
	static double fitFunction_2(Double_t *x, Double_t *par);
	static double fitFunction_3(Double_t *x, Double_t *par);
	static double fitFunction_4(Double_t *x, Double_t *par);
	static double fitFunction_5(Double_t *x, Double_t *par);

	static Double_t fitFunctionTwoComp(Double_t *x, Double_t *par);
	static double fitFunction_2_TwoComp(Double_t *x, Double_t *par);
	static double fitFunction_3_TwoComp(Double_t *x, Double_t *par);
	static double fitFunction_4_TwoComp(Double_t *x, Double_t *par);
	static double fitFunction_5_TwoComp(Double_t *x, Double_t *par);

	static Double_t fitFunctionOneComp(Double_t *x, Double_t *par);
	static double fitFunction_2_OneComp(Double_t *x, Double_t *par);
	
	static void CalculateFilterCoeff(int points);
	static void ReserveVectors();
	
	static int time_start_index;
	static int time_finish_index;

	static vector<double> yv_front;
	static vector<double> xv_front;

	vector<double> time_i;
	vector<double> amp_i;
		
	TF1 *fitFcn;
	TGraphErrors *gr;
	TGraph *gr_der;
	TGraph *gr_der2;
	TGraph *gr_front;
	TMultiGraph *mg;

//	short int number_of_functions;

	static vector<double> xverr;
	static vector<double> yverr;
	
	
	static vector<double> C_i_s;
	static vector<double> C_i_der;
	static vector<double> C_i_der2;
	
};

#endif // _RootFit_

