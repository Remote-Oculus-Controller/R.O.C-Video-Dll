// test-dll.cpp : Defines the entry point for the console application.
//


#include <unityAPI.h>

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>

using namespace std::chrono;
unsigned int frames;

high_resolution_clock::time_point start;

void videoFrameCallback(int id, uint8_t * data, int width, int height)
{
	//std::cout << "ID : " << id << std::endl;
	if (id != 0)
	{
	//	std::cout << "Not from ID 0 , throwing..." << std::endl;
		return;
	}

	frames++;
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - start);

	//std::cout << "Average Presentation time : " << time_span.count() << std::endl;

	cv::Mat image(cv::Size(width, height), CV_8UC3, data, cv::Mat::AUTO_STEP);
	cv::imshow("Display window", image); // Show our image inside it.
	start = high_resolution_clock::now();
}

void clientStatusChangeCallback(int id, bool status)
{
	std::cout << "Client stattus changed " << "ID : " << id << " Status : " << status << std::endl;
}


int main()
{
	pushAddr("rtsp://192.168.1.89:5554/playlist.m3u");
	//pushAddr("rtsp://192.168.1.89:5554/playlist.m3u");
	setResolution(640, 480);
	setNewVideoFrameCallback(videoFrameCallback);
	setClientStatusChangeCallback(clientStatusChangeCallback);

	start = high_resolution_clock::now();
	frames = 0;

	if (startRTSP(false) == 0)
		std::cout << "RTSP Started" << std::endl;
	else
		std::cout << "Error while starting RTSP" << std::endl;

	cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE); // Create a window for display.
	cv::waitKey(0); // Wait for a keystroke in the window

	if (stopRTSP() == 0)
		std::cout << "RTSP stopped" << std::endl;
	else
		std::cout << "Error while stopping RTSP" << std::endl;
	
	cv::waitKey(0);

	return 0;
}

