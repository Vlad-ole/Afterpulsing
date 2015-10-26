#pragma once
#include <string>
#include "TObjArray.h"
#include <iostream>
#include <fstream>
#include "TNtuple.h"
using namespace std;

class Monostate
{
public:
	Monostate();
	~Monostate();

	static void SaveHlists(); //записать графики в файлы

	static string dir_name;
	static string raw_name;
	static string der_name;
	static string der2_name;
	
	static TObjArray Hlist_test;
	static TObjArray Hlist_test_2;
	static TObjArray Hlist_chi2_amp_cut_2;

	static TObjArray Hlist_f1;
	static TObjArray Hlist_f1_good;
	static TObjArray Hlist_f1_bad;


	static TObjArray Hlist_f2;
	static TObjArray Hlist_f2_good;
	static TObjArray Hlist_f2_bad;
	
	static TObjArray Hlist_f3;
	static TObjArray Hlist_f3_good;
	static TObjArray Hlist_f3_bad;

	static TObjArray Hlist_f4_good;
	static TObjArray Hlist_f4_bad;

	static TObjArray Hlist_f5_good;
	static TObjArray Hlist_f5_bad;

	static TObjArray Hlist_f2_bad_new_time;
	static TObjArray Hlist_f2_bad_old_time;

	static TObjArray Hlist_f3_bad_new_time;

	static TObjArray Hlist_reco_time_good;
	static TObjArray Hlist_reco_time_bad;

	static ofstream amp_chi2_fnc1;
	static ofstream amp_chi2_fnc2;
	static ofstream amp_chi2_fnc3;

	static ofstream amp_chi2_fnc1_all_signals;
	static ofstream amp_chi2_fnc2_all_signals;
	static ofstream amp_chi2_fnc3_all_signals;

	static ofstream amp_chi2_dt_fnc2_all_signals;
	
	static ofstream time_i;
	static ofstream time_delta;
	static ofstream file_dt;
	static ofstream file_amp;
	static ofstream file_long_dt;

	static TNtuple *ntuple_fit;
	
	const static double chi2_per_dof_th;

};

