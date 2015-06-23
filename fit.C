Double_t fitFunction(Double_t *x,Double_t *par) 
{ 
	Double_t arg = 0; 
	if (par[2] != 0) arg = (x[0] - par[1])/par[2]; 
	Double_t fitval = par[0]*TMath::Exp(-0.5*arg*arg); 
	return fitval; 
}

Double_t fitFunction_summ(Double_t *x, Double_t *par) 
{ 
	return fitFunction(x, par) + fitFunction(x, &par[3]) + fitFunction(x, &par[6]); 
}

void fit_gauss_(char name[])
{
	TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	c1->SetGrid();
	
	std::vector<double> xv;
	std::vector<double> yv;
		
	Double_t x, y;
	FILE *f = fopen(name,"r");
	//xv.clear();
	//yv.clear();
	while (!feof(f))
	{ 
		fscanf(f,"%lf %lf\n", &x, &y);
		xv.push_back(x);
		yv.push_back(y);
	}
			
	TGraph * gr = new TGraph(xv.size(), &xv[0], &yv[0]);
	
	
	TF1 *fitFcn = new TF1("fitFcn", fitFunction_summ, -1000, 1000, 9);
	
	fitFcn->SetParLimits(0, 10000, 15000); 
	fitFcn->SetParLimits(1, -10, 10); 
	fitFcn->SetParLimits(2, 0, 200); 
	
	fitFcn->SetParLimits(3, 100, 200); 
	fitFcn->SetParLimits(4, 480, 520); 
	fitFcn->SetParLimits(5, 50, 150); 
	
	fitFcn->SetParLimits(6, 10, 30); 
	fitFcn->SetParLimits(7, 800, 1000); 
	fitFcn->SetParLimits(8, 50, 150);
	
	
	fitFcn->SetParameter(0, 100);
	fitFcn->SetParameter(1, 0);
	fitFcn->SetParameter(2, 12000);
	
	fitFcn->SetParameter(3, 153.622);
	fitFcn->SetParameter(4, 502.345);
	fitFcn->SetParameter(5, 86.9206);
	
	
	
	gr->Fit("fitFcn", "R");
	
	
	gr->SetMarkerColor(4);
	gr->SetMarkerStyle(kFullCircle);
	gr->Draw("AP");	
	
		
	/*
	double par[3] = {6675.1, -0.215767, 12.6479};
	
	for(int i=0; i<xv.size(); i++)
	{
		yv[i] -= fitf(xv[i], par); 
	}
	
	TGraph * gr2 = new TGraph(xv.size(), &xv[0], &yv[0]);
	gr2->SetMarkerColor(5);
	gr2->SetMarkerStyle(kFullCircle);
	gr2->Draw();
	*/	
		
	c1->Update();
}