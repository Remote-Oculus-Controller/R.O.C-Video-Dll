#include "stdafx.h"
#include "RTSPController.h"


// Constructor

RTSPController::RTSPController(unsigned int width , unsigned int height , std::vector<char *> addrs)
{
	// Set the decoder resolution and store the RTSP url(s)
	this->_width  = width;
	this->_height = height;
	this->_addrs  = addrs;
}


// Destructor
RTSPController::~RTSPController()
{
	// Nothing to do here
}

// Threaded function to handle clients
DWORD RTSPController::run()
{

}