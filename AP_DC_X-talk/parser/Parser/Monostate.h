#pragma once
#include <string>
#include "TObjArray.h"
#include <iostream>
#include <fstream>
using namespace std;

class Monostate
{
public:
	Monostate();
	~Monostate();

	static void SaveHlists(); //�������� ������� � �����

	static string dir_name;
	static string raw_name;

	static TObjArray Hlist_f1;
	static TObjArray Hlist_f1_bad;
	static TObjArray Hlist_f2;
	static TObjArray Hlist_f2_bad;
	static TObjArray Hlist_f3;
	static TObjArray Hlist_f3_bad;

	static ofstream amp_chi2_fnc1;
	static ofstream amp_chi2_fnc2;
	static ofstream amp_chi2_fnc3;
	
	static ofstream time_i;

	const static int num_of_param;
	const static int step;

	const static int rec_lenght;

	const static double disp;

	const static double threshold_der;
	const static double threshold;

	const static int time_pre;
	const static int time_post;

};

