// R.O.C-Video-Dll.h : fichier d'en-tête principal pour la DLL R.O.C-Video-Dll
//

#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"		// symboles principaux
#include "default.h"
#include "unityAPI.h"
#include "callback.h"
#include "RTSPController.h"
#include <vector>
#include <Windows.h>
#include "ROCThread.h"

// CROCVideoDllApp
// Consultez R.O.C-Video-Dll.cpp pour l'implémentation de cette classe

class CROCVideoDllApp : public CWinApp
{
public:
	CROCVideoDllApp();

// Substitutions
public:

	virtual	BOOL InitInstance();
	virtual int  pushAddr(char * addr);
	virtual bool setResolution(unsigned int width, unsigned int height);
	virtual bool isStarted();
	virtual int start(bool isTCP);
	virtual int stop();
	virtual uint8_t * generateNewFrame();
	DWORD print();

	
	virtual void setNewVideoFrameCallback(NewVideoFrameCallback callback);
	virtual NewVideoFrameCallback getNewVideoFrameCallback();

	virtual void setClientStatusChangeCallback(ClientStatusChangeCallback callback);
	virtual ClientStatusChangeCallback getClientStatusChangeCallback();

private:

	std::vector<char *> _addrs;
	bool				_isStarted;
	unsigned int		_width;
	unsigned int		_height;

	RTSPController		_controller;

	void(*_newVideoFrameCallback)(int id, uint8_t* data, int width, int height);
	void(*_clientStatusChangeCallback)(int id, bool status);

	Thread<CROCVideoDllApp> * thread;

	DECLARE_MESSAGE_MAP()
};


