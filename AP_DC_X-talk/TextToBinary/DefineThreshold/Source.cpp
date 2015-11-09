#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <ctime>
# include <chrono>

//root cern
#include "TTree.h"
#include "TFile.h"

using namespace std;

vector<double> xv;
vector<double> yv;
vector<double> yv_der;
vector<double> yv_der2;

int main()
{
	yv.reserve(20 * 1000 * 1000);
	yv_der.reserve(20 * 1000 * 1000);
	yv_der2.reserve(20 * 1000 * 1000);
	
	string dir_name = "D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\275K\\69_30V\\";
	int file_run = 1;
	double part_or_file = 1;
	
	//-----------------------------------------------
	ostringstream f_oss;
	f_oss << dir_name << "raw\\binary\\run_" << file_run << ".bin";

	FILE *f = fopen(f_oss.str().c_str(), "rb");

	if (f == NULL)
	{
		cout << "Can't open this file: " << f_oss.str().c_str() << endl;
		system("pause");
	}

	int yv_size;
	fread(&yv_size, sizeof(int), 1, f);
	cout << "file length = " << yv_size << endl;

	int yv_size_new = yv_size * part_or_file; // выбрать долю от всех данных. 
	cout << "file length cutted= " << yv_size_new << endl;

	yv.resize(yv_size_new);
	fread(&yv[0], sizeof(vector<double>::value_type), yv_size_new, f);
	//---------------------------------------------


	ostringstream f_der_oss;
	f_der_oss << dir_name << "der\\run_" << file_run << ".bin";

	ostringstream f_der2_oss;
	f_der2_oss << dir_name << "der2\\run_" << file_run << ".bin";

	string der_name = f_der_oss.str();
	string der2_name = f_der2_oss.str();


	//прочитать первую и вторую производную из файла
	FILE *f_der = fopen(der_name.c_str(), "rb");
	FILE *f_der2 = fopen(der2_name.c_str(), "rb");

	if (f_der == NULL || f_der2 == NULL)
	{
		cout << "Can't open this file: " << der_name.c_str() << endl;
		cout << "Can't open this file: " << der2_name.c_str() << endl;
		system("pause");
	}

	yv_der.resize(yv_size_new);
	yv_der2.resize(yv_size_new);
	fread(&yv_der[0], sizeof(vector<double>::value_type), yv_size_new, f_der);
	fread(&yv_der2[0], sizeof(vector<double>::value_type), yv_size_new, f_der2);

	//end of reading
	////////////////////////////////////////////////////////////////////////////////

	string string_tree = dir_name + "tree_th_der.root";
	

	TTree tree("t1", "tree");
	double th_amp, th_der, tau_amp, tau_der;
	int counter;
	
	tree.Branch("th_amp", &th_amp, "th_amp/D");
	tree.Branch("th_der", &th_der, "th_der/D");
	tree.Branch("tau_amp", &tau_amp, "tau_amp/D");
	tree.Branch("tau_der", &tau_der, "tau_der/D");
	tree.Branch("counter", &counter, "counter/I");

	

	int conter_temp = 0;

	const double th_amp_start = -0.01;
	const double th_amp_stop = 0;
	const int th_amp_points = 100;

	double th_amp_step = (th_amp_stop - th_amp_start) / th_amp_points;

	TFile f_tree(string_tree.c_str(), "RECREATE");
	//ofstream file(dir_name + "th_amp.dat");

	const bool starecases_der = true;


	for (tau_amp = 10; tau_amp < 11; tau_amp+= 10)
	{
#pragma omp parallel for num_threads(8)
		for (int j = 0; j < th_amp_points; j++)
		{
			double th_amp_temp = th_amp_start + th_amp_step * j;
			double x_time = 0;
			int counter_temp = 0;
			bool flag = 1;

			for (unsigned int i = 0; i < yv.size(); i++)
			{
				if (starecases_der)
				{
					if ((yv_der[i] < th_amp_temp) && flag && (yv[i] < -0.002))
					{
						x_time = i * 0.2;
						flag = 0;
						counter_temp++;
					}

					if (yv_der[i] > th_amp_temp && (i*0.2 - x_time) > (tau_amp) && flag == 0)
					{
						flag = 1;
					}
				}
				else
				{

					if ((yv[i] < th_amp_temp) && flag )
					{
						x_time = i * 0.2;
						flag = 0;
						counter_temp++;
					}

					if (yv[i] > th_amp_temp && (i*0.2 - x_time) > (tau_amp) && flag == 0)
					{
						flag = 1;
					}
				}

			}

			
			cout << "counter = " << conter_temp << endl;
			conter_temp++;

#pragma omp critical
			{
				counter = counter_temp;
				th_amp = th_amp_temp;
				tree.Fill();
				if (conter_temp % 100 == 1) tree.AutoSave("SaveSelf");
			}

			//file << -th_amp << "\t" << counter << endl;
		}
	}

	tree.Write();


	system("pause");
	return 0;
}
