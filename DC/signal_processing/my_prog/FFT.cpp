
#include <iostream>
#include <fstream>
#include <vector>
#include "FFT.h"

#include "TH1D.h"
#include "TVirtualFFT.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TMath.h"
#include "fftw3.h"


using namespace std;

FFT::FFT(vector<double> &yv)
{
	/*Int_t N = 10; Double_t *in = new Double_t[N];
	TVirtualFFT *fftr2c = TVirtualFFT::FFT(1, &N, "R2C");*/
	//fftr2c->SetPoints(in);
	//fftr2c->Transform();
	//Double_t re, im;
	//for (Int_t i = 0; i < N; i++)
	//{
	//	fftr2c->GetPointComplex(i, re, im);
	//	yv_fft.push_back(sqrt(re*re + im*im));
	//}


}


FFT::~FFT()
{
}
