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

string dir_name = "D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\295K\\70_01V\\";
string file_name;
string file_write;
string file_read;

int file_i;

void write_data()
{
	//write
	cout << "write file" << endl;
	FILE *stream;
	
	stream = fopen(file_write.c_str(), "wb");

	int size = yv.size();
	fwrite(&size, sizeof(size), 1, stream);

	for (int i = 0; i < yv.size(); i++)
	{
		double a = yv[i];
		fwrite(&a, sizeof(a), 1, stream);
	}

	fclose(stream);
}

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
			cout << "read file " << counter / double(2E7) * 100 << " file № " << file_i << endl;
		}

		counter++;
	}

	fclose(stream);
}

int main()
{
	long int before = GetTickCount();

	for (file_i = 1; file_i <= 10; file_i++)
	{
		ostringstream file_read_oss;
		file_read_oss << dir_name << "raw\\text\\run_" << file_i << ".txt";
		
		ostringstream file_write_oss;
		file_write_oss << dir_name << "raw\\binary\\run_" << file_i << ".bin";

		file_write = file_write_oss.str();
		file_read = file_read_oss.str();
		
		yv.clear();
		read_data();
		write_data();
	}

	long int after = GetTickCount();
	
	cout << endl << "run time (in ms) \t " << after - before << endl;
	cout << endl << "run time (in s) \t " << (after - before) / 1000.0 << endl;
	cout << endl << "run time (in m) \t " << (after - before) / (1000.0 * 60) << endl;

	system("pause");
	return 0;
}