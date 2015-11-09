#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <Windows.h>
#include <sstream>

using namespace std;

//vector<double> xv;
vector<double> yv;
vector<double> yv_der;
vector<double> yv_der2;

vector<double> C_i_der;
vector<double> C_i_der2;

string dir_name = "D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\290K\\69_83V\\";
string file_name;
string file_write;
string file_read;

string file_der_write;
string file_der2_write;

int file_i;

//записать данные, 1-ю и 2-ю производные
void write_data()
{
	FILE *stream;
	
	//write data
	cout << "write file" << endl;
	stream = fopen(file_write.c_str(), "wb");

	//cout << file_write << endl;

	if (stream == NULL)
	{
		cout << "can't open this file: " << file_write << endl;
	}

	int size = yv.size();
	fwrite(&size, sizeof(size), 1, stream);
	fwrite(&yv[0], sizeof(vector<double>::value_type), yv.size(), stream);

	//for (int i = 0; i < yv.size(); i++)
	//{
	//	double a = yv[i];
	//	fwrite(&a, sizeof(a), 1, stream);
	//}
	
	fclose(stream);



	//write der
	cout << "write der" << endl;

	stream = fopen(file_der_write.c_str(), "wb");

	
	fwrite(&yv_der[0], sizeof(vector<double>::value_type), yv.size(), stream);
	//for (int i = 0; i < yv.size(); i++)
	//{
	//	double a = yv[i];
	//	fwrite(&a, sizeof(a), 1, stream);
	//}

	fclose(stream);


	//write der2
	cout << "write der2" << endl;

	stream = fopen(file_der2_write.c_str(), "wb");
	fwrite(&yv_der2[0], sizeof(vector<double>::value_type), yv.size(), stream);
	//for (int i = 0; i < yv.size(); i++)
	//{
	//	double a = yv[i];
	//	fwrite(&a, sizeof(a), 1, stream);
	//}

	fclose(stream);


}

//прочитать текстовый файл и записать его в вектор
void read_data()
{
	//read	
	cout << "read file" << endl;

	FILE *stream;

	stream = fopen(file_read.c_str(), "rb");
	if (stream == NULL)
	{
		cout << "can't open file: " << file_read << endl;
	}

	int counter = 0;
	double x, y;
	while (!feof(stream))
	{
		fscanf(stream, "%lf %lf\n", &x, &y);
		//xv.push_back(x); // in ns
		yv.push_back(y);

		//показать прогресс
		if (counter % 50000 == 0)
		{
			long int read_file = GetTickCount();
			cout << "read file " << counter / double(2E7) * 100 << " file є " << file_i << endl;
		}

		counter++;
	}

	fclose(stream);
}


void CalculateFilterCoeff(int points)
{
	cout << endl << "start Calculate filter coefficients" << endl;
	//SavitzkyЦGolay filter
	//order = 3
	
	int m = points;// from 8 to 

	//посчитать коэффициенты  C_i
	for (int i = (1 - m) / 2.0; i <= (m - 1) / 2.0; i++)
	{
		double numerator = 5 * (3 * pow(m, 4.0) - 18 * pow(m, 2.0) + 31)*i - 28 * (3 * pow(m, 2.0) - 7)*pow(i, 3.0);
		double denominator = m * (pow(m, 2.0) - 1) * (3 * pow(m, 4.0) - 39 * pow(m, 2.0) + 108) / 15.0;
		C_i_der.push_back(numerator / denominator);

		numerator = 12 * m * pow(i, 2.0) - m * (pow(m, 2.0) - 1);
		denominator = pow(m, 2.0) * (pow(m, 2.0) - 1)*(pow(m, 2.0) - 4) / 30.0;
		C_i_der2.push_back(numerator / denominator);

		//numerator = (3 * pow(m, 2.0) - 7 - 20 * pow(i, 2.0)) / 4.0;
		//denominator = m * (pow(m, 2.0) - 4) / 3.0;
		//C_i_s.push_back(numerator / denominator);

	}
	cout << endl << "stop Calculate filter coefficients" << endl;
}

void CalculateDerivative(int points)
{
	cout << endl << "start Calculate Derivatives" << endl;
	const int point_half = (points - 1) / 2.0;
	yv_der.resize(yv.size());
	yv_der2.resize(yv.size());

#pragma omp parallel for num_threads(8)
	for (int i = 0; i < yv.size(); i++)
	{

		if (i < point_half || i >(yv.size() - point_half - 1))
		{
			yv_der[i] = 0;
			yv_der2[i] = 0;
		}
		else
		{
			double value = 0;
			for (int j = 0; j < C_i_der.size(); j++)
			{
				value += C_i_der[j] * yv[i - point_half + j];
			}
			yv_der[i] = value;


			value = 0;
			for (int j = 0; j < C_i_der.size(); j++)
			{
				value += C_i_der2[j] * yv[i - point_half + j];
			}
			yv_der2[i] = value;

		}

	}
}

int main()
{
	long int before = GetTickCount();

	yv.reserve(20 * 1000 * 1000);
	yv_der.reserve(20*1000*1000);
	yv_der2.reserve(20 * 1000 * 1000);

	const int points = 51;
	CalculateFilterCoeff(points);
	

	for (file_i = 1; file_i <= 5; file_i++)
	{
		ostringstream file_read_oss;
		file_read_oss << dir_name << "raw\\text\\run_" << file_i << ".txt";

		ostringstream file_write_oss;
		file_write_oss << dir_name << "raw\\binary\\run_" << file_i << ".bin";

		ostringstream file_der_write_oss;
		file_der_write_oss << dir_name << "der\\run_" << file_i << ".bin";

		ostringstream file_der2_write_oss;
		file_der2_write_oss << dir_name << "der2\\run_" << file_i << ".bin";

		file_read = file_read_oss.str();
		read_data();

		CalculateDerivative(points);

		
		file_write = file_write_oss.str();		
		file_der_write = file_der_write_oss.str();
		file_der2_write = file_der2_write_oss.str();
		write_data();

		yv.clear();
		yv_der.clear();
		yv_der2.clear();	
		
	}

	long int after = GetTickCount();

	cout << endl << "run time (in ms) \t " << after - before << endl;
	cout << endl << "run time (in s) \t " << (after - before) / 1000.0 << endl;
	cout << endl << "run time (in m) \t " << (after - before) / (1000.0 * 60) << endl;

	system("pause");
	return 0;
}