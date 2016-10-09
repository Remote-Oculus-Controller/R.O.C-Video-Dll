// R.O.C-Video-Dll.cpp : définit les routines d'initialisation pour la DLL.
//

#include "stdafx.h"
#include "R.O.C-Video-Dll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CROCVideoDllApp

BEGIN_MESSAGE_MAP(CROCVideoDllApp, CWinApp)
END_MESSAGE_MAP()


// construction CROCVideoDllApp

CROCVideoDllApp::CROCVideoDllApp()
{
	// Set the default resolutions.
	this->_height = DEFAULT_WIDTH;
	this->_width  = DEFAULT_HEIGHT;

	// Set the default callback value to NULL
	this->_newVideoFrameCallback		= NULL;
	this->_clientStatusChangeCallback	= NULL;

	// Set the default Thread , Controller and state value
	// to NULL , NULL and false
	this->_thread = NULL;
	this->_controller = NULL;
	this->_isStarted = false;
}


// Seul et unique objet CROCVideoDllApp

CROCVideoDllApp theApp;


// initialisation de CROCVideoDllApp

BOOL CROCVideoDllApp::InitInstance()
{
	// Basic instance initialisation for DLL object.
	CWinApp::InitInstance();
	return TRUE;
}


int CROCVideoDllApp::pushAddr(char * addr)
{
	// Push the received RTSP url to the vector.
	this->_addrs.push_back(addr);
	// Return the ID of the client which adress is : addr
	return this->_addrs.size() - 1;
}

bool CROCVideoDllApp::isStarted()
{
	// Return the current state of the clients.
	return this->_isStarted;
}

int CROCVideoDllApp::start(bool isTCP)
{
	// Do nothing if any of the callback is not set or if the clients are already running.
	if (_clientStatusChangeCallback == NULL || _newVideoFrameCallback == NULL || this->_isStarted == true)
		return 1;
	
	// Allocate a new RTSPController to handle the RTSP streams.
	this->_controller = new RTSPController(this->_width, this->_height, this->_addrs);
	
	// If allocation failed exit.
	if (this->_controller == NULL)
		return 1;
	
	// Allocate a new Thread Handler for the asynchronous reception of the RTSP streams.
	this->_thread = new  Thread<RTSPController>(this->_controller , &RTSPController::run);

	// If allocation failed free _controller and exit.
	if (this->_thread == NULL)
	{
		delete this->_controller;
		return 1;
	}
	
	// Start the Thread Handler , if no error occured set _isStarted and return.
	if (this->_thread->start()) 
	{
		this->_isStarted = true;
		return 0;
	}

	// Otherwise clear the Thread Handler and the RTSPController then exit.
	delete this->_thread;
	delete this->_controller;

	return 1;
}

int CROCVideoDllApp::stop()
{
	// If the clients are not running then exit
	if (this->_isStarted == false)
		return 1;

	// If the thread is running (security check) then wait for it to finish
	// Then clean any remaining data.
	if (this->_thread) {
		this->_thread->join();
		delete this->_thread;
		delete this->_controller;
	}
	// Set the new state of the clients as not running.
	this->_isStarted = false;
	return 0;
}

bool CROCVideoDllApp::setResolution(unsigned int width, unsigned int height)
{
	// If the resolutions are not valid then exit
	if (width == 0 || height == 0)
		return false;

	// Otherwise set them and return.
	this->_width	= width;
	this->_height	= height;

	return true;
}

void CROCVideoDllApp::setNewVideoFrameCallback(NewVideoFrameCallback callback)
{
	// Set the NewVideoFrameCallback
	this->_newVideoFrameCallback = callback;
}

NewVideoFrameCallback CROCVideoDllApp::getNewVideoFrameCallback()
{
	// Get the NewVideoFrameCallback
	return this->_newVideoFrameCallback;
}

void CROCVideoDllApp::setClientStatusChangeCallback(ClientStatusChangeCallback callback)
{
	// Set the ClientStatusChangeCallback
	this->_clientStatusChangeCallback = callback;
}

ClientStatusChangeCallback CROCVideoDllApp::getClientStatusChangeCallback()
{
	// Get the ClientStatusChangeCallback
	return this->_clientStatusChangeCallback;
}