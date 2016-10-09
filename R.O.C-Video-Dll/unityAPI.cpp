#include "stdafx.h"
#include "unityAPI.h"

// Handler on the DLL application

extern CROCVideoDllApp theApp;


// Push RTSP url

int WINAPI pushAddr(char * addrInput)
{
	if (addrInput == NULL)
		return -1;

	char * addr = _strdup(addrInput);
	return theApp.pushAddr(addr);
}

// Set decoder resolution 

bool WINAPI setResolution(unsigned int width, unsigned int height)
{
	return theApp.setResolution(width, height);
}

// Start all the RTSP streams

int WINAPI startRTSP(bool isTCP)
{
	return theApp.start(isTCP);
}

// Stop all the RTSP streams

int WINAPI stopRTSP()
{
	return theApp.stop();
}

// Get the actual state of the client with the id : id

bool WINAPI getClientStatus(int id)
{
	return true;
}

// Set the callback to execute after a frane is received 

void WINAPI setNewVideoFrameCallback(NewVideoFrameCallback callback)
{
	theApp.setNewVideoFrameCallback(callback);
}

// Set the callback to execute after a client status changes

void WINAPI setClientStatusChangeCallback(ClientStatusChangeCallback callback)
{
	theApp.setClientStatusChangeCallback(callback);
}