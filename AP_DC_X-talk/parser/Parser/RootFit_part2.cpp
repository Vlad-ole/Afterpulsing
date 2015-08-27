#include "RootFit.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TMath.h"
#include "Monostate.h"

void RootFit::FindStartStop(double threshold_der, double threshold_amp)
{
	bool flag = 1;


	double x_time;
	//поиск начала и конца 
	for (unsigned int i = 0; i < xv.size(); i++)
	{
		if ((yv_der[i] < threshold_der) && flag)
		{
			x_time = xv[i];
			flag = 0;
			time_start.push_back(i);
		}

		//разрешить искать сигнал, когда сигнал дойдет до шумов
		if (yv[i] > threshold_amp && flag == 0 && (xv[i] - x_time) > 5)
		{
			time_finish.push_back(i);
			flag = 1;
		}
	}
}

void RootFit::CalculateStartParameters(double time_dead, double threshold_der)
{
	
	bool flag = 1;
	int x_time = 0;
	
	int i = current_signal;
	int time_start_index = time_start[i];

	time_front.clear();

	//найти стартовые параметры дл€ начала сигнала
	for (int j = time_start_index; j < time_finish[i]; j++)
	{
		if (yv_der[j] < threshold_der && flag)
		{
			time_front.push_back(j);
			flag = 0;
			x_time = j;
		}

		if (yv_der[j] > threshold_der && (j - x_time) >(time_dead / 0.2))
		{
			flag = 1;
		}

	}
}


void RootFit::CalculateDer(int type, int points)
{
	if (type == 0)
	{
		//simple method
		for (unsigned int i = 0; i < xv.size(); i++)
		{
			if (i < points)
			{
				yv_der.push_back(0);
			}
			else
			{
				yv_der.push_back(yv[i] - yv[i - points]);
			}

		}
	}
	else
	{
		//SavitzkyЦGolay filter
		//order = 3

		vector<double> C_i;

		int m = points;// from 8 to 

		for (int i = (1 - m) / 2.0; i <= (m - 1) / 2.0; i++)
		{
			double numerator = 5 * (3 * pow(m, 4.0) - 18 * pow(m, 2.0) + 31)*i - 28 * (3 * pow(m, 2.0) - 7)*pow(i, 3.0);
			double denominator = m * (pow(m, 2.0) - 1) * (3 * pow(m, 4.0) - 39 * pow(m, 2.0) + 108) / 15;
			C_i.push_back(numerator / denominator);
		}

		/*for (int i = 0; i < C_i.size(); i++)
		{
			cout << C_i[i] << endl;
		}*/


		int point_half = (m - 1) / 2.0;
		double value;
		for (unsigned int i = 0; i < xv.size(); i++)
		{
			if (i < point_half || i >(xv.size() - point_half - 1))
			{
				yv_der.push_back(0);
			}
			else
			{
				value = 0;
				for (int j = 0; j < C_i.size(); j++)
				{
					value += C_i[j] * yv[i - point_half + j];
				}
				yv_der.push_back(value);
			}


		}
	}


}