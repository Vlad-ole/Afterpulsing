#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>


#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <process.h>
#include <time.h>
#include <sys/timeb.h>
#include <iostream>
#include <sstream>
#include <iomanip>
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
	string directory_init = "D:\\Data_work\\afterpulsing\\265T_24.00V\\";
	string directory_raw = directory_init + "raw\\265T_24.00V_001_wf_1.dat";

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
					
			
			yv_der.push_back(0);
			for (int i = 1; i < xv.size(); i++)
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
					for (int j = (i - 20); j < (i - 10); j++)
					{
						baseline += yv[j];
					}
					baseline /= 10;

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




	system("PAUSE");
	return 0;
}


