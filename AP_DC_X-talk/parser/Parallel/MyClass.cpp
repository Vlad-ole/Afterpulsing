#include "MyClass.h"
#include "TVirtualFitter.h"

//TF1 *MyClass::fitFcn;

MyClass::MyClass()
{
	fitFcn = new TF1("fitFcn", MyClass::fitFunction, -10, 20, 3);
	const double noise_amp = 300;

	//fill vectors ...
	for (int i = -10; i < 20; i++)
	{
		xv.push_back(i);
		yv.push_back((pow((i - 3.5) * 5, 2.0) + 3) + noise_amp*gRandom->Uniform(-1, 1));

		xverr.push_back(0);
		yverr.push_back(noise_amp);
	}

	gr = new TGraphErrors(xv.size(), &xv[0], &yv[0], &xverr[0], &yverr[0]);
	gr->SetMarkerColor(4);
	gr->SetMarkerStyle(kFullCircle);
}

MyClass::~MyClass()
{
}

Double_t MyClass::fitFunction(Double_t *x, Double_t *par)
{
	return pow((x[0] - par[2])*par[0], 2.0) + par[1];
}

void MyClass::SetParameters()
{
	//set statr parameters and limits
	fitFcn->SetParameter(0, 1);
	fitFcn->SetParLimits(0, 0, 10);

	fitFcn->SetParameter(1, 1);
	fitFcn->SetParLimits(1, -100, 100);

	fitFcn->SetParameter(2, 1);
	fitFcn->SetParLimits(2, -10, 10);
}

void MyClass::DoFit()
{
	gr->Fit("fitFcn", "RQ");
}