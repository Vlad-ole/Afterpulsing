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

string dir_name;
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

	////write data
	//cout << "write file" << endl;
	//stream = fopen(file_write.c_str(), "wb");
	////cout << file_write << endl;
	//if (stream == NULL)
	//{
	//	cout << "can't open this file: " << file_write << endl;
	//}
	//int size = yv.size();
	//fwrite(&size, sizeof(size), 1, stream);
	//fwrite(&yv[0], sizeof(vector<double>::value_type), yv.size(), stream);
	//fclose(stream);

	
	//write der
	cout << "write der" << endl;

	stream = fopen(file_der_write.c_str(), "wb");


	fwrite(&yv_der[0], sizeof(vector<double>::value_type), yv.size(), stream);

	fclose(stream);


	//write der2
	cout << "write der2" << endl;

	stream = fopen(file_der2_write.c_str(), "wb");
	fwrite(&yv_der2[0], sizeof(vector<double>::value_type), yv.size(), stream);

	fclose(stream);
}

//прочитать бинарный файл и записать его в вектор
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

	int yv_size;
	//fread(&yv_size_tmp, sizeof(int), 1, stream);
	yv_size = 5000*1000;
	//cout << "yv_size_tmp = " << yv_size_tmp << endl;
	cout << "file length = " << yv_size << endl;

	yv.resize(yv_size);
	fread(&yv[0], sizeof(vector<double>::value_type), yv_size, stream);

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

int main(int argc, char* argv[])
{
	cout << "number of arguments = " << argc << endl;
	if (argc == 1)
	{
		//dir_name = "D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\275K\\68_60V\\";
		cout << "Please, input dir_name " << endl;
		cout << argv[0] << endl;
		return 0;
	}
	else if (argc == 4)
	{
		dir_name = argv[1];
		cout << argv[0] << endl;
		cout << argv[1] << endl;
		cout << argv[2] << endl;
		cout << argv[3] << endl;
	}
	else
	{
		cout << "too many or too few arguments! " << endl;
	}



	int file_i_start = atoi(argv[2]);
	int file_i_stop = atoi(argv[3]);

	cout << "file_i_start = " << file_i_start << endl;
	cout << "file_i_stop = " << file_i_stop << endl;

	long int before = GetTickCount();

	yv.reserve(20 * 1000 * 1000);
	yv_der.reserve(20 * 1000 * 1000);
	yv_der2.reserve(20 * 1000 * 1000);

	const int points = 51;
	CalculateFilterCoeff(points);


	for (file_i = file_i_start; file_i <= file_i_stop; file_i++)
	{
		ostringstream file_read_oss;
		file_read_oss << dir_name << "raw\\binary\\run_" << file_i << ".bin";

		//ostringstream file_write_oss;
		//file_write_oss << dir_name << "raw\\binary\\run_" << file_i << ".bin";

		ostringstream file_der_write_oss;
		file_der_write_oss << dir_name << "der\\run_" << file_i << ".bin";

		ostringstream file_der2_write_oss;
		file_der2_write_oss << dir_name << "der2\\run_" << file_i << ".bin";

		file_read = file_read_oss.str();
		read_data();

		for (int i = 0; i < 5; i++)
		{
			cout << yv[i] << endl;
		}

		CalculateDerivative(points);


		//file_write = file_write_oss.str();
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