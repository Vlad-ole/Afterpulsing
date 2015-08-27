#include "RootFit.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TMath.h"
#include "Monostate.h"

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
	TMultiGraph *mg = new TMultiGraph();
	mg->Add(gr);
	mg->Add(gr_der);
	Hlist.Add(mg);
}




void RootFit::SetDispXY(double x, double y)
{
	//вектора с ошибками
	for (unsigned int j = 0; j < xv.size(); j++)
	{
		xverr.push_back(x);
		yverr.push_back(y);
	}
}




//вспомогательная функция
double RootFit::F(double t, double sigma, double tau)
{
	return TMath::Exp((sigma*sigma - 2 * t*tau) / (2 * tau*tau)) * (1 + TMath::Erf((t - sigma*sigma / tau) / (sigma * sqrt(2))));
}

//функция, которой буду фитировать
Double_t RootFit::fitFunction(Double_t *x, Double_t *par)
{
	double A = par[0];
	double t_0 = par[1];
	double tau_rec_fast = par[2];
	double tau_rise = par[3];
	double V_0 = par[4];

	double sigma = par[5];

	double t = x[0] - t_0;
	double tau_total_fast = (tau_rec_fast * tau_rise) / (tau_rec_fast + tau_rise);


	return -(A / 2) * (F(t, sigma, tau_rec_fast) - F(t, sigma, tau_total_fast)) + V_0;
}

//сумма двух сигналов
double RootFit::fitFunction_2(Double_t *x, Double_t *par)
{
	return fitFunction(x, par) + fitFunction(x, &par[6]);
}

//сумма трех сигналов
double RootFit::fitFunction_3(Double_t *x, Double_t *par)
{
	return fitFunction(x, par) + fitFunction(x, &par[6]) + fitFunction(x, &par[12]);
}