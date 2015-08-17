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
const double threshold_der = -0.006; // mV
const double threshold = -0.008; // V
const double threshold_2e = -0.018; // V

int main()
{
	string directory_init = "D:\\Data_work\\tektronix_signal\\295K\\295K_73.00\\";
	string directory_raw = directory_init + "raw\\dat.txt";


	//file with raw data
	FILE *work_file = fopen(directory_raw.c_str(), "r");
	if (work_file == NULL)
	{
		cout << "Error read file" << endl;
		exit(0);
	}

	//file with result
	ofstream file_amp(directory_init + "amp.dat");
	ofstream file_dtime(directory_init + "dtime.dat");
	ofstream file_N_pulsing_in_gate(directory_init + "N_pulsing_in_gate.dat");
	ofstream file_N_total(directory_init + "N_total.dat");
	ofstream file_dc_rate(directory_init + "dc_rate.dat");
	ofstream file_chi2(directory_init + "chi2.dat");

	ofstream file_part(directory_init + "part.dat");


	//**********************************************
	//stop

	vector<double> xv;
	vector<double> yv;

	vector<double> yv_der;

	vector<double> yv_average;

	const int time_pre = (1000 / 0.2);
	const int time_post = (1000 / 0.2);


	const int time_gate = time_post + time_pre;
	const int step = 20;

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
			cout << double(xv.size()) / rec_lenght * 100 << " %" << endl;
			//break;
		}


		if (xv.size() % rec_lenght == 0) // 
		{
			cout << "part " << counter << " was started" << endl;
			counter++;


			//caclulate derivative
			for (int i = 0; i < (xv.size() - step); i++)
			{
				yv_der.push_back(yv[i + step] - yv[i]);
			}

						
			//caclulate delta time and charge
			bool flag = 1;
			double baseline;
			double integral;
			double x_time = 0;
			double amplitude;
			int total_num_pulsing;
			bool flag_afp;
			int x_time_afp;
			
			bool one_time = 1;

			for (int i = 0; i < (xv.size() - step); i++)
			{

				if ((yv[i] < threshold) && flag && (i > time_pre) && ((i + time_post) < (xv.size() - step)))
				{

					flag_afp = 1;
					//x_time_afp = i;
					total_num_pulsing = 0;


					//вычисление количества импульсов на отрезке 
					for (int j = i - time_pre; j < i + time_post; j++)
					{
						if (yv_der[j] < threshold_der && flag_afp)
						{
							total_num_pulsing++;
							flag_afp = 0;

							x_time_afp = j;
						}

						if (yv_der[j] > threshold_der && (flag_afp == 0) && (j - x_time_afp) > 40)
						{
							flag_afp = 1;
							 
						}
					}

					cout << "total_num_pulsing " << total_num_pulsing << endl;


					amplitude = 5000;
					if (total_num_pulsing == 1) // отбрасывание послеимпульсов
					{
						for (int j = i - time_pre; j < i + time_post; j++) // нахождение максимальной амплитуды
						{
							if (yv[j] < amplitude)
								amplitude = yv[j];
						}

						if (amplitude > threshold_2e && amplitude < threshold) // проверка, что импульс одноэлектронный
						{

							number_of_pulsing++;
							for (int j = -time_pre; j < time_post; j++)// записть значений в вектор среднего
							{
								yv_average[j + time_pre] += yv[i + j];
							}
						}

					}



					x_time = xv[i];
					flag = 0;

				}


				if (yv[i] > threshold && flag == 0 && ((xv[i] - x_time) > time_post*2E-10))
				{
					flag = 1;
				}
			}

			xv.clear();
			yv.clear();
		}

	}


	fclose(work_file);

	system("PAUSE");
	return 0;
}


