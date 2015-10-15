#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <Windows.h>

using namespace std;

//vector<double> xv;
vector<double> yv;

string dir_name = "D:\\Data_work\\tektronix_signal\\MPPC_S10362-11-100C\\70_00V\\raw\\";
string file_name = "run_5";
string file_write = dir_name + "binary\\" + file_name + ".bin";
string file_read = dir_name + "text\\" + file_name + ".txt";

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
			cout << "read file " << counter / double(2E7) * 100 << endl;
		}

		counter++;
	}

	fclose(stream);
}

int main()
{
	long int before = GetTickCount();
	read_data();
	write_data();
	long int after = GetTickCount();


	cout << endl << "run time (in ms) \t " << after - before << endl;
	cout << endl << "run time (in s) \t " << (after - before) / 1000.0 << endl;
	cout << endl << "run time (in m) \t " << (after - before) / (1000.0 * 60) << endl;

	system("pause");
	return 0;
}