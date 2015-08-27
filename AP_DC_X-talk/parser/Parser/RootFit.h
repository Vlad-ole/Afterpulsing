#pragma once
#include <vector>
#include "TObjArray.h"
#include  "TF1.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
using namespace std;

class RootFit
{
public:
	RootFit(short int number_of_function);
	~RootFit();	

	void SetParameters(double time_first, double time_second = 0, double time_third = 0);
	void DoFit();
	double GetChi2PerDof();
	double GetAmplitude();

	void Print_dt_amp();
	void SaveGraphs(TObjArray &Hlist);	

	static vector<double> xv;
	static vector<double> yv;
	static vector<double> xverr;
	static vector<double> yverr;

	static vector<double> yv_der;

	static void CalculateDer(int type, int points);
	static void FindStartStop(double threshold_der, double threshold_amp);
	static void CalculateStartParameters(double time_dead, double threshold_der); //вычислить стартовые времена t_i. Необходимо задать мертвое время в нс.

	static vector<int> time_start;
	static vector<int> time_finish;
	static vector<int> time_front;

	
	static void SetDispXY(double x = 0, double y = 0);
	
	static int time_shit;

	static int current_signal;

private:
	
	static int time_start_index;
	static int time_finish_index;


	vector<double> time_i;
	vector<double> amp_i;
		
	TF1 *fitFcn;
	TGraphErrors *gr;
	TGraph *gr_der;
	TMultiGraph *mg;

	short int number_of_functions;

	
	
	static Double_t fitFunction(Double_t *x, Double_t *par);
	static double F(double t, double sigma, double tau);
	
	static double fitFunction_2(Double_t *x, Double_t *par);
	static double fitFunction_3(Double_t *x, Double_t *par);
	
};

