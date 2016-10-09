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

	// Performs basic instance initialisation
	virtual	BOOL InitInstance();

	// Push an RTSP url 
	virtual int  pushAddr(char * addr);

	// Set the decoder resolution
	virtual bool setResolution(unsigned int width, unsigned int height);

	// Get the state of the clients
	virtual bool isStarted();

	// Start the clients using TCP (true) or UDP (false)
	virtual int start(bool isTCP);

	// Stop the clients
	virtual int stop();

	// Set / Get the callback called when a new frame is ready
	virtual void setNewVideoFrameCallback(NewVideoFrameCallback callback);
	virtual NewVideoFrameCallback getNewVideoFrameCallback();

	// Set / Get the callback called when a client status changes
	virtual void setClientStatusChangeCallback(ClientStatusChangeCallback callback);
	virtual ClientStatusChangeCallback getClientStatusChangeCallback();

private:

	// Vector of RTSP url(s)
	std::vector<char *> _addrs;
	
	// State of the Clients
	bool				_isStarted;

	// Decoder resolution
	unsigned int		_width;
	unsigned int		_height;

	// RTSPController and his Thread Handler
	RTSPController			*	_controller;
	Thread<RTSPController>	*	_thread;

	// Callbacks to communicate with unity
	void(*_newVideoFrameCallback)(int id, uint8_t* data, int width, int height);
	void(*_clientStatusChangeCallback)(int id, bool status);


	DECLARE_MESSAGE_MAP()
};


