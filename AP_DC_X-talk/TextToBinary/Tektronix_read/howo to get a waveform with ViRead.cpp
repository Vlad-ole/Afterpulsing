#include <visa.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static ViChar curve[20000];

void main(void)
{

	ViSession id, RM;
	ViStatus retErr;
	ViUInt32 retCnt, ii;
	ViChar cmd[256], rcv[256], idn[256];

	viOpenDefaultRM(&RM);
	retErr = viOpen(RM, "USB::0x0699::0x0405::C025165::INSTR", VI_NULL,
		VI_NULL, &id);
	if (retErr < VI_SUCCESS) 
	{
		viStatusDesc(RM, retErr, rcv); rcv[strlen(rcv)] = '\0';
		printf("%s\n", rcv);
		getchar();
		exit(0);
	}

	sprintf(cmd, "*IDN?");
	retErr = viWrite(id, (ViPBuf)cmd, strlen(cmd), &retCnt);
	if (retErr < VI_SUCCESS) 
	{
		viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
		printf("%s\n", rcv);
		getchar();
	}

	retErr = viRead(id, (ViPBuf)rcv, 256, &retCnt);
	if (retErr < VI_SUCCESS) 
	{
		viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
		printf("%s\n", rcv);
		getchar();
	}
	else
	{
		rcv[retCnt] = '\0';
		printf("%s\n", rcv);
	}

	sprintf(cmd, "DATA:SOURCE CH1");
	retErr = viWrite(id, (ViPBuf)cmd, strlen(cmd), &retCnt);
	if (retErr < VI_SUCCESS) 
	{
		viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
		printf("%s\n", rcv);
		getchar();
	}

	sprintf(cmd, "DATA:START 1");
	retErr = viWrite(id, (ViPBuf)cmd, strlen(cmd), &retCnt);
	if (retErr < VI_SUCCESS) 
	{
		viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
		printf("%s\n", rcv);
		getchar();
	}

	sprintf(cmd, "DATA:STOP 500");
	retErr = viWrite(id, (ViPBuf)cmd, strlen(cmd), &retCnt);
	if (retErr < VI_SUCCESS) 
	{
		viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
		printf("%s\n", rcv);
		getchar();
	}

	sprintf(cmd, "CURVEStream?");
	retErr = viWrite(id, (ViPBuf)cmd, strlen(cmd), &retCnt);
	if (retErr < VI_SUCCESS) 
	{
		viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
		printf("%s\n", rcv);
		getchar();
	}
	//Get the next 5 buffers of data...
	viSetAttribute(id, VI_ATTR_TMO_VALUE, 30000L);
	for (ii = 0; ii < 5; ++ii) {
		retErr = viRead(id, (ViPBuf)curve, 100000, &retCnt);
		if (retErr < VI_SUCCESS) 
		{
			viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
			printf("%s\n", rcv);
			getchar();
			ii = 5;
		}
		else
		{
			printf("Got a %d\n buffer full of data.\n", ii + 1);

		}
	}
	viSetAttribute(id, VI_ATTR_TMO_VALUE, 10000L);
	//Get the instrument back to a talker/listener...
	retErr = viClear(id);

	//Check for errors (use error_query in the driver..or whatever the call is.
	sprintf(cmd, "*esr?");
	retErr = viWrite(id, (ViPBuf)cmd, strlen(cmd), &retCnt);
	if (retErr < VI_SUCCESS) 
	{
		viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
		printf("%s\n", rcv);
		getchar();
	}

	retErr = viRead(id, (ViPBuf)curve, 10000, &retCnt);
	if (retErr < VI_SUCCESS) 
	{
		viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
		printf("%s\n", rcv);
		getchar();
	}
	else if (rcv[0] != '0')
	{
		sprintf(cmd, "allev?");
		retErr = viWrite(id, (ViPBuf)cmd, strlen(cmd), &retCnt);
		if (retErr < VI_SUCCESS) 
		{
			viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
			printf("%s\n", rcv);
			getchar();
		}

		retErr = viRead(id, (ViPBuf)rcv, 255, &retCnt);
		if (retErr < VI_SUCCESS) 
		{
			viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
			printf("%s\n", rcv);
			getchar();
		}

	}
	//Get the ID again to be sure all is well again...
	sprintf(cmd, "*idn?");
	retErr = viWrite(id, (ViPBuf)cmd, strlen(cmd), &retCnt);
	if (retErr < VI_SUCCESS)
	{
		viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
		printf("%s\n", rcv);
		getchar();
	}

	retErr = viRead(id, (ViPBuf)idn, 255, &retCnt);
	if (retErr < VI_SUCCESS)
	{
		viStatusDesc(id, retErr, rcv); rcv[strlen(rcv)] = '\0';
		printf("%s\n", rcv);
		getchar();
	}
	else
	{
		rcv[retCnt] = '\0';
		printf("%s\nAll is ok now again...\n", idn);
	}


	viClose(RM);
	getchar();

}