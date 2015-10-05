#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <Windows.h>

using namespace std;

vector<double> xv;
vector<double> yv;

int main()
{
	//double xv_stack[50000000];

	////write
	//cout << "write file" << endl;
	//FILE *stream;
	//stream = fopen("G:\\test_file_read.dat", "wb");

	//
	//for (int i = 0; i < 50000000; i++)
	//{
	//	double a = i;
	//	fwrite(&a, sizeof(a), 1, stream);
	//}

	//fclose(stream);

	

	//read	
	cout << "read file" << endl;

	long int before = GetTickCount();
	FILE *stream;
	stream = fopen("G:\\test_file_read.dat", "rb");
	const int N = 10000;

	double b[50000];
	
	while (!feof(stream))
	{
		
		fread(&b, sizeof(b), 1, stream);		

		for (int i = 0; i < N; i++)
		{
			//xv.push_back(b[i]);
		}

		//counter++;	
	}

	//for (int i = 0; i < xv.size(); i++)
	//{
	//	cout << "value = " << xv[i] << endl;
	//}
	fclose(stream);

	long int after = GetTickCount();
	cout << endl << "run time (in ms) \t " << after - before << endl;
	cout << endl << "run time (in s) \t " << (after - before) / 1000.0 << endl;
	cout << endl << "run time (in m) \t " << (after - before) / (1000.0 * 60) << endl;

	system("pause");
	return 0;
}