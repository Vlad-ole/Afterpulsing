#include <iostream>
#include <vector>

#include "TRandom.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include  "TF1.h"

//#include "MinimizerOptions.h"
#include "Math/MinimizerOptions.h"

using namespace std;

Double_t fitFunction(Double_t *x, Double_t *par)
{
	return pow((x[0] - par[2])*par[0], 2.0) + par[1];
}

int main()
{
	double noise_amp = 300;

	vector<double> xv;
	vector<double> yv;

	vector<double> xverr;
	vector<double> yverr;


	//fill vectors ...
	for (int i = -10; i < 20; i++)
	{
		xv.push_back(i);
		yv.push_back((pow((i - 3.5) * 5, 2.0) + 3) + noise_amp*gRandom->Uniform(-1, 1));

		xverr.push_back(0);
		yverr.push_back(noise_amp);
	}

	TGraphErrors * gr = new TGraphErrors(xv.size(), &xv[0], &yv[0], &xverr[0], &yverr[0]);
	TF1 *fitFcn = new TF1("fitFcn", fitFunction, -10, 20, 3);

	gr->SetMarkerColor(4);
	gr->SetMarkerStyle(kFullCircle);

	fitFcn->SetParameter(0, 1);
	fitFcn->SetParLimits(0, 0, 10);

	fitFcn->SetParameter(1, 1);
	fitFcn->SetParLimits(1, -100, 100);

	fitFcn->SetParameter(2, 1);
	fitFcn->SetParLimits(2, -10, 10);

	
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit", "Simplex"); // change minimizer method
	gr->Fit("fitFcn", "R");

	system("pause");
	return 0;
}