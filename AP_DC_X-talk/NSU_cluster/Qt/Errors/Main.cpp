//base
#include <iostream>
#include <sstream>

//root cern
#include "TTree.h"
#include "TFile.h"
#include "TChain.h"
#include "TApplication.h"

using namespace std;

int main()
{
    TApplication a("a", 0, 0); // just to make sure that the autoloading of ROOT libraries works
    TTree tree("t1", "Parser");

    Double_t a_1;
    tree.Branch("a_1", &a_1, "a_1/D");

    a_1 = 16.0076;
    tree.Fill();

    TFile f_tree("/mnt/scratch/ws/vpoleynikov/201601261856ws1/MPPC_S10362-11-100C/285K/69_65V/trees/run_0.root", "RECREATE");
    tree.Write();


    /*
    string dir_name = "/mnt/scratch/ws/vpoleynikov/201601261856ws1/MPPC_S10362-11-100C/285K/69_65V/";
    TChain chain("t1");   // name of the tree is the argument
    for(int i = 1; i <= 1; i++)
    {
            ostringstream file_tree_oss;
            file_tree_oss << dir_name << "trees/run_" << i << ".root";
            chain.Add(file_tree_oss.str().c_str());
    }

    Double_t a_1;
    chain.SetBranchAddress("a_1", &a_1);
    for (int i = 0; i < chain.GetEntries(); ++i)
    {
        chain.GetEntry(i);
        cout << a_1 << endl;
    }
    */


    return 0;
}

