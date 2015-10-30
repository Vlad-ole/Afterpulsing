#pragma once

//root cern
#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h" 
#include "TF1.h"
#include "TRandom.h"
#include "TMultiGraph.h"

using namespace std;

class JustClass
{
public:
	JustClass();
	~JustClass();

	TGraph *gr_der;
	TGraph *gr_der2;
	TMultiGraph *mg;

	static vector<double> xv;
	static vector<double> yv;

	void FillData();
};

