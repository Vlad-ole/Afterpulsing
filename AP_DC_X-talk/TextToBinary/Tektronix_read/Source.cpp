#include <iostream>
#include "visa.h"

using namespace std;

int main()
{

	ViSession rm = NULL, vi = NULL;
	ViUInt32 retCnt;
	ViChar buffer[256];
	
	viOpenDefaultRM(&rm);
	viOpen(rm, "GPIB0::1::INSTR", VI_NULL, VI_NULL, &vi);
	//viWrite(vi, "*idn ? ", 5, &retCnt);
	
	//viRead(vi, buffer, 256, &retCnt);
	printf("device: %s\n", buffer);
	viClose(rm);


	system("pause");
	return 0;
}

//06.11.2015 14:53 32.976 	 0.0003s 	 USB::0x0699::0x0405::C025165::INSTR 	 TEKTRONIX, DPO4104B, C025165, CF : 91.1CT FV : v3.18 	 Read
