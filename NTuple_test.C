#include "Riostream.h"

void NTuple_create()
{
	
	TNtuple *ntuple = new TNtuple("ntuple","data from ascii file","x:y:z");
	for(int i = 0; i < 100; i++)
	{
		ntuple->Fill(i, sqrt(i), pow(i, 1.5));
	}
	
	ntuple->Draw("x:y");
	
	TFile *f = new TFile("D:\\Data_work\\basic.root","RECREATE");
	ntuple->Write();
	f->Close();
}

void ReadNTuple()
{
	//TFile *f = new TFile("D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\295K\\70_01V\\results\\ntuple_01_10.root");
	//TNtuple *my_ntuple = (TNtuple*)f->GetObjectChecked("ntuple", "TNtuple");
	
	TChain *chain = new TChain("ntuple"); // name of the tree is the argument
	chain->Add("D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\295K\\70_01V\\results\\ntuple_01_10.root");
	chain->Add("D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\295K\\70_01V\\results\\ntuple_11_20.root");
	
	// a_1:chi_1:a_2:b_2:chi_2:dt_2_ab - ntuple format
	chain->SetMarkerStyle(4);
	
	TCut fit_1 = "chi_1 < 4";
	TCut A1 = "a_1 > 0.1 && a_1 < 0.3";
	TCut B1 = "a_1 > 0.3 && a_1 < 0.5";
	
	TCut fit_2 = "chi_1 > 4 && chi_2 < 4";
	TCut A2 = "(a_2 + b_2) > 0.15 && (a_2 + b_2) < 0.25";
	TCut B2_tail = "(a_2 + b_2) > 0.25 && (a_2 + b_2) < 0.38";
	TCut B2_circle = "(a_2 + b_2) > 0.38 && (a_2 + b_2) < 0.46";
	TCut C2 = "(a_2 + b_2) > 0.46 && (a_2 + b_2) < 0.7";
		
	chain->Draw("chi_2:(a_2 + b_2)", fit_1 && A1);
	//my_ntuple->Draw("chi_2:(a_2 + b_2)", fit_1 && A2);
	
	//f->Close();
	
	//TChain chain("T");   // name of the tree is the argument
	//chain.Add("file1.root");
}

Double_t fitFunction(Double_t *x, Double_t *par)
{
   return par[0] * pow( (x[0] - par[2]) , 2.0) + par[1];
}


void CreateTree()
{
	TFile f("D:\\Data_work\\tree.root","recreate");
	TTree t1("t1","a simple Tree with simple variables");
	
	Double_t x0, y0, amp;
	
	//create vectors
	vector<double> xv;
	vector<double> yv;
	vector<double> xverr;
	vector<double> yverr;
	
	//loop to create several graphs
	for(int j = 0; j < 10; j++)
	{	
		//fill vectors		
		for (Int_t i= -10; i<= 10; i++) 
		{
			xv.push_back(i);
			yv.push_back(0.2 * i*i + gRandom->Gaus(0, 1));
			xverr.push_back(0);
			yverr.push_back(1);
		}
		
		TGraphErrors *gr = new TGraphErrors(xv.size(), &xv[0], &yv[0], &xverr[0], &yverr[0]);
		gr->SetMarkerColor(4);
		gr->SetMarkerStyle(kFullCircle);
		
		TF1 *fitFcn = new TF1("fitFcn", fitFunction, -10, 10, 3);
		gr->Fit("fitFcn", "R");
		
		x0 = fitFcn->GetParameter(2);
		y0 = fitFcn->GetParameter(1);
		amp = fitFcn->GetParameter(0);
		
		//cout << "x0 = " << x0 << " y0 = " << y0 << " amp = " << amp << endl;
		gr->Draw("AP");
		
		t1.Branch("x0",&x0,"x0/D");
		t1.Branch("y0",&y0,"y0/D");
		t1.Branch("amp",&amp,"amp/D");
		t1.Branch("gr","TGraphErrors",&gr, 128000, 0);
		
		t1.Fill();
		
		xv.clear();
		yv.clear();
		xverr.clear();
		yverr.clear();
	}
	
	
	t1.Write();
}