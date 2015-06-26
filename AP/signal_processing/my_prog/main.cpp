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
const double threshold=-100; // in mV
double time_v=400;//=150; //in nanoseconds!!!
const double shift=-100; //in nanoseconds
const double shift_initial=1000; //in nanoseconds
const double ADC_time=1.0/5; //in nanoseconds
const double ADC_time_absolute=ADC_time*pow(10.0, -9); //in seconds
//const double z_shift=-6.05442; //750V
const double z_shift=0; //800V


int main()
{
	double x1,y;

	vector<double> xv(20E6);
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




	system("PAUSE");
	return 0;
}


