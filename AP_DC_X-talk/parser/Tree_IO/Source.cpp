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

void CreateTree()
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

		//gr->Dump();
		//gr->Clear();
		//gr_1->Clear();
		//gr_2->Clear();
		
		gr->Write();

	}

	

	
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

	TBranch *branch = t->GetBranch("gr");
	branch->SetAddress(&graph);
	//branch->SetAutoDelete(kTRUE);
	//t->SetBranchAddress("gr", &graph);

	for (int i = 0; i < t->GetEntries(); ++i)
	{
		t->GetEntry(i);		
		//graph->Dump();
		//Hlist.Add(new TMultiGraph(*graph)); // error
		
		//TMultiGraph* graph_copy = graph->Clone();
		//graph = new TMultiGraph();
		Hlist.Add(graph->Clone());
	}

	

	TFile ofile_Hlist(graphs_name.c_str(), "RECREATE");
	Hlist.Write();
	ofile_Hlist.Close();
}



int main(int argc, char *argv[])
{
	//CreateTree();
	ReadTree();

	system("pause");
	return 0;
}

