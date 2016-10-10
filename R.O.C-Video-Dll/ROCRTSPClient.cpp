#include "stdafx.h"
#include "ROCRTSPClient.h"

ROCRTSPClient* ROCRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL,int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum) 
{
	return new ROCRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

ROCRTSPClient::ROCRTSPClient(UsageEnvironment& env, char const* rtspURL, int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
	: RTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1) 
{
}

ROCRTSPClient::~ROCRTSPClient() 
{
}
