#include <iostream>
#include "visa.h"

int main(int argc, char* argv[])
{
	ViSession	rm, vi;
	ViStatus	status;
	char		string[256];
	ViUInt32	retCnt;

	status = viOpenDefaultRM(&rm);
	if (status < VI_SUCCESS) goto error;

	status = viOpen(rm, "USB::0x0699::0x0405::C025165::INSTR", NULL, NULL, &vi);
	if (status < VI_SUCCESS) goto error;

	status = viWrite(vi, (ViBuf) "*idn?", 5, &retCnt);
	if (status < VI_SUCCESS) goto error;

	status = viRead(vi, (ViBuf)string, 256, &retCnt);
	if (status < VI_SUCCESS) goto error;

	printf("*idn response %s\n", string);

	viClose(vi);
	viClose(rm);

	system("pause");
	return 0;
error:
	viStatusDesc(rm, status, string);
	fprintf(stderr, "Error: %s\n", (ViBuf)string);

	system("pause");
	return 0;
}

