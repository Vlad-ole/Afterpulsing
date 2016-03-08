#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Double_t fitFunction(Double_t *x, Double_t *par) 
{ 
	double T = x[0];
	double V = x[1];
	
	double a = par[1];
	double b = par[0];
	double c = par[2];
	
	//return par[0] * pow(x[0], 1.5) * TMath::Exp(- par[1] / x[0] ) * TMath::Exp( par[2] * x[1] );  // exp
	return a*V + b*T + c;
}

void gen_data(int N)
{
	ofstream file("D:\\Data_work\\test_V_BD.dat");
	
	double a = 1.294E5;
	double b = -5.320E3;
	double c = -7.404E6;
	
	const double conv = 1E7 / 3710.0;
	for (double i = 265; i < 295; i+= (295.0 - 265.0) / N)
	{
		for(double j = 70.5; j < 73; j+=0.2 )
		{
			double T = i;// + gRandom->Uniform(-0.5, 0.5);
			double V = j;// + gRandom->Uniform(-0.01, 0.01);
			double G = (a * V + b * T + c);// + gRandom->Gaus(0, 20 * conv); 

			file << T << "\t" << V << "\t" << G / conv << endl;
		}
	}
	
}

void fit_V_bd_surf(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
	std::vector<double> zv;
	
	std::vector<double> xerrv;
	std::vector<double> yerrv;
	std::vector<double> zerrv;
	
	Double_t x, y, z, z_err;
	FILE *f = fopen(name,"r");
	
	const double conv = 1E7 / 3710.0;
	
	
	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf %lf %lf\n", &x, &y, &z, &z_err);
		xv.push_back(x); 
		yv.push_back(y);
		zv.push_back(z * conv);
		zerrv.push_back(/*z_err * conv*/0.00001);		
	}	

			for (int i = 0; i < xv.size(); i++)
			{
				xerrv.push_back(0/*0.29*/);
				yerrv.push_back(0/*0.01*/);
				//zerrv.push_back(0.5 * conv);
			}
			
			
			TGraph2DErrors * gr = new TGraph2DErrors(xv.size(), &xv[0], &yv[0], &zv[0], &xerrv[0], &yerrv[0], &zerrv[0]);
			TF2 *fitFcn = new TF2("fitFcn", fitFunction, 260, 300, 0, 100, 3);
	
			
			//fitFcn->SetParameter(0, 4.80590e+001); 
			//fitFcn->SetParameter(1, -1.97756e+000); 
			//fitFcn->SetParameter(2, -2.74847e+003); 

			
			//fitFcn->SetParLimits(0, 1, 100);
			//fitFcn->SetParLimits(1, -10, 0);
			//fitFcn->SetParLimits(2, -10000, -100);
			
			
			
			gr->Fit("fitFcn", "R");	
			gr->SetMarkerColor(4);
			gr->SetMarkerStyle(kFullCircle);
			gr->Draw("surf1");	
			
			cout <<  fitFcn->GetChisquare() / fitFcn->GetNDF() << endl;
			
			cout << "par 0 (b) relative error \t " << fabs(fitFcn->GetParError(0) /  fitFcn->GetParameter(0))  << endl;
			cout << "par 1 (a) relative error \t " << fabs(fitFcn->GetParError(1) /  fitFcn->GetParameter(1))  << endl;
			cout << "par 2 (c) relative error \t " << fabs(fitFcn->GetParError(2) /  fitFcn->GetParameter(2))  << endl;
			
			cout << "V_0 = " << - (fitFcn->GetParameter(2) / fitFcn->GetParameter(1)) << " +- " 
			<< sqrt(pow(fitFcn->GetParError(2) /  fitFcn->GetParameter(2) , 2.0) + pow(fitFcn->GetParError(1) /  fitFcn->GetParameter(1) , 2.0)) * (-1) * (fitFcn->GetParameter(2) / fitFcn->GetParameter(1)) <<  endl;

			cout << "dV / dt = " << -(fitFcn->GetParameter(0) / fitFcn->GetParameter(1)) << " +- "
			<< sqrt(pow(fitFcn->GetParError(0) /  fitFcn->GetParameter(0) , 2.0) + pow(fitFcn->GetParError(1) /  fitFcn->GetParameter(1) , 2.0)) * (-1) * (fitFcn->GetParameter(0) / fitFcn->GetParameter(1)) <<  endl;
			
			//gSystem->Sleep(3000);			
			
			c1->Update();
							
			xv.clear();
			yv.clear();	
	
	
}