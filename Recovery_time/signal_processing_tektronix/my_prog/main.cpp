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


const int rec_lenght = 19999981; // (points)
const double threshold_der = -7; // mV
const double threshold = -0.008; // V
const double threshold_2e = -0.018; // V

int main()
{
	string directory_init = "D:\\Data_work\\tektronix_signal\\265K\\265K_72.59\\";
	string directory_raw = directory_init + "raw\\dat.txt";


	//file with raw data
	FILE *work_file = fopen(directory_raw.c_str(), "r");
	if (work_file == NULL)
	{
		cout << "Error read file" << endl;
		exit(0);
	}

	//file with result
	ofstream file_average_signal(directory_init + "average_signal.dat");
	ofstream file_amp(directory_init + "amp.dat");

	ofstream file_out(directory_init + "out.dat");
	ofstream file_test(directory_init + "test.dat");


	vector<double> xv;
	vector<double> yv;

	vector<double> yv_average;

	int time_pre =  (100 / 0.2);
	int time_post = (200 / 0.2);
	int time_gate = time_post + time_pre;

	cout << time_pre << "\t" << time_post << "\t" << time_gate << endl;

	for (int i = 0; i < time_gate; i++)
	{
		yv_average.push_back(0);
	}


	cout.precision(3);
	cout.scientific;

	//read file with raw data
	int counter = 1;
	int number_of_pulsing = 0;

	unsigned long int time_number = 0;

	while (!feof(work_file))
	{
		double x, y;
		fscanf(work_file, "%lf %lf\n", &x, &y);

		xv.push_back(time_number * 2.0E-10);
		yv.push_back(y);

		time_number++;

		//cout << time_number * 2E-10 << "\t" << y << endl;

		if (xv.size() % 100000 == 0)
		{
			cout << double (xv.size()) / rec_lenght * 100 << " %" << endl;
			//break;
		}


		if (xv.size() % rec_lenght == 0) // if you don't have enough memory, you shoud decrease rec_lenght parameter (only here)
		{
			cout << "part " << counter << " was started" << endl;
			counter++;

			//caclulate delta time and charge
			bool flag = 1;
			double baseline;
			double integral;
			double x_time = 0;			
			
						
			for (int i = 0; i < xv.size(); i++)
			{

				
				
				if ((yv[i] < threshold) && flag && (i > time_pre) && ((i + time_post) < xv.size()))
				{
					//calculate amplitude
					double amplitude = 5000;
					for (int j = i - time_pre; j < i + time_post; j++)
					{
						if (yv[j] < amplitude)
							amplitude = yv[j];
					}

					file_amp << amplitude << endl;

					
					
					
					if (amplitude > threshold_2e)
					{
						number_of_pulsing++;
						//cout << "number_of_pulsing " <<  number_of_pulsing << endl;
						
						for (int j = -time_pre; j < time_post; j++)
						{
							yv_average[j + time_pre] += yv[i + j];
							//cout << i + j << "\t" << yv[i + j] << endl;

							//cout << j << "\t" << time_pre << "\t" << j + time_pre << endl;
							
							if ( (j + time_pre) == 512 )
								cout << yv[i + j] << endl;

						}

						//cout << "yv[i + 11] " << yv[i + 11] << " number_of_pulsing " << number_of_pulsing << endl;
						//cout << "yv[i + 12] " << yv[i + 12] << " number_of_pulsing " << number_of_pulsing << endl;
						//cout << "yv[i + 13] " << yv[i + 13] << " number_of_pulsing " << number_of_pulsing << endl;
						//cout << endl;

					}

					x_time = xv[i];

					flag = 0;
				}


				//cout << "(xv[i] - x_time) " << (xv[i] - x_time) << "time_post / (2E-10) " << time_post << endl;

				if ( yv[i] > threshold && flag == 0 && ( (xv[i] - x_time) > time_post*2E-10 ) )
				{
					flag = 1;
				}

			}

			xv.clear();
			yv.clear();

		}

	}

	cout << yv_average[512] << endl;

	for (int i = 0; i < (time_gate ); i++)
	{
		file_test << i << "\t" << yv_average[i] << endl;
	}
	
	for (int i = 0; i < (time_gate); i++)
	{
		file_average_signal << i << "\t" << yv_average[i] / number_of_pulsing << endl;
	}
	
	cout << "number_of_pulsing = \t" << number_of_pulsing << endl;


	fclose(work_file);

	system("PAUSE");
	return 0;
}


