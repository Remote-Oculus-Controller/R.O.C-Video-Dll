#include "stdafx.h"
#include "RTSPController.h"
#include "RTSPApi.h"
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

extern CROCVideoDllApp theApp;

// FFMPEG
extern "C" {

#ifdef HAVE_AV_CONFIG_H
#undef HAVE_AV_CONFIG_H
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

volatile char eventLoopWatchVariable = 0;

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
	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

	// avcodec init
	avcodec_register_all();
	av_register_all();

	//  Open and start streaming each adress 
	for (int i = 0; i <= this->_addrs.size() - 1 ; ++i) {
		openURL(*env, NULL , this->_addrs[i] , i);
	}

	// All subsequent activity takes place within the event loop
	env->taskScheduler().doEventLoop(&eventLoopWatchVariable);

	// Cleanup

	delete scheduler;
	env->reclaim(); 

	theApp.getClientStatusChangeCallback()(-1, false);

	return 0;
}