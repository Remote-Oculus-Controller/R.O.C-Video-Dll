#include "stdafx.h"
#include "ROCRTSPClient.h"
#include "RTSPApi.h"
#include <iostream>

ROCRTSPClient* ROCRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL, int id , int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum) 
{
	return new ROCRTSPClient(env, rtspURL, id , verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

ROCRTSPClient::ROCRTSPClient(UsageEnvironment& env, char const* rtspURL, int id , int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
	: RTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1) 
{
	this->id = id;
}

ROCRTSPClient::~ROCRTSPClient() 
{
	std::cout << "I am dyiiiiiiiiiiiing" << std::endl;
}
