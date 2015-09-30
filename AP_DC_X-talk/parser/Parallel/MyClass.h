#pragma once
#include <vector>
#include "TRandom.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "Math/MinimizerOptions.h"

using namespace std;

class MyClass
{
public:
	MyClass();
	~MyClass();

	void DoFit();
	void SetParameters();

private:
	vector<double> xv;
	vector<double> yv;

	vector<double> xverr;
	vector<double> yverr;

	static TF1 *fitFcn;
	static Double_t fitFunction(Double_t *x, Double_t *par);
	TGraphErrors * gr;

};
