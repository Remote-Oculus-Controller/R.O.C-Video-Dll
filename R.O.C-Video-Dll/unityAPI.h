#pragma once

#ifndef UNITY_API
#define UNITY_API

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <Windows.h>
#include "R.O.C-Video-Dll.h"
#include "callback.h"

// Functions called from unity with C# FFI

extern "C" 
{
	int WINAPI pushAddr(char * addr);

	int WINAPI startRTSP(bool isTCP);
	int WINAPI stopRTSP();

	bool WINAPI getClientStatus(int id);
	bool WINAPI setResolution(unsigned int width, unsigned int height);

	void WINAPI setNewVideoFrameCallback(NewVideoFrameCallback callback);
	void WINAPI setClientStatusChangeCallback(ClientStatusChangeCallback callback);
}

#endif
