#pragma once

#ifndef ROCRTSPCLIENT_H
#define ROCRTSPCLIENT_H

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "StreamClientState.h"

class ROCRTSPClient : public RTSPClient 
{
public:
	static ROCRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL, int verbosityLevel = 0, char const* applicationName = NULL, portNumBits tunnelOverHTTPPortNum = 0);

protected:
	ROCRTSPClient(UsageEnvironment& env, char const* rtspURL, int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
	virtual ~ROCRTSPClient();

public:
	StreamClientState scs;
};

#endif