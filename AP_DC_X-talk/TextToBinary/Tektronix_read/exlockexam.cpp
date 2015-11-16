#include <stdio.h>
#include <stdlib.h>
#include "visa.h"

//print voltage scale and voltage position for the first channel

int main(int argc, char* argv[])
{
	ViSession	rm = VI_NULL, vi = VI_NULL;
	ViStatus	status;
	char		string[256];
	ViUInt32	retCnt;
	int			i = 0;
	bool		lockflag = false;
	bool		bLockState = false;

	if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'l') {
		lockflag = true;
	}

	status = viOpenDefaultRM(&rm);
	if (status < VI_SUCCESS) goto error;

	status = viOpen(rm, "USB::0x0699::0x0405::C025165::INSTR", NULL, NULL, &vi);
	if (status < VI_SUCCESS) goto error;

	for (i = 1; i < 100; i++) {
		if (lockflag) {
			viLock(vi, VI_EXCLUSIVE_LOCK, VI_TMO_INFINITE, NULL, NULL);
			bLockState = true;
		}
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

		if (lockflag) {
			viUnlock(vi);
			bLockState = false;
		}
	}

	viClose(vi);
	viClose(rm);

	system("pause");
	return 0;

error:
	viStatusDesc(rm, status, string);
	fprintf(stderr, "Error: %s\n", (ViBuf)string);
	if (bLockState && vi != VI_NULL)
		viUnlock(vi);
	if (vi != VI_NULL)
		viClose(vi);
	if (rm != VI_NULL)
		viClose(rm);
	
	system("pause");
	return 0;
}

