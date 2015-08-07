#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

#include "FFT.h"
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <process.h>
#include <time.h>
#include <sys/timeb.h>
#include <iostream>
#include <math.h>

using namespace std;


const int rec_lenght = 30000; // (points)
const int threshold_init = 3662;
const int threshold_final = 3530;

bool filter = false;

int main()
{
	string directory_init = "D:\\Data_work\\dark_current\\Hamamatsu_S10362-33-025C\\295\\295K_71.90V\\";
	string directory_raw = directory_init + "raw\\dump_000_wf_1.dat";


	//file with raw data
	FILE *work_file = fopen(directory_raw.c_str(), "r");
	if (work_file == NULL)
	{
		cout << "Error read file" << endl;
		exit(0);
	}

	//file with result
	//if (filter)
	//{
	//	ofstream file_staircase(directory_init + "filter//staircase.dat");
	//	ofstream file_staircase_rate(directory_init + "filter//staircase_rate.dat");
	//}
	
		ofstream file_staircase(directory_init + "staircase.dat");	
		ofstream file_staircase_rate(directory_init + "staircase_rate.dat");
	
	

	vector<double> xv;
	vector<double> yv;

	vector<int> signal_counter_v;

	for (int i = 0; i < (threshold_init - threshold_final); i++)
	{
		signal_counter_v.push_back(0);
	}

	cout.precision(3);
	cout.scientific;

	//read file with raw data
	int counter = 1;
	int counter_2 = 0;

	while (!feof(work_file))
	{
		double x, y;
		fscanf(work_file, "%lf %lf\n", &x, &y);

		xv.push_back(x);
		yv.push_back(y);


		if (xv.size() % rec_lenght == 0)
		{
			cout << "part " << counter << " was started" << endl;
			counter++;

			bool flag = 1;
			double x_time = 0;
			int signal_counter;

			//filter
			if (filter)
			{
				for (int k = 5; k < rec_lenght; k++)
				{
					yv[k] = (yv[k - 4] + yv[k - 3] + yv[k - 2] + yv[k - 1] + yv[k]) / 5;
					//file_out << k << "\t" << yv[k] << endl;
				}
			}

			

			for (int threshold = threshold_init; threshold > threshold_final; threshold--)
			{
				signal_counter = 0;

				//int threshold = threshold_init;

				for (int i = 0; i < xv.size(); i++)
				{

					if ((yv[i] < threshold) && flag /* && (i > 150) && ( (i + 30) < xv.size() )*/)
					{
						signal_counter++;
						x_time = xv[i];
						flag = 0;
						//cout << "signal" << endl;
					}


					if (yv[i] > threshold && flag == 0 && ((xv[i] - x_time) > (1)))
					{
						flag = 1;
					}

				}

				signal_counter_v[threshold_init - threshold] += signal_counter;

			}

			xv.clear();
			yv.clear();

		}

	}

	for (int i = 0; i < (threshold_init - threshold_final); i++)
	{
		file_staircase << i << "\t" << signal_counter_v[i] << endl;
		file_staircase_rate << i << "\t" << signal_counter_v[i] / (counter*rec_lenght*4.0*1E-9) << endl;
	}



	fclose(work_file);

	system("PAUSE");
	return 0;
}


