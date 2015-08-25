#pragma once
#include <vector>
#include "TObjArray.h"
#include  "TF1.h"
#include "TGraphErrors.h"
using namespace std;

class RootFit
{
public:
	RootFit(int time_start_index, int time_finish_index, vector<double>& xv, vector<double>& yv, vector<double>& xverr, vector<double>& yverr, short int number_of_function);
	~RootFit();	

	void SetParameters(double time_start_ns, double time_finish_ns, vector<double>& time_front_ns);
	void DoFit();
	double GetChi2PerDof();
	double GetAmplitude();

	void Print_dt_amp();
	void SaveGraphs(TObjArray &Hlist);

	

	
private:
	vector<double> xv;
	vector<double> yv;
	vector<double> xverr;
	vector<double> yverr;

	vector<double> time_i;
	vector<double> amp_i;

	vector<int> time_front;
	
	TF1 *fitFcn;
	TGraphErrors *gr;

	int time_start_index;
	int time_finish_index;

	short int number_of_functions;
	
	static Double_t fitFunction(Double_t *x, Double_t *par);
	static double F(double t, double sigma, double tau);
	
	static double fitFunction_2(Double_t *x, Double_t *par);
	static double fitFunction_3(Double_t *x, Double_t *par);

	
	
};

