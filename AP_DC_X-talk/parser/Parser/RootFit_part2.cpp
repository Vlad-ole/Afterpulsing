#include "RootFit.h"
#include "TGraphErrors.h"
#include  "TF1.h"
#include "TMath.h"
#include "Monostate.h"

vector<double> RootFit::yv_der;

void RootFit::FindStartStop()
{
	bool flag = 1;


	double x_time;
	//����� ������ � ����� 
	for (unsigned int i = 0; i < xv.size(); i++)
	{
		if ((yv_der[i] < threshold_der) && flag)
		{
			x_time = xv[i];
			flag = 0;
			time_start.push_back(i);
		}

		//��������� ������ ������, ����� ������ ������ �� �����
		if (yv[i] > threshold_amp && flag == 0 && (xv[i] - x_time) > 5)
		{
			time_finish.push_back(i);
			flag = 1;
		}
	}
}

void RootFit::CalculateStartParameters(double time_dead)
{
	time_start_index = time_start[current_signal] - time_shit;
	time_finish_index = time_finish[current_signal];

	bool flag = 1;
	int x_time = 0;
	
	//int time_start_index = time_start[current_signal];

	time_front.clear();

	int time_dead_index = time_dead / 0.2;

	//����� ��������� ��������� ��� ������ �������
	for (int j = time_start_index; j < time_finish[current_signal]; j++)
	{
		
		if (yv_der[j] < threshold_der && flag)
		{
			//int der_min = j;
			//for (int k = j; k < j + time_dead_index; k++)
			//{
			//	if (yv_der[k] < yv_der[j])
			//	{
			//		der_min = k;
			//	}

			//	//cout << k << "\t" << der_min << endl;
			//	cout << yv_der[k] << "\t" << yv_der[der_min] << endl;
			//	cout << xv[k] << "\t" << xv[der_min] << endl;
			//	cout << endl;
			//}

			//it2 = max_element(myVector.begin(), myVector.end());
			//cout << " the max is " << *it2 << endl;
			
			//cout << *min_element(yv_der.begin(), yv_der.end()) << endl;

			int min_index = min_element(yv_der.begin() + j, yv_der.begin() + j + time_dead_index) - yv_der.begin();

			//cout << xv[j] << "\t" << xv[min_index] << "\t" << xv[j + time_dead_index] << "\t" << current_signal << endl;
			//cout << xv[der_min] << endl;

			//cout << "min value is " << *min_element(yv_der.begin() + j, yv_der.begin() + j + time_dead_index) << endl;
			//cout << "min value at " << min_element(yv_der.begin() + j, yv_der.begin() + j + time_dead_index) - yv_der.begin() << endl;
			
			//system("pause");

			time_front.push_back(min_index);
			flag = 0;
			x_time = j;
		}

		if (yv_der[j] > threshold_der && (j - x_time) > (time_dead_index) )
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
		//Savitzky�Golay filter
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