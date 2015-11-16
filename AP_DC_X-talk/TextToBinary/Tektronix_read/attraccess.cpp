#include <iostream>
#include <memory.h>
#include <visa.h>

// This example gets some info about the visa implementation,
// opens a specific GPIB device, sets the timeout to 5 seconds, and
// does an *idn query then prints the result.

int main(int argc, char* argv[])
{
	ViSession	rm = VI_NULL, vi = VI_NULL;
	ViStatus	status;
	ViChar		buffer[256];
	ViUInt32	retCnt;
	ViVersion	version = 0, impl = 0;

	// Open a default Session
	status = viOpenDefaultRM(&rm);
	if (status < VI_SUCCESS) goto error;

	// Get and print Visa vendor's name, Visa Specification Version, and Implementation Version.
	status = viGetAttribute(rm, VI_ATTR_RSRC_MANF_NAME, buffer);
	if (status < VI_SUCCESS) goto error;
	status = viGetAttribute(rm, VI_ATTR_RSRC_SPEC_VERSION, &version);
	if (status < VI_SUCCESS) goto error;
	status = viGetAttribute(rm, VI_ATTR_RSRC_IMPL_VERSION, &impl);
	if (status < VI_SUCCESS) goto error;
	printf("VISA Manufacturer Name: %s, supports %x spec, %x implimentation version\n", buffer, version, impl);

	// Open the gpib device at primary address 1, gpib board 8
	status = viOpen(rm, "USB::0x0699::0x0405::C025165::INSTR", VI_NULL, VI_NULL, &vi);
	if (status < VI_SUCCESS) goto error;

	// Set timeout to 5 seconds
	status = viSetAttribute(vi, VI_ATTR_TMO_VALUE, 5000);
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

	system("pause");
	return 0;

error:
	// Report error and clean up
	viStatusDesc(vi, status, buffer);
	fprintf(stderr, "failure: %s\n", buffer);
	if (rm != VI_NULL) {
		viClose(rm);
	}

	system("pause");
	return 1;
}
