#pragma once

#ifndef UNITY_API
#define UNITY_API

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <Windows.h>
#include "R.O.C-Video-Dll.h"


// Callback called on new frame for unity

using NewVideoFrameCallback = void(*)(int id, uint8_t* data, int width, int height);

// Callback call on client status change for unity

using ClientStatusChangeCallback = void(*)(int id, bool status);


// Functions called from unity with C# FFI

extern "C" 
{
	int WINAPI pushAddr(char * addr);
	int WINAPI setResolution(unsigned int width, unsigned int height);

	int WINAPI startRTSP(bool isTCP);
	int WINAPI stopRTSP();

	bool WINAPI getClientStatus(int id);

	void WINAPI setNewVideoFrameCallback(NewVideoFrameCallback);
	void WINAPI setClientStatusChangeCallback(ClientStatusChangeCallback);
}

#endif