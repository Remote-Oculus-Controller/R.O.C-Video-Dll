#pragma once

#include <vector>

class RTSPController
{
public:
	
	// Constructor
	RTSPController(unsigned int width , unsigned int height , std::vector<char *> addrs);
	
	// Destructor
	~RTSPController();

public:

	// Threaded function to handle clients.
	DWORD run();

private:

	// Decoder Resolution
	unsigned int _width;
	unsigned int _height;

	// Vector of RTSP url(s)
	std::vector<char *> _addrs;
};

