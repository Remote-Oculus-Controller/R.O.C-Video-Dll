// test-dll.cpp : Defines the entry point for the console application.
//


#include <unityAPI.h>

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>

using namespace std::chrono;
unsigned int frames;


void videoFrameCallback(int id, uint8_t * data, int width, int height)
{

	cv::Mat image(cv::Size(width, height), CV_8UC3 , data, cv::Mat::AUTO_STEP);
	if (id == 0)
		cv::imshow("Display window 0", image); // Show our image inside it.
	else
		cv::imshow("Display window 1", image); // Show our image inside it.
}

void clientStatusChangeCallback(int id, bool status)
{
	std::cout << "Client stattus changed " << "ID : " << id << " Status : " << status << std::endl;
}


int main()
{
	cv::namedWindow("Display window 0", cv::WINDOW_AUTOSIZE); // Create a window for display.
	cv::namedWindow("Display window 1", cv::WINDOW_AUTOSIZE); // Create a window for display.


	while (1)
	{
		std::string adress;
		std::cout << "Entrez l'adress RTSP 1 : ";
		std::cin >> adress;
		std::vector<char> char_array(adress.begin(), adress.end());
		char_array.push_back(0);
		pushAddr(&char_array[0]);

		adress.clear();
		adress = "";

		std::cout << "Entrez l'adress RTSP 2 : ";
		std::cin >> adress;
		std::vector<char> char_array2(adress.begin(), adress.end());
		char_array2.push_back(0);
		pushAddr(&char_array2[0]);


		setResolution(640, 480);
		setNewVideoFrameCallback(videoFrameCallback);
		setClientStatusChangeCallback(clientStatusChangeCallback);

		frames = 0;

		if (startRTSP(false) == 0)
			std::cout << "RTSP Started" << std::endl;
		else
			std::cout << "Error while starting RTSP" << std::endl;
	
		cv::waitKey(0); // Wait for a keystroke in the window

		if (stopRTSP() == 0)
			std::cout << "RTSP stopped" << std::endl;
		else
			std::cout << "Error while stopping RTSP" << std::endl;
		
		char k = 0;

		while (k != 'c' && k != 'q')
		{
			std::cout << "Press q to quit , c to start again." << std::endl;
			std::cin >> k;

			if (k == 'q')
				exit(0);
		}
	}

	return 0;
}

