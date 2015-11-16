#include <stdio.h>
#include <stdlib.h>
#include "visa.h"
#include <windows.h>
#include <signal.h>

ViSession	rm = VI_NULL, vi = VI_NULL;

int main(int argc, char* argv[])
{
	ViStatus	status;
	char		string[256];
	ViUInt32	retCnt;
	int			i = 0;

	status = viOpenDefaultRM(&rm);
	if (status < VI_SUCCESS) goto error;

	status = viOpen(rm, "USB::0x0699::0x0405::C025165::INSTR", VI_NULL, VI_NULL, &vi);
	if (status < VI_SUCCESS) goto error;

	// A shared lock only allows other applications that use the same
	// key to have access to the specified resource.
	viLock(vi, VI_SHARED_LOCK, VI_TMO_INFINITE, "mykey", VI_NULL);

	for (i = 1; i < 100; i++) {
		viLock(vi, VI_EXCLUSIVE_LOCK, VI_TMO_INFINITE, VI_NULL, VI_NULL);
		status = viWrite(vi, (ViBuf) "ch1:scale?", 10, &retCnt);
		if (status < VI_SUCCESS) goto error;
		status = viRead(vi, (ViBuf)string, 256, &retCnt);
		if (status < VI_SUCCESS) goto error;
		printf("%d: scale %s", i, string);

		status = viWrite(vi, (ViBuf) "ch1:position?", 13, &retCnt);
		if (status < VI_SUCCESS) goto error;
		status = viRead(vi, (ViBuf)string, 256, &retCnt);
		if (status < VI_SUCCESS) goto error;
		printf("%d: position %s", i, string);
		viUnlock(vi);
		::Sleep(1000);
	}

	// Clean up and exit
	viUnlock(vi);
	viClose(vi);
	viClose(rm);

	system("pause");
	return 0;
error:
	// Print error info
	viStatusDesc(rm, status, string);
	fprintf(stderr, "Error: %s\n", (ViBuf)string);

	// Clean up
	if (vi != VI_NULL) {
		// clear all remaining locks
		while (viUnlock(vi) >= VI_SUCCESS)
			;
		viClose(vi);
	}
	if (rm != VI_NULL)
		viClose(rm);

	system("pause");
	return 0;
}

