#pragma once
#include <vector>
using namespace std;


class FFT
{
public:
	FFT(vector<double> &yv);
	~FFT();

	vector<double> yv_fft;

};

