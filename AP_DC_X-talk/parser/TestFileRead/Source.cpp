#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <Windows.h>

using namespace std;

vector<double> xv;
vector<double> yv;

double *b = new double[19999981];

int a;

string dir_name = "D:\\Data_work\\tektronix_signal\\295K\\73_90V\\raw\\";
string file_name = "run_1.bin";
string file_write = dir_name + "binary\\" + file_name;


void read_data()
{
	//read	
	cout << "read file" << endl;

	
	FILE *stream;
	stream = fopen(file_write.c_str(), "rb");
	
	
	fread(&a, sizeof(int), 1, stream);
	yv.resize(a);

	const int N = 10000;

	double *dP = &b[0];

	//fread(dP, sizeof(double), N, stream);
	int counter = 0;

	fread(&yv[0 + counter], sizeof(vector<double>::value_type), a, stream);
	//while (!feof(stream))
	//{

	//	//fread(dP, sizeof(double), N, stream);
	//	//dP += N;

	//	fread(&yv[0 + counter], sizeof(vector<double>::value_type), N, stream);
	//	counter++;


	////	//for (int i = 0; i < N; i++)
	////	//{
	////	//	//xv.push_back(b[i]);
	////	//	//xv[i + counter*N] = b[i];
	////	//	//xv_stack[] = b[i];
	////	//}
	////	//cout << "counter = " << counter << endl;
	////	//counter++;
	////	
	//}

	fclose(stream);
}

void read_data_cpp()
{
	cout << "read file cpp" << endl;
	
	ifstream stream(file_write);
	stream.read( (char *)&a, sizeof(int) );
	
	while (!stream.eof())
	{
		
	}
	
}


int main()
{

	long int before = GetTickCount();
	read_data();
	long int after = GetTickCount();

	cout << "size = " << a << endl << endl;
	
	for (int i = 0; i < 30; i++)
	{
		cout << yv[i] << endl;
	}

	

	cout << endl << "run time (in ms) \t " << after - before << endl;
	cout << endl << "run time (in s) \t " << (after - before) / 1000.0 << endl;
	cout << endl << "run time (in m) \t " << (after - before) / (1000.0 * 60) << endl;

	system("pause");
	return 0;
}