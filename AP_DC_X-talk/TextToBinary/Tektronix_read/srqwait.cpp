// srqwait.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "visa.h"

int main(int argc, char* argv[])
{
	ViSession	rm, vi;
	ViStatus	status;
	char		string[256];
	ViUInt32	retCnt;
	int			i;
	ViUInt16	stb;
	ViEventType	eventType = 0;
	ViEvent		context = 0;

	status = viOpenDefaultRM(&rm);
	if (status < VI_SUCCESS) goto error;

	status = viOpen(rm, "USB::0x0699::0x0405::C025165::INSTR", NULL, NULL, &vi);
	if (status < VI_SUCCESS) goto error;

	status = viEnableEvent(vi, VI_EVENT_SERVICE_REQ, VI_QUEUE, VI_NULL);
	if (status < VI_SUCCESS) goto error;

	// Setup instrument 
	status = viWrite(vi, (ViBuf) ":DATA:ENCDG RIBINARY;SOURCE CH1;START 1;STOP 500;WIDTH 2", 56, &retCnt);
	if (status < VI_SUCCESS) goto error;
	status = viWrite(vi, (ViBuf) ":ACQUIRE:STOPAFTER SEQUENCE;REPET 0;STATE 0;MODE SAMPLE", 55, &retCnt);
	if (status < VI_SUCCESS) goto error;
	status = viWrite(vi, (ViBuf) "DESE 1;*ESE 1;*SRE 32", 21, &retCnt);
	if (status < VI_SUCCESS) goto error;

	// Do cause some srqs
	for (i = 0; i < 100; i++) {
		status = viWrite(vi, (ViBuf) "*CLS", 4, &retCnt);
		if (status < VI_SUCCESS) goto error;
		status = viWrite(vi, (ViBuf) ":ACQUIRE:STATE 1", 16, &retCnt);
		if (status < VI_SUCCESS) goto error;
		status = viWrite(vi, (ViBuf) "*OPC", 4, &retCnt);
		if (status < VI_SUCCESS) goto error;
		status = viWaitOnEvent(vi, VI_EVENT_SERVICE_REQ, 5000, &eventType, &context);
		if (status >= VI_SUCCESS) {
			printf("(%d) Received SRQ\n", i);
			viClose(context);
		}
		else {
			viStatusDesc(vi, status, string);
			printf("(%d) viWaitOnEvent Failed - \"%s\"\n", string);
		}
		viReadSTB(vi, &stb);
	}

	// Cleanup and exit
	status = viDisableEvent(vi, VI_EVENT_SERVICE_REQ, VI_QUEUE);
	if (status < VI_SUCCESS) goto error;

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
