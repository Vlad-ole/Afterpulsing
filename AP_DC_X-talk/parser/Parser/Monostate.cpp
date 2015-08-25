#include "Monostate.h"
#include "TFile.h"

Monostate::Monostate()
{
}


Monostate::~Monostate()
{
}

const int Monostate::num_of_param = 6;
const int Monostate::step = 20;

const int Monostate::rec_lenght = 100000;

const double Monostate::disp = 0.00113151;

const double Monostate::threshold_der = -0.006;
const double Monostate::threshold = -0.001;

const int Monostate::time_pre = (20 / 0.2);
const int Monostate::time_post = (100 / 0.2);

string Monostate::dir_name = "D:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\";
string Monostate::raw_name = dir_name + "raw\\20M.txt";

TObjArray Monostate::Hlist_f1(0);
TObjArray Monostate::Hlist_f1_bad(0);
TObjArray Monostate::Hlist_f2(0);
TObjArray Monostate::Hlist_f2_bad(0);
TObjArray Monostate::Hlist_f3(0);
TObjArray Monostate::Hlist_f3_bad(0);

ofstream Monostate::amp_chi2_fnc1(dir_name + "amp_chi2_fnc1.dat");
ofstream Monostate::amp_chi2_fnc2(dir_name + "amp_chi2_fnc2.dat");
ofstream Monostate::amp_chi2_fnc3(dir_name + "amp_chi2_fnc3.dat");

ofstream Monostate::time_i(dir_name + "time_i.dat");

void Monostate::SaveHlists()
{
	string s_Hlist_f1 = dir_name + "Hlist_f1.root";
	string s_Hlist_f1_bad = dir_name + "Hlist_f1_bad.root";
	string s_Hlist_f2 = dir_name + "Hlist_f2.root";
	string s_Hlist_f2_bad = dir_name + "Hlist_f2_bad.root";
	string s_Hlist_f3 = dir_name + "Hlist_f3.root";
	string s_Hlist_f3_bad = dir_name + "Hlist_f3_bad.root";

	// Open a file, save the ntuple and close the file
	TFile ofile_Hlist_f1(s_Hlist_f1.c_str(), "RECREATE");
	Hlist_f1.Write();
	ofile_Hlist_f1.Close();

	TFile ofile_Hlist_f1_bad(s_Hlist_f1_bad.c_str(), "RECREATE");
	Hlist_f1_bad.Write();
	ofile_Hlist_f1_bad.Close();

	TFile ofile_Hlist_f2(s_Hlist_f2.c_str(), "RECREATE");
	Hlist_f2.Write();
	ofile_Hlist_f2.Close();

	TFile ofile_Hlist_f2_bad(s_Hlist_f2_bad.c_str(), "RECREATE");
	Hlist_f2_bad.Write();
	ofile_Hlist_f2_bad.Close();

	TFile ofile_Hlist_f3(s_Hlist_f3.c_str(), "RECREATE");
	Hlist_f3.Write();
	ofile_Hlist_f3.Close();

	TFile ofile_Hlist_f3_bad(s_Hlist_f3_bad.c_str(), "RECREATE");
	Hlist_f3_bad.Write();
	ofile_Hlist_f3_bad.Close();
}
