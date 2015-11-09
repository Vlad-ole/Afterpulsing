#include <stdio.h>
#include <memory.h>
#include <visa.h>

// This example opens a specific GPIB device, does an *idn query
// and prints the result.

int main(int argc, char* argv[])
{
	ViSession	rm = VI_NULL, vi = VI_NULL;
	ViStatus	status;
	ViChar		buffer[256];
	ViUInt32	retCnt;

	// Open a default Session
	status = viOpenDefaultRM(&rm);
	if (status < VI_SUCCESS) goto error;

	// Open the gpib device at primary address 1, gpib board 8
	status = viOpen(rm, "USB::0x0699::0x0405::C025165::INSTR", VI_NULL, VI_NULL, &vi);
	if (status < VI_SUCCESS) goto error;

	// Send an ID query.
	status = viWrite(vi, (ViBuf) "*idn?", 5, &retCnt);
	if (status < VI_SUCCESS) goto error;

	// Clear the buffer and read the response
	memset(buffer, 0, sizeof(buffer));
	status = viRead(vi, (ViBuf)buffer, sizeof(buffer), &retCnt);
	if (status < VI_SUCCESS) goto error;

	// Print the response
	printf("id: %s\n", buffer);

	// Clean up
	viClose(vi); // Not needed, but makes things a bit more understandable
	viClose(rm);

	return 0;

error:
	// Report error and clean up
	viStatusDesc(vi, status, buffer);
	fprintf(stderr, "failure: %s\n", buffer);
	if (rm != VI_NULL) {
		viClose(rm);
	}
	return 1;
}
