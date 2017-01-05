#include "stdafx.h"
#include "ROCSink.h"
#include "R.O.C-Video-Dll.h"
#include <iostream>


extern CROCVideoDllApp theApp;

int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}

// Implementation of "ROCSink":

// Even though we're not going to be doing anything with the incoming data, we still need to receive it.
// Define the size of the buffer that we'll use:
#define DUMMY_SINK_RECEIVE_BUFFER_SIZE 1024 * 1000

ROCSink* ROCSink::createNew(UsageEnvironment& env, MediaSubsession& subsession, int id , char const* streamId) {
	return new ROCSink(env, subsession, id , streamId);
}

ROCSink::ROCSink(UsageEnvironment& env, MediaSubsession& subsession, int id ,char const* streamId)
	: MediaSink(env),
	fSubsession(subsession) {
	fStreamId = strDup(streamId);
	fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
	fReceiveBufferAV = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE + 4];
	fReceiveBufferAV[0] = 0;
	fReceiveBufferAV[1] = 0;
	fReceiveBufferAV[2] = 0;
	fReceiveBufferAV[3] = 1;

	this->id = id;

	av_init_packet(&avpkt);
	avpkt.flags |= AV_PKT_FLAG_KEY;
	avpkt.pts = avpkt.dts = 0;

	/* set end of buffer to 0 (this ensures that no overreading happens for damaged mpeg streams) */
	memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

	//codec = avcodec_find_decoder(CODEC_ID_MPEG1VIDEO);
	codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!codec) {
		envir() << "codec not found!";
		exit(4);
	}

	c = avcodec_alloc_context3(codec);

	// No log
	av_log_set_level(0);
	picture = av_frame_alloc();


	if (codec->capabilities & CODEC_CAP_TRUNCATED) {
		c->flags |= CODEC_FLAG_TRUNCATED; // we do not send complete frames
	}

	//c->width = theApp.getWidth();
	//c->height = theApp.getHeight();
	c->pix_fmt = AV_PIX_FMT_YUV420P;

	/* for some codecs width and height MUST be initialized there becuase this info is not available in the bitstream */

	if (avcodec_open2(c, codec, NULL) < 0) {
		envir() << "could not open codec";
		exit(5);
	}


	
}

ROCSink::~ROCSink() {
	delete[] fReceiveBuffer;
	delete[] fStreamId;
}

void ROCSink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
	struct timeval presentationTime, unsigned durationInMicroseconds) {
	ROCSink* sink = (ROCSink*)clientData;
	sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

// If you don't want to see debugging output for each received frame, then comment out the following line:
//#define DEBUG_PRINT_EACH_RECEIVED_FRAME 1
/*
void ROCSink::r2sprop2() {
avpkt.data[0]	= 0;
avpkt.data[1]	= 0;
avpkt.data[2]	= 0;
avpkt.data[3]	= 1;
avpkt.data[4]	= 0x68;
avpkt.data[5]	= 0xeb;
avpkt.data[6]	= 0xe3;
avpkt.data[7]	= 0xcb;
avpkt.data[8]	= 0x22;
avpkt.data[9]	= 0xc0;
avpkt.size=10;
len = avcodec_decode_video2 (c, picture, &got_picture, &avpkt);
if (len < 0) {
//		envir() << "Error while decoding frame" << frame;
//		exit(6);
}
}
void ROCSink::r2sprop() {
avpkt.data[0]	= 0;
avpkt.data[1]	= 0;
avpkt.data[2]	= 0;
avpkt.data[3]	= 1;
avpkt.data[4]	= 0x67;
avpkt.data[5]	= 0x64;
avpkt.data[6]	= 0x00;
avpkt.data[7]	= 0x1e;
avpkt.data[8]	= 0xac;
avpkt.data[9]	= 0xd9;
avpkt.data[10]	= 0x40;
avpkt.data[11]	= 0xa0;
avpkt.data[12]	= 0x2f;
avpkt.data[13]	= 0xf9;
avpkt.data[14]	= 0x70;
avpkt.data[15]	= 0x11;
avpkt.data[16]	= 0x00;
avpkt.data[17]	= 0x00;
avpkt.data[18]	= 0x03;
avpkt.data[19]	= 0x03;
avpkt.data[20]	= 0xe8;
avpkt.data[21]	= 0x00;
avpkt.data[22]	= 0x00;
avpkt.data[23]	= 0xe9;
avpkt.data[24]	= 0xba;
avpkt.data[25]	= 0x8f;
avpkt.data[26]	= 0x16;
avpkt.data[27]	= 0x2d;
avpkt.data[28]	= 0x96;
avpkt.size=29;
len = avcodec_decode_video2 (c, picture, &got_picture, &avpkt);
if (len < 0) {
//		envir() << "Error while decoding frame" << frame;
//		exit(6);
}
}
*/


void ROCSink::setSprop(u_int8_t const* prop, unsigned size) {
	uint8_t *buf;
	uint8_t *buf_start;
	buf = (uint8_t *)malloc(1000);
	buf_start = buf + 4;

	avpkt.data = buf;
	avpkt.data[0] = 0;
	avpkt.data[1] = 0;
	avpkt.data[2] = 0;
	avpkt.data[3] = 1;
	memcpy(buf_start, prop, size);
	avpkt.size = size + 4;

	avcodec_send_packet(c, &avpkt);

	if (avcodec_receive_frame(c, picture) != 0) {
		envir() << "Error while decoding frame" << frame;
		//		exit(6);
	}

	envir() << "after setSprop\n";
	//	exit (111);
}

void ROCSink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
	struct timeval presentationTime, unsigned durationInMicroseconds) {
	// We've just received a frame of data.  (Optionally) print out information about it:
//#define DEBUG_PRINT_EACH_RECEIVED_FRAME
#ifdef DEBUG_PRINT_EACH_RECEIVED_FRAME
	if (fStreamId != NULL) envir() << "Stream \"" << fStreamId << "\"; ";
	envir() << fSubsession.mediumName() << "/" << fSubsession.codecName() << ":\tReceived " << frameSize << " bytes";
	if (numTruncatedBytes > 0) envir() << " (with " << numTruncatedBytes << " bytes truncated)";
	char uSecsStr[6 + 1]; // used to output the 'microseconds' part of the presentation time
	sprintf(uSecsStr, "%06u", (unsigned)presentationTime.tv_usec);
	envir() << ".\tPresentation time: " << (int)presentationTime.tv_sec << "." << uSecsStr;
	if (fSubsession.rtpSource() != NULL && !fSubsession.rtpSource()->hasBeenSynchronizedUsingRTCP()) {
		envir() << "!"; // mark the debugging output to indicate that this presentation time is not RTCP-synchronized
	}
#ifdef DEBUG_PRINT_NPT
	envir() << "\tNPT: " << fSubsession.getNormalPlayTime(presentationTime);
#endif
	envir() << "\n";
#endif
	if (strcmp(fSubsession.codecName(), "H264") == 0) {
		avpkt.data = fReceiveBufferAV;
		//	r2sprop();
		//	r2sprop2();
		//	avpkt.size = (int)fReceiveBuffer[0];
		avpkt.size = frameSize + 4;
		//	avpkt.size = frameSize;
		if (avpkt.size != 0) {
			memcpy(fReceiveBufferAV + 4, fReceiveBuffer, frameSize);
			avpkt.data = fReceiveBufferAV; 

			avcodec_send_packet(c, &avpkt);
			if (avcodec_receive_frame(c, picture) == 0) 
			{
				uint8_t * out = new uint8_t[theApp.getWidth() * theApp.getHeight() * 3];
				uint8_t * outData[1] = { out }; // RGB have one plane 
				int outLinesize[1] = { theApp.getWidth() * 3 }; // RGB32 Stride

				struct SwsContext *sws;
				
				std::cout << "Frame duration is : " << durationInMicroseconds << std::endl;

				sws = sws_getContext(picture->width,
					picture->height,
					AV_PIX_FMT_YUV420P,
					theApp.getWidth(),
					theApp.getHeight(),
					AV_PIX_FMT_RGB24,
					SWS_FAST_BILINEAR,
					NULL,
					NULL,
					NULL);
			

				sws_scale(sws,
					picture->data,
					picture->linesize,
					0,
					picture->height,
					outData,
					outLinesize);

				struct timeval now;

				gettimeofday(&now, NULL);

				theApp.getNewVideoFrameCallback()(this->id, out , theApp.getWidth(), theApp.getHeight());
				

				delete out;
				sws_freeContext(sws);
				frame++;
			}
			
				

				
			}
			else {
				envir() << "no picture :( !\n";
			}

	}

	// Then continue, to request the next frame of data:
	continuePlaying();
}

Boolean ROCSink::continuePlaying() {
	if (fSource == NULL) return False; // sanity check (should not happen)

									   // Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
	fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE,
		afterGettingFrame, this,
		onSourceClosure, this);
	return True;
}