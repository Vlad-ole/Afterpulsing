// srq.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "visa.h"

ViStatus _VI_FUNCH ServiceReqEventHandler(ViSession vi, ViEventType eventType, ViEvent event, ViAddr userHandle)
{
	printf("srq occurred\n");
	return VI_SUCCESS;
}

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

	// Setup and enable event handler
	status = viInstallHandler(vi, VI_EVENT_SERVICE_REQ, ServiceReqEventHandler, NULL);
	if (status < VI_SUCCESS) goto error;
	status = viEnableEvent(vi, VI_EVENT_SERVICE_REQ, VI_HNDLR, VI_NULL);
	if (status < VI_SUCCESS) goto error;

	// Setup instrument 
	status = viWrite(vi, (ViBuf) ":RECALL:SETUP FACTORY", 21, &retCnt);
	if (status < VI_SUCCESS) goto error;
	status = viWrite(vi, (ViBuf) ":SELECT:CH1 1;CH2 0;CH3 0;CH4 0", 31, &retCnt);
	if (status < VI_SUCCESS) goto error;
	status = viWrite(vi, (ViBuf) ":DATA:ENCDG RIBINARY;SOURCE CH1;START 1;STOP 500;WIDTH 2", 56, &retCnt);
	if (status < VI_SUCCESS) goto error;
	status = viWrite(vi, (ViBuf) ":ACQUIRE:STOPAFTER SEQUENCE;REPET 0;STATE 0;MODE SAMPLE", 55, &retCnt);
	if (status < VI_SUCCESS) goto error;
	status = viWrite(vi, (ViBuf) "DESE 1;*ESE 1;*SRE 32", 21, &retCnt);
	if (status < VI_SUCCESS) goto error;

	// Do a single acq
	status = viWrite(vi, (ViBuf) "*CLS", 4, &retCnt);
	if (status < VI_SUCCESS) goto error;
	status = viWrite(vi, (ViBuf) ":ACQUIRE:STATE 1", 16, &retCnt);
	if (status < VI_SUCCESS) goto error;
	status = viWrite(vi, (ViBuf) "*OPC", 4, &retCnt);
	if (status < VI_SUCCESS) goto error;

	// Wait around long enough for srq event to occur
	::Sleep(10000);

	// Cleanup and exit
	status = viDisableEvent(vi, VI_EVENT_SERVICE_REQ, VI_HNDLR);
	if (status < VI_SUCCESS) goto error;
	status = viUninstallHandler(vi, VI_EVENT_SERVICE_REQ, ServiceReqEventHandler, NULL);
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

