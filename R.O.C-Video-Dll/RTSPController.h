#pragma once

class RTSPController
{
public:
	RTSPController();
	~RTSPController();


public:
	static DWORD WINAPI MyThreadFunction(void* pContext);
};

