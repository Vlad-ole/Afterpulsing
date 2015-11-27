//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <visa.h>
#include <string>


// This function reads the currently selected waveform and returns
// it as an array of doubles.
double* ReadWaveform(ViSession vi, long* elements) {
	ViStatus	status;
	float		yoffset, ymult;
	ViChar		buffer[256];
	ViChar		c;
	long		count, i;
	double*		ptr = NULL;
	unsigned char		szBuff[256] = { 0, };

	//assert(elements != NULL);

	status = viSetAttribute(vi, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);
	status = viSetAttribute(vi, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);

	// Turn headers off, this makes parsing easier
	status = viPrintf(vi, "header off\n");
	if (status < VI_SUCCESS) goto error;

	// Get record length value
	status = viQueryf(vi, "hor:reco?\n", "%ld", elements);
	if (status < VI_SUCCESS) goto error;

	//*elements=1000000;
	status = viPrintf(vi, "hor:reco %ld\n", *elements);
	if (status < VI_SUCCESS) goto error;


	// Make sure start, stop values for curve query match the full record length
	status = viPrintf(vi, "data:start 1");
	if (status < VI_SUCCESS) goto error;

	// Make sure start, stop values for curve query match the full record length
	status = viPrintf(vi, "data:stop %d\n", *elements);
	if (status < VI_SUCCESS) goto error;

	status = viPrintf(vi, "DATA:SOURCE CH1\n");
	if (status < VI_SUCCESS) goto error;


	// Get the yoffset to help calculate the vertical values.

	status = viQueryf(vi, "WFMOutpre:YOFF?\n", "%f", &yoffset);
	if (status < VI_SUCCESS) goto error;

	// Get the ymult to help calculate the vertical values.
	status = viQueryf(vi, "WFMOutpre:YMULT?\n", "%f", &ymult);
	if (status < VI_SUCCESS) goto error;

	// Request 8bit binary data on the curve query

	if (status < VI_SUCCESS) goto error;

	printf("%d\n", *elements);




	// Request the curve
	status = viPrintf(vi, "CURVE?\n");
	if (status < VI_SUCCESS) goto error;

	// Always flush if a viScanf follows a viPrintf or viBufWrite.
	status = viFlush(vi, VI_WRITE_BUF | VI_READ_BUF_DISCARD);
	if (status < VI_SUCCESS) goto error;

	// Get first char and validate
	status = viSetAttribute(vi, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_DISABLE);
	status = viScanf(vi, "%c", &c);
	if (status < VI_SUCCESS) goto error;
	//assert(c == '#');

	printf("%c", c);

	// Get width of element field.
	status = viScanf(vi, "%c", &c);
	if (status < VI_SUCCESS) goto error;
	//assert(c >= '0' && c <= '9');

	printf("%c", c);

	// Read element characters
	count = c - '0';
	for (i = 0; i < count; i++) {
		status = viScanf(vi, "%c", &c);
		if (status < VI_SUCCESS) goto error;

		printf("%c", c);

		//assert(c >= '0' && c <= '9');
	}

	// Read waveform into allocated storage
	ptr = (double*)malloc(*elements*sizeof(double));

	for (i = 0; i < *elements; i++) {
		status = viScanf(vi, "%c", &c);
		if (status < VI_SUCCESS) goto error;
		ptr[i] = (((double)c) - yoffset) * ymult;
	}

	status = viFlush(vi, VI_WRITE_BUF | VI_READ_BUF_DISCARD);
	if (status < VI_SUCCESS) goto error;

	return ptr;

error:
	// Report error and clean up
	viStatusDesc(vi, status, buffer);
	fprintf(stderr, "failure: %s\n", buffer);
	if (ptr != NULL) free(ptr);
	return NULL;
}


int main(int argc, char* argv[])
{
	ViSession	rm = VI_NULL, vi = VI_NULL;
	ViStatus	status;
	ViChar		buffer[256];
	double*		wfm = NULL;
	long		elements, i;



	// Open a default Session
	status = viOpenDefaultRM(&rm);
	if (status < VI_SUCCESS) goto error;

	// Open the gpib device at primary address 1, gpib board 8
	status = viOpen(rm, "TCPIP::192.168.255.5::INSTR", VI_NULL, VI_NULL, &vi);
	if (status < VI_SUCCESS) goto error;
	/*long analogchcount=0;
	status = viQueryf(vi, (ViString)"CONFIGuration:ANALOg:NUMCHANnels?", "%ld", &analogchcount);
	if (status < VI_SUCCESS) goto error;	*/

	// Read waveform and write it to stdout
	wfm = ReadWaveform(vi, &elements);
	if (wfm != NULL) {
		for (i = 0; i < elements; i++) {
			printf("%f\n", wfm[i]);
			;
		}
	}

	// Clean up
	if (wfm != NULL) free(wfm);
	viClose(vi); // Not needed, but makes things a bit more understandable
	viClose(rm);
	char c;
	scanf_s("%c", &c);
	return 0;

error:
	// Report error and clean up
	viStatusDesc(vi, status, buffer);
	fprintf(stderr, "failure: %s\n", buffer);
	if (rm != VI_NULL) viClose(rm);
	if (wfm != NULL) free(wfm);
	return 1;
}