#include "stdafx.h"
#include "unityAPI.h"

// Handler on the DLL application

extern CROCVideoDllApp theApp;

// Push RTSP stream addr

int WINAPI pushAddr(char * addrInput)
{
	char * addr = _strdup(addrInput);
	return 0;
}

// Set decoder resolution 

int WINAPI setResolution(unsigned int width, unsigned int height)
{
	if (width == 0 || height == 0)
		return false;
	theApp.width  = width;
	theApp.height = height;
	return true;
}

// Start all the RTSP streams

int WINAPI startRTSP(bool isTCP)
{
	return 0;
}

// Stop all the RTSP streams

int WINAPI stopRTSP()
{
	return 0;
}

// Get the actual state of the client with the id : id

bool WINAPI getClientStatus(int id)
{
	return true;
}

// Set the callback to execute after a frane is received 

void WINAPI setNewVideoFrameCallback(NewVideoFrameCallback callback)
{

}

// Set the callback to execute after a client status changes

void WINAPI setClientStatusChangeCallback(ClientStatusChangeCallback callback)
{

}