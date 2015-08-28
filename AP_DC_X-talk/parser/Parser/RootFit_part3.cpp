#include "RootFit.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TMath.h"
#include "Monostate.h"

vector<double> RootFit::xverr;
vector<double> RootFit::yverr;
vector<double> RootFit::yv_front;
vector<double> RootFit::xv_front;

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
	for (int i = 0; i<gr_der->GetN(); i++) gr_der->GetY()[i] *= 5;
	
	gr_front->SetMarkerColor(6);
	gr_front->SetMarkerStyle(kFullCircle);
	gr_front->SetMarkerSize(1);
	
	TMultiGraph *mg = new TMultiGraph();
	mg->Add(gr);

	mg->Add(gr_front);

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

void  RootFit::CreateFrontGraph()
{
		
	//cout << xv[time_start_index] << "\t" << xv[time_finish_index] << "\t" << xv.size() << endl;

	xv_front.clear();
	yv_front.clear();

	for (int i = time_start_index; i < time_finish_index; i++)
	{
		for (int j = 0; j < time_front.size(); j++)
		{
			if (i == time_front[j])
			{
				yv_front.push_back(yv[i]);
			}
			else
			{
				yv_front.push_back(0);
			}
		}
		
		xv_front.push_back(xv[i]);

		//cout << xv[i] << "\t" << endl;
	}


	/*for (int i = 0; i < xv_front.size(); i++)
	{
		cout << xv_front[i] << "\t" << yv_front[i] << endl;
	}

	system("pause");*/

	//cout << xv_front.size() << "\t" << yv_front.size() << endl;
}