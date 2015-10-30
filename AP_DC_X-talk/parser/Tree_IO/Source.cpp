//base
#include <iostream>

//root cern
#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h" 
#include "TF1.h"
#include "TRandom.h"
#include "TMultiGraph.h"


using namespace std;

string dir_name = "D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\295K\\70_01V\\";
string tree_name = dir_name + "tree.root";
string graphs_name = dir_name + "graphs.root";

void CreateTree_work()
{
	TTree tree("t1", "Parser tree");

	TMultiGraph *gr = new TMultiGraph();
	TGraphErrors *gr_1 = new TGraphErrors();
	TGraphErrors *gr_2 = new TGraphErrors();

	gr->Add(gr_1);
	gr->Add(gr_2);

	tree.Branch("gr", "TMultiGraph", &gr, 128000, 0);

	TFile f("D:\\Data_work\\tree.root", "recreate");

	//loop to create 5 TMultiGraph
	for (int j = 0; j < 5; j++)
	{

		
		//fill graphs
		for (int i = 0; i < 10; ++i)
		{
			gr_1->SetPoint(i, i + gRandom->Uniform(-.5, .5), gRandom->Uniform(-.5, .5));
			gr_2->SetPoint(i, i + gRandom->Uniform(-.1, .1), gRandom->Uniform(-.1, .1));
		}



		tree.Fill();
		//gr->Write();

	}
	
	tree.Write();
}


void CreateTree_test()
{
	TTree tree("t1", "Parser tree");

	TMultiGraph *gr = new TMultiGraph();
	TGraphErrors *gr_1 = new TGraphErrors();
	TGraphErrors *gr_2 = new TGraphErrors();

	gr->Add(gr_1);
	gr->Add(gr_2);

	tree.Branch("gr", "TMultiGraph", &gr, 128000, 0);

	

	//loop to create 5 TMultiGraph
	for (int j = 0; j < 5; j++)
	{
		TGraphErrors *gr_a = new TGraphErrors();
		TGraphErrors *gr_b = new TGraphErrors();

		//fill graphs
		for (int i = 0; i < 10; ++i)
		{
			gr_a->SetPoint(i, i + gRandom->Uniform(-.5, .5), gRandom->Uniform(-.5, .5));
			gr_b->SetPoint(i, i + gRandom->Uniform(-.1, .1), gRandom->Uniform(-.1, .1));
		}

		*gr_1 = *gr_a; //work
		*gr_2 = *gr_b;

		tree.Fill();

		delete gr_a;
		delete gr_b;

	}

	TFile f("D:\\Data_work\\tree.root", "recreate");
	tree.Write();
}

void ReadTree()
{
	TObjArray Hlist(0);
	Hlist.SetOwner(kTRUE);

	TFile f(tree_name.c_str());
	TTree* t;
	f.GetObject("t1", t);

	TMultiGraph* graph = 0;

	t->SetBranchAddress("gr", &graph);

	for (int i = 0; i < t->GetEntries(); ++i)
	{
		t->GetEntry(i);		
		Hlist.Add(graph->Clone());
	}

	

	TFile ofile_Hlist(graphs_name.c_str(), "RECREATE");
	Hlist.Write();
	ofile_Hlist.Close();
}

void CreateTObjectArray_test()
{

	TObjArray Hlist(0);
	Hlist.SetOwner(kTRUE);

	TMultiGraph *gr = new TMultiGraph();
	TGraphErrors *gr_1 = new TGraphErrors();
	TGraphErrors *gr_2 = new TGraphErrors();

	gr->Add(gr_1);
	gr->Add(gr_2);	
	
	//loop to create 5 TMultiGraph
	for (int j = 0; j < 5; j++)
	{
		TGraphErrors *gr_a = new TGraphErrors();
		TGraphErrors *gr_b = new TGraphErrors();

		//fill graphs
		for (int i = 0; i < 10; ++i)
		{
			gr_a->SetPoint(i, i + gRandom->Uniform(-.5, .5), gRandom->Uniform(-.5, .5));
			gr_b->SetPoint(i, i + gRandom->Uniform(-.1, .1), gRandom->Uniform(-.1, .1));
		}		


		*gr_1 = *gr_a;
		*gr_2 = *gr_b;

		Hlist.Add(gr->Clone());
		delete gr_a;
		delete gr_b;

	}

	TFile f("D:\\Data_work\\TObjArray.root", "recreate");
	Hlist.Write();
	f.Close();
}

int main(int argc, char *argv[])
{
	//CreateTObjectArray_test();
	CreateTree_test();
	//ReadTree();

	system("pause");
	return 0;
}

