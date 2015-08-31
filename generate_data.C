#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <TRandom.h>

using namespace std;
using namespace TMath;

void generate_data()
{
	ofstream file("F:\\Data_work\\tektronix_signal\\295K\\295K_73.90\\raw\\test_data.txt");
	
	int period = 1000;
	int numder = 10;
	
	for(int i = 0; i < numder; i++)
	{
		for(int j = i*period; j < (i+1)*period; j++)
		{
			//meander
			
			if( j > (i*period + 400) && j < (i*period + 600) )
				file << j*0.2 << "\t" << 0 << endl;
			else
				file << j*0.2 << "\t" << -0.01 << endl;
			//sin
			//file << j*0.2 << "\t" << sin(j / (period * 1.0) ) << endl;			
			
		}		
		
	}
}
//