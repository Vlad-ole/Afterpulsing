#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <visa.h>

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

	assert(elements != NULL);

	status = viSetAttribute(vi, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);
	status = viSetAttribute(vi, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);

	// Turn headers off, this makes parsing easier
	status = viPrintf(vi, "header off\n");
	if (status < VI_SUCCESS) goto error;

	// Get record length value
	status = viQueryf(vi, "hor:reco?\n", "%ld", elements);
	if (status < VI_SUCCESS) goto error;

	// Make sure start, stop values for curve query match the full record length
	status = viPrintf(vi, "data:start 1;data:stop %d\n", *elements);
	if (status < VI_SUCCESS) goto error;

	// Get the yoffset to help calculate the vertical values.

	status = viQueryf(vi, "WFMOutpre:YOFF?\n", "%f", &yoffset);
	if (status < VI_SUCCESS) goto error;

	// Get the ymult to help calculate the vertical values.
	status = viQueryf(vi, "WFMOutpre:YMULT?\n", "%f", &ymult);
	if (status < VI_SUCCESS) goto error;

	// Request 8bit binary data on the curve query
	status = viPrintf(vi, "DATA:ENCDG RIBINARY;WIDTH 1\n");
	if (status < VI_SUCCESS) goto error;

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
	assert(c == '#');

	// Get width of element field.
	status = viScanf(vi, "%c", &c);
	if (status < VI_SUCCESS) goto error;
	assert(c >= '0' && c <= '9');

	// Read element characters
	count = c - '0';
	for (i = 0; i < count; i++) {
		status = viScanf(vi, "%c", &c);
		if (status < VI_SUCCESS) goto error;
		assert(c >= '0' && c <= '9');
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


// This program shows the performance effect of sizing buffers 
// with buffered I/O.
int main(int argc, char* argv[])
{
	ViSession		rm = VI_NULL, vi = VI_NULL;
	ViStatus		status;
	ViChar			buffer[256];
	double*			wfm = NULL;
	long			elements, i;
	ViUInt32		bufferSize = 10;
	unsigned long	start, total;

	// Open a default Session
	status = viOpenDefaultRM(&rm);
	if (status < VI_SUCCESS) goto error;

	// Open the gpib device at primary address 1, gpib board 8
	status = viOpen(rm, "GPIB8::1::INSTR", VI_NULL, VI_NULL, &vi);
	if (status < VI_SUCCESS) goto error;

	// Try buffer sizes 10, 100, ..., 10000 to show effect of 
	// buffer sizes on performance.
	for (bufferSize = 10; bufferSize <= 10000; bufferSize *= 10) {
		// Set new buffer size
		viSetBuf(vi, VI_READ_BUF, bufferSize);

		// Get Start time for benchmark
		start = time(NULL);

		// Loop several times
		for (i = 0; i < 5; i++) {
			wfm = ReadWaveform(vi, &elements);
		}

		// Print results
		total = time(NULL) - start;
		printf("bufSize %d, time %3.1fs\n", bufferSize, ((double)total) / 5.0);
	}

	// Clean up
	if (wfm != NULL) free(wfm);
	viClose(vi); // Not needed, but makes things a bit more understandable
	viClose(rm);

	return 0;

error:
	// Report error and clean up
	viStatusDesc(vi, status, buffer);
	fprintf(stderr, "failure: %s\n", buffer);
	if (rm != VI_NULL) viClose(rm);
	if (wfm != NULL) free(wfm);
	return 1;
}
