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


const double rec_lenght = 30000; // (points)
const double threshold_der = -6; // mV

int main()
{
	//file with raw data
	FILE *work_file = fopen("D:\\Data_work\\afterpulsing\\265T_24.00V_000_wf_1.dat", "r");
	if (work_file == NULL)
	{
		cout << "Error read file" << endl;
		exit(0);
	}

	//file with result
	ofstream file_time_charge("D:\\Data_work\\afterpulsing\\time_charge.dat");
	ofstream file_time("D:\\Data_work\\afterpulsing\\time.dat");
	ofstream file_charge("D:\\Data_work\\afterpulsing\\charge.dat");

	vector<double> xv;
	vector<double> yv;

	//read file with raw data
	while (!feof(work_file))
	{
		double x, y;
		fscanf(work_file, "%lf %lf\n", &x, &y);

		xv.push_back(x);
		yv.push_back(y);
	}

	//calculate derivative
	vector<double> yv_der;

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
		if ( (yv_der[i] < threshold_der) && flag && (i > 150) )
		{
			//calculate baseline
			baseline = 0;
			for (int j = (i - 150); j < (i - 50); j++)
			{
				baseline += yv[j];
			}
			baseline /= 100;

			//calculate integral
			integral = 0;
			for (int j = (i - 10); j < (i + 100); j++)
			{
				integral += (baseline - yv[j]);
			}
			
			file_charge << integral << endl;
			
			if (x_time != 0)
			{
				file_time << xv[i] - x_time << endl;
				file_time_charge << xv[i] - x_time << "\t" << integral << endl;
			}
					
			x_time = xv[i];

			flag = 0;
		}		


		if ( yv[i]>threshold && flag == 0 && ( (xv[i] - x_time) > 1 ) )
		{
			flag = 1;
		}

	}



	/*double x1,y;*/

	/*vector<double> xv(20E6);
	vector<double> yv(20E6);

	bool flag=1;

	int fCounter = 0;

	for(int count=0; count<1 ; count++)
	{
	int i_last=0;
	xv.clear();
	yv.clear();

	FILE *work_file=fopen("C:\\diplom\\SPM_gain\\March_14_1.602V_gain.txt", "r");
	if (work_file!=NULL)
	cout << "C:\\diplom\\SPM_gain\\March_14_1.602V_gain.txt" << endl;

	ofstream file_test("C:\\diplom\\SPM_gain\\test.txt");

	double counter=0;
	while (!feof(work_file))
	{
	fscanf(work_file,"%lf %lf\n", &x1, &y);

	x1=counter*2E-10;
	y*=1000;

	xv.push_back(x1);
	yv.push_back(y);

	if (xv.size() % 1000000 == 0)
	cout << xv.size() << endl;

	counter++;

	if (counter < 1000000)
	file_test << x1 << "\t" << y << endl;
	}

	for(int i=0; i < xv.size(); i++)
	{

	stringstream sst;
	sst << "C:\\diplom\\SPM_gain\\signals\\signal" <<  std::setw(7) << std::setfill('0') << fCounter <<".dat";
	ofstream file_out(sst.str().c_str());

	if( yv[i]<threshold && flag && ( (i + 100) < 20E6))
	{

	double x_shift=xv[i-150];
	for(int j=(i-150); j<(i+250); j++)
	{
	file_out << xv[j]-x_shift << "\t" << yv[j] << endl;
	}
	flag=0;

	if (fCounter%100==0 )
	cout << "fCounter=\t" << fCounter << endl;
	fCounter++;
	}

	if(yv[i]>threshold && flag==0)
	{
	flag=1;
	}


	}
	}


	*/

	system("PAUSE");
	return 0;
}


