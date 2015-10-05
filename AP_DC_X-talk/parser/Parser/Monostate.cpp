#include "Monostate.h"
#include "TFile.h"

Monostate::Monostate()
{
}


Monostate::~Monostate()
{
}

const double Monostate::chi2_per_dof_th = 5;


const int Monostate::rec_lenght = 1000*17000;


const int Monostate::time_pre = (20 / 0.2);
const int Monostate::time_post = (100 / 0.2);

//string Monostate::dir_name = "G:\\";
string Monostate::dir_name = "D:\\Data_work\\tektronix_signal\\295K\\73_90V\\";
//string Monostate::raw_name = dir_name + "raw\\test_signal_pure.txt";
string Monostate::raw_name = dir_name + "raw\\run_2.txt";

TObjArray Monostate::Hlist_test(0);

TObjArray Monostate::Hlist_f1(0);
TObjArray Monostate::Hlist_f1_good(0);
TObjArray Monostate::Hlist_f1_bad(0);


TObjArray Monostate::Hlist_f2(0);
TObjArray Monostate::Hlist_f2_good(0);
TObjArray Monostate::Hlist_f2_bad(0);

TObjArray Monostate::Hlist_f3(0);
TObjArray Monostate::Hlist_f3_good(0);
TObjArray Monostate::Hlist_f3_bad(0);

TObjArray Monostate::Hlist_f4_good(0);
TObjArray Monostate::Hlist_f4_bad(0);

TObjArray Monostate::Hlist_f5_good(0);
TObjArray Monostate::Hlist_f5_bad(0);

TObjArray Monostate::Hlist_f2_bad_new_time(0);
TObjArray Monostate::Hlist_f2_bad_old_time(0);


//average
TObjArray Monostate::Hlist_reco_time_bad(0);
TObjArray Monostate::Hlist_reco_time_good(0);

TObjArray Monostate::Hlist_f3_bad_new_time(0);

ofstream Monostate::amp_chi2_fnc1(dir_name + "amp_chi2_fnc1.dat");
ofstream Monostate::amp_chi2_fnc2(dir_name + "amp_chi2_fnc2.dat");
ofstream Monostate::amp_chi2_fnc3(dir_name + "amp_chi2_fnc3.dat");

ofstream Monostate::time_delta(dir_name + "time_delta.dat");
ofstream Monostate::file_dt(dir_name + "dt.dat");
ofstream Monostate::file_amp(dir_name + "amp.dat");
ofstream Monostate::time_i(dir_name + "time_i.dat");
ofstream Monostate::file_long_dt(dir_name + "long_dt.dat");


void Monostate::SaveHlists()
{
	string s_Hlist_test = dir_name + "Hlist_test.root";
	
	//average
	string s_Hlist_good_time = dir_name + "Hlist_good_time.root";
	string s_Hlist_bad_time = dir_name + "Hlist_bad_time.root";
	
	string s_Hlist_f1 = dir_name + "Hlist_f1.root";
	string s_Hlist_f1_good = dir_name + "Hlist_f1_good.root";
	string s_Hlist_f1_bad = dir_name + "Hlist_f1_bad.root";

	string s_Hlist_f2 = dir_name + "Hlist_f2.root";
	string s_Hlist_f2_good = dir_name + "Hlist_f2_good.root";
	string s_Hlist_f2_bad = dir_name + "Hlist_f2_bad.root";

	string s_Hlist_f3 = dir_name + "Hlist_f3.root";
	string s_Hlist_f3_good = dir_name + "Hlist_f3_good.root";
	string s_Hlist_f3_bad = dir_name + "Hlist_f3_bad.root";

	string s_Hlist_f4_good = dir_name + "Hlist_f4_good.root";
	string s_Hlist_f4_bad = dir_name + "Hlist_f4_bad.root";

	string s_Hlist_f5_good = dir_name + "Hlist_f5_good.root";
	string s_Hlist_f5_bad = dir_name + "Hlist_f5_bad.root";

	string s_Hlist_f2_bad_new_time = dir_name + "Hlist_f2_bad_new_time.root";
	string s_Hlist_f2_bad_old_time = dir_name + "Hlist_f2_bad_old_time.root";

	string s_Hlist_f3_bad_new_time = dir_name + "Hlist_f3_bad_new_time.root";

	// Open a file, save the ntuple and close the file
	
	TFile ofile_Hlist_test(s_Hlist_test.c_str(), "RECREATE");
	Hlist_test.Write();
	ofile_Hlist_test.Close();
	

	//average
	TFile ofile_Hlist_good_time(s_Hlist_good_time.c_str(), "RECREATE");
	Hlist_reco_time_good.Write();
	ofile_Hlist_good_time.Close();

	TFile ofile_Hlist_bad_time(s_Hlist_bad_time.c_str(), "RECREATE");
	Hlist_reco_time_bad.Write();
	ofile_Hlist_bad_time.Close();


	//f1
	TFile ofile_Hlist_f1(s_Hlist_f1.c_str(), "RECREATE");
	Hlist_f1.Write();
	ofile_Hlist_f1.Close();

	TFile ofile_Hlist_f1_good(s_Hlist_f1_good.c_str(), "RECREATE");
	Hlist_f1_good.Write();
	ofile_Hlist_f1_good.Close();
	
	TFile ofile_Hlist_f1_bad(s_Hlist_f1_bad.c_str(), "RECREATE");
	Hlist_f1_bad.Write();
	ofile_Hlist_f1_bad.Close();

	
	//f2
	TFile ofile_Hlist_f2(s_Hlist_f2.c_str(), "RECREATE");
	Hlist_f2.Write();
	ofile_Hlist_f2.Close();

	TFile ofile_Hlist_f2_good(s_Hlist_f2_good.c_str(), "RECREATE");
	Hlist_f2_good.Write();
	ofile_Hlist_f2_good.Close();
	
	TFile ofile_Hlist_f2_bad(s_Hlist_f2_bad.c_str(), "RECREATE");
	Hlist_f2_bad.Write();
	ofile_Hlist_f2_bad.Close();

	
	//f3
	TFile ofile_Hlist_f3(s_Hlist_f3.c_str(), "RECREATE");
	Hlist_f3.Write();
	ofile_Hlist_f3.Close();

	TFile ofile_Hlist_f3_good(s_Hlist_f3_good.c_str(), "RECREATE");
	Hlist_f3_good.Write();
	ofile_Hlist_f3_good.Close();
	
	TFile ofile_Hlist_f3_bad_new_time(s_Hlist_f3_bad_new_time.c_str(), "RECREATE");
	Hlist_f3_bad_new_time.Write();
	ofile_Hlist_f3_bad_new_time.Close();

	TFile ofile_Hlist_f3_bad(s_Hlist_f3_bad.c_str(), "RECREATE");
	Hlist_f3_bad.Write();
	ofile_Hlist_f3_bad.Close();

	//f4
	TFile ofile_Hlist_f4_good(s_Hlist_f4_good.c_str(), "RECREATE");
	Hlist_f4_good.Write();
	ofile_Hlist_f4_good.Close();

	TFile ofile_Hlist_f4_bad(s_Hlist_f4_bad.c_str(), "RECREATE");
	Hlist_f4_bad.Write();
	ofile_Hlist_f4_bad.Close();
	
	//f5
	TFile ofile_Hlist_f5_good(s_Hlist_f5_good.c_str(), "RECREATE");
	Hlist_f5_good.Write();
	ofile_Hlist_f5_good.Close();

	TFile ofile_Hlist_f5_bad(s_Hlist_f5_bad.c_str(), "RECREATE");
	Hlist_f5_bad.Write();
	ofile_Hlist_f5_bad.Close();

	//
	TFile oHlist_Hlist_f2_bad_new_time(s_Hlist_f2_bad_new_time.c_str(), "RECREATE");
	Hlist_f2_bad_new_time.Write();
	oHlist_Hlist_f2_bad_new_time.Close();

	TFile oHlist_Hlist_f2_bad_old_time(s_Hlist_f2_bad_old_time.c_str(), "RECREATE");
	Hlist_f2_bad_old_time.Write();
	oHlist_Hlist_f2_bad_old_time.Close();


	

	//string string_time_i = Monostate::dir_name + "time_i.dat";
	//FILE *f2 = fopen(string_time_i.c_str(), "r");

	//double x_old, x, y;
	//bool flag = 0;
	//while (!feof(f2))
	//{
	//	fscanf(f2, "%lf %lf\n", &x, &y);
	//	
	//	//if (y > 0.02) // отбросить события, вызванные шумами
	//	{
	//		if (flag)
	//		{
	//			time_delta << x - x_old << "\t" << y << endl;
	//			file_dt << x - x_old << endl;
	//			file_amp << y << endl;
	//		}
	//		x_old = x;

	//		flag = 1;
	//	}

	//}

}
