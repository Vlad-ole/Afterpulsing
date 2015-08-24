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
#include "TObjArray.h"
#include "TGraph.h"
#include "TFile.h"

using namespace std;


const int rec_lenght = 19999981; // (points)
const double threshold_der = -0.006; // mV
const double threshold = -0.008; // V
const double threshold_2e = -0.018; // V

int main()
{
	string directory_init = "D:\\Data_work\\tektronix_signal\\280K\\280K_73.24\\";
	string directory_raw = directory_init + "raw\\20M.txt";

	TObjArray Hlist_reco_time_bad(0);
	TObjArray Hlist_reco_time_good(0);

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

	ofstream file_derivative(directory_init + "derivative.dat");
	//ofstream file_der_staircase(directory_init + "der_staircase.dat");

	ofstream file_test_signal(directory_init + "test_signal.dat");

	ofstream file_num_1e(directory_init + "num_1e.dat");

	vector<double> xv;
	vector<double> yv;

	vector<double> yv_der;

	vector<double> yv_average;

	const int time_pre = (500 / 0.2);
	const int time_post = (500 / 0.2);


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

	}
		//if (xv.size() % rec_lenght == 0) // 
		{
			cout << "part " << counter << " was started" << endl;
			counter++;


			//caclulate derivative
			for (int i = 0; i < xv.size(); i++)
			{
				if (i < 20)
					yv_der.push_back(0);
				else
					yv_der.push_back(yv[i] - yv[i - step]);
			}

			// лестница для производной
			/*bool flag_0 = 1;
			int th_der_count = 0;
			double th_der_time_last = 0;

			for (double th_der = 0; th_der > -0.03; th_der-=0.0004)
			{			
				
				th_der_count = 0;
				for (int i = 0; i < (xv.size() - step); i++)
				{
					if ((yv_der[i] < th_der) && flag_0 && (i > time_pre) && ((i + time_post) < (xv.size() - step)))
					{
						th_der_count++;
						flag_0 = 0;
						th_der_time_last = xv[i];
					}

					if (yv_der[i] > th_der && flag_0 == 0 && ((xv[i] - th_der_time_last) > 20 * 2E-10) )
					{
						flag_0 = 1;
					}

				}

				file_der_staircase << th_der << "\t" << th_der_count << endl;

			}*/

			
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

			for (int i = 0; i < (xv.size()); i++)
			{

				if ((yv_der[i] < threshold_der) && flag && (i > time_pre) && ((i + time_post) < (xv.size()) ) )
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

					if (total_num_pulsing == 1 && one_time)
					{
						for (int j = i - time_pre; j < i + time_post; j++)
						{
							file_test_signal << j << "\t" << yv[j] << endl;
						}
						
						one_time = 0;
					}

					file_test << total_num_pulsing << endl;


					amplitude = 5000;
					if (total_num_pulsing == 1) // отбрасывание послеимпульсов
					{
						for (int j = i - time_pre; j < i + time_post; j++) // нахождение максимальной амплитуды
						{
							if (yv[j] < amplitude)
								amplitude = yv[j];
						}

						TGraph *gr = new TGraph(time_pre + time_post, &xv[i - time_pre], &yv[i - time_pre]);
						
						if (amplitude > -0.008)
						{							
							Hlist_reco_time_bad.Add(gr);
						}
						
						Hlist_reco_time_good.Add(gr);

						file_amp << abs(amplitude) << endl;

						if (amplitude > threshold_2e && amplitude < threshold) // проверка, что импульс одноэлектронный
						{

							number_of_pulsing++;
							for (int j = -time_pre; j < time_post; j++)// запись значений в вектор среднего
							{
								yv_average[j + time_pre] += yv[i + j];
							}
						}

					}



					x_time = xv[i];
					flag = 0;

				}


				if (yv_der[i] > threshold_der && flag == 0 && ((xv[i] - x_time) > time_post*2E-10))
				{
					flag = 1;
				}
			}



			////writre file with derivative

			//for (int k = step; k < 50000; k++)
			//{
			//	file_derivative << k*2E-10 << "\t" << yv[k + step] - yv[k] << endl;
			//}


			//for (int k = step; k < 50000; k++)
			//{
			//	file_part_signal << xv[k] << "\t" << yv[k] << endl;
			//}


			xv.clear();
			yv.clear();
		}

	//}//while


	string ofile_0_s = directory_init + "Hlist_reco_time_bad.root";
	string ofile_1_s = directory_init + "Hlist_reco_time_good.root";
	
	TFile ofile_0(ofile_0_s.c_str(), "RECREATE");
	Hlist_reco_time_bad.Write();
	ofile_0.Close();

	TFile ofile_1(ofile_1_s.c_str(), "RECREATE");
	Hlist_reco_time_good.Write();
	ofile_1.Close();

	for (int i = 0; i < (time_gate); i++)
	{
		file_average_signal << i << "\t" << yv_average[i] / number_of_pulsing << endl;
	}

	cout << "number_of_pulsing = \t" << number_of_pulsing << endl;


	file_num_1e << number_of_pulsing  << endl;

	fclose(work_file);

	system("PAUSE");
	return 0;
}


