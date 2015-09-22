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
	double noise_amp = 0;

	vector<double> xv;
	vector<double> yv;

	vector<double> xverr;
	vector<double> yverr;

	double a = 5;
	double y_0 = 3;
	double x_0 = 100000.3; //error!
	//double x_0 = 99999.3; // work well

	//fill vectors ...
	for (int i = -20 + x_0; i < x_0 + 20; i++)
	{
		xv.push_back(i);
		yv.push_back((pow((i - x_0) * a, 2.0) + y_0) + noise_amp*gRandom->Uniform(-1, 1));

		xverr.push_back(0);
		yverr.push_back(noise_amp);
	}

	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	TGraphErrors * gr = new TGraphErrors(xv.size(), &xv[0], &yv[0], &xverr[0], &yverr[0]);
	TF1 *fitFcn = new TF1("fitFcn", fitFunction, -20 + x_0, x_0 + 20, 3);

	gr->SetMarkerColor(4);
	gr->SetMarkerStyle(kFullCircle);

	fitFcn->SetParameter(0, 1);
	fitFcn->SetParLimits(0, 0, 10);

	fitFcn->SetParameter(1, 1);
	fitFcn->SetParLimits(1, -100, 100);

	fitFcn->SetParameter(2, x_0);
	fitFcn->SetParLimits(2, -20 + x_0, 20 + x_0);

	
	ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit", "Migrad"); // change minimizer method
	gr->Fit("fitFcn", "R");
	gr->Draw("AP");
	
	cout << setprecision(17) <<"Parameter(2) is " << fitFcn->GetParameter(2) << endl;

	return 0;
}