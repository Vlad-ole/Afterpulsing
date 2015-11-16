#include <iostream>
#include <memory.h>
#include <visa.h>

// This example cycles through all GPIB devices with primary address 
// between 1 and 5 and prints out each instrument's *idn? query.

int main(int argc, char* argv[])
{
	ViSession	rm = VI_NULL, vi = VI_NULL;
	ViStatus	status;
	ViChar		desc[256], id[256], buffer[256];
	ViUInt32	retCnt, itemCnt;
	ViFindList	list;
	ViUInt32	i;

	// Open a default Session
	status = viOpenDefaultRM(&rm);
	if (status < VI_SUCCESS) goto error;

	// Find all GPIB devices
	status = viFindRsrc(rm, "GPIB?*INSTR{VI_ATTR_GPIB_PRIMARY_ADDR >= 1 && VI_ATTR_GPIB_PRIMARY_ADDR <= 5}", &list, &itemCnt, desc);
	//status = viFindRsrc(rm, "USB?*INSTR{}", &list, &itemCnt, desc); my correstion
	if (status < VI_SUCCESS) goto error;

	for (i = 0; i < itemCnt; i++) {
		// Open resource found in rsrc list
		status = viOpen(rm, desc, VI_NULL, VI_NULL, &vi);
		if (status <  VI_SUCCESS) goto error;

		// Send an ID query.
		status = viWrite(vi, (ViBuf) "*idn?", 5, &retCnt);
		if (status < VI_SUCCESS) goto error;

		// Clear the buffer and read the response
		memset(id, 0, sizeof(id));
		status = viRead(vi, (ViBuf)id, sizeof(id), &retCnt);
		if (status < VI_SUCCESS) goto error;

		// Print the response
		printf("id: %s: %s\n", desc, id);

		// We're done with this device so close it
		viClose(vi);

		// Get the next item
		viFindNext(list, desc);
	}

	// Clean up
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
