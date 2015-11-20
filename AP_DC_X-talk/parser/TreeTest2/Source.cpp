//base
#include <iostream>
#include <sstream>

//root cern
#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h" 
#include "TF1.h"
#include "TRandom.h"
#include "TMultiGraph.h"


using namespace std;

int main(int argc, char *argv[])
{
	
	//TFile f("D:\\Data_work\\tree_test\\tree.root", "recreate");

	string dir_name = "D:\\Data_work\\tree_test\\tree_";




	for (int i = 0; i < 10; i++)
	{
		TTree tree("t1", "Parser tree");
		double var;
		tree.Branch("var", &var, "var/D");

		ostringstream file_tree_oss;
		file_tree_oss << dir_name << i << ".root";
		TFile f_tree(file_tree_oss.str().c_str(), "RECREATE");

		var = 10;
		tree.Fill();
		tree.Write(file_tree_oss.str().c_str());
	}


	

	
	system("pause");
	return 0;
}