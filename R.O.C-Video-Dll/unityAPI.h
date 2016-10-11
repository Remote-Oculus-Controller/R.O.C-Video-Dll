#pragma once

#ifndef UNITY_API
#define UNITY_API

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <Windows.h>
#include "callback.h"

// Functions called from unity with C# FFI

extern "C" 
{
	// Called to push an RTSP url
	int WINAPI pushAddr(char * addr);

	// Start the client(s)
	int WINAPI startRTSP(bool isTCP);

	// Stop the client(s)
	int WINAPI stopRTSP();

	// Get the status of the client with id : id
	bool WINAPI getClientStatus(int id);

	// Set the decoder resolution
	bool WINAPI setResolution(unsigned int width, unsigned int height);

	// Set the callback to call when a frame is received
	void WINAPI setNewVideoFrameCallback(NewVideoFrameCallback callback);

	// Set the callback to call when a client status changes
	void WINAPI setClientStatusChangeCallback(ClientStatusChangeCallback callback);
}

#endif
