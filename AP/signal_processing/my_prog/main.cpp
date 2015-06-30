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
const double threshold = -100; // in mV
double time_v = 400;//=150; //in nanoseconds!!!
const double shift = -100; //in nanoseconds
const double shift_initial = 1000; //in nanoseconds
const double ADC_time = 1.0 / 5; //in nanoseconds
const double ADC_time_absolute = ADC_time*pow(10.0, -9); //in seconds
//const double z_shift=-6.05442; //750V
const double z_shift = 0; //800V


const int rec_lenght = 30000; // (points)
const double threshold_der = -7; // mV

int main()
{
	string directory_init = "D:\\Data_work\\afterpulsing\\275T_71.00V_ham_33_25\\";
	string directory_raw = directory_init + "raw\\ham_test_000_wf_1.dat";

	string directory_time = directory_init + "time.dat";
	string directory_charge = directory_init + "charge.dat";
	string directory_time_charge = directory_init + "time_charge.dat";


	//file with raw data
	FILE *work_file = fopen(directory_raw.c_str(), "r");
	if (work_file == NULL)
	{
		cout << "Error read file" << endl;
		exit(0);
	}

	//file with result
	ofstream file_time(directory_time);
	ofstream file_charge(directory_charge);
	ofstream file_time_charge(directory_time_charge);

	ofstream file_out(directory_init + "out.dat");


	vector<double> xv;
	vector<double> yv;

	//calculate derivative
	vector<double> yv_der;

	cout.precision(3);
	cout.scientific;

	//read file with raw data
	int counter = 1;
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


			////filter
			//for (int k = 5; k < rec_lenght; k++)
			//{
			//	yv[k] = (yv[k - 4] + yv[k - 3] + yv[k - 2] + yv[k - 1] + yv[k]) / 5;
			//	//file_out << k << "\t" << yv[k] << endl;
			//}

			//calculate derivative	
			yv_der.push_back(0);
			for (int i = 1; i < rec_lenght; i++)
			{
				yv_der.push_back(yv[i] - yv[i - 1]);
				//file_out << xv[i] << "\t" << yv_der[i] << endl;
			}

	
			//caclulate delta time and charge
			bool flag = 1;
			double baseline;
			double integral;
			double x_time = 0;

			for (int i = 0; i < xv.size(); i++)
			{
				if ((yv_der[i] < threshold_der) && flag && (i > 150) && ( (i + 5) < xv.size() ) )
				{
					//calculate baseline
					baseline = 0;
					for (int j = (i - 6); j < (i - 5); j++)
					{
						baseline += yv[j];
					}

					baseline /= 1;

					////calculate integral
					//integral = 0;
					//for (int j = (i - 10); j < (i + 150); j++)
					//{
					//	integral += (baseline - yv[j]);
					//}

					//calculate amplitude
					integral = 5000;
					for (int j = (i - 5); j < (i + 5); j++)
					{
						if (yv[j] < integral)
							integral = yv[j];
					}

					integral = baseline - integral;

					file_charge << integral << endl;

					if (x_time != 0)
					{
						file_time << xv[i] - x_time << endl;
						file_time_charge << xv[i] - x_time << "\t" << integral << endl;
					}

					x_time = xv[i];

					flag = 0;
				}


				if (yv[i] > threshold && flag == 0 && ((xv[i] - x_time) > 1))
				{
					flag = 1;
				}

			}

			xv.clear();
			yv.clear();
			yv_der.clear();
						

		}

	}

	fclose(work_file);

	system("PAUSE");
	return 0;
}


