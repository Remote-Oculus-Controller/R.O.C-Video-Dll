#include "stdafx.h"
#include "RTSPApi.h"
#include "ROCRTSPClient.h"
#include "StreamClientState.h"
#include "ROCSink.h"
#include <stdint.h>


extern CROCVideoDllApp theApp;


void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL , int id) {
	// Begin by creating a "RTSPClient" object.  Note that there is a separate "RTSPClient" object for each stream that we wish
	// to receive (even if more than stream uses the same "rtsp://" URL).
	RTSPClient* rtspClient = ROCRTSPClient::createNew(env, rtspURL, id ,  0 /* RTSP_CLIENT_VERBOSITY_LEVEL */ , progName);
	if (rtspClient == NULL) {
		env << "Failed to create a RTSP client for URL \"" << rtspURL << "\": " << env.getResultMsg() << "\n";
		theApp.getClientStatusChangeCallback()(8, false);
		return;
	}

	env << "Created new client for URL " << rtspURL << "\n";
	++rtspClientCount;

	// Next, send a RTSP "DESCRIBE" command, to get a SDP description for the stream.
	// Note that this command - like all RTSP commands - is sent asynchronously; we do not block, waiting for a response.
	// Instead, the following function call returns immediately, and we handle the RTSP response later, from within the event loop:
	rtspClient->sendDescribeCommand(continueAfterDESCRIBE);
}


// Implementation of the RTSP 'response handlers':

void continueAfterDESCRIBE(RTSPClient * rtspClient, int resultCode, char* resultString) {
	do {
		UsageEnvironment& env = rtspClient->envir(); // alias
		StreamClientState& scs = ((ROCRTSPClient*)rtspClient)->scs; // alias
		

		if (resultCode != 0) {
			env << "Failed to get a SDP description: " << resultString << "\n";
			theApp.getClientStatusChangeCallback()(((ROCRTSPClient*)rtspClient)->id, false);
			delete[] resultString;
			break;
		}

		char* const sdpDescription = resultString;
		env << "Got a SDP description:\n" << sdpDescription << "\n";

		// Create a media session object from this SDP description:
		scs.session = MediaSession::createNew(env, sdpDescription);
		delete[] sdpDescription; // because we don't need it anymore
		if (scs.session == NULL) {
			theApp.getClientStatusChangeCallback()(((ROCRTSPClient*)rtspClient)->id, false);
			env << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\n";
			break;
		}
		else if (!scs.session->hasSubsessions()) {
			theApp.getClientStatusChangeCallback()(((ROCRTSPClient*)rtspClient)->id, false);
			env << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
			break;
		}

		// Then, create and set up our data source objects for the session.  We do this by iterating over the session's 'subsessions',
		// calling "MediaSubsession::initiate()", and then sending a RTSP "SETUP" command, on each one.
		// (Each 'subsession' will have its own data source.)
		scs.iter = new MediaSubsessionIterator(*scs.session);
		setupNextSubsession(rtspClient);
		return;
	} while (0);

	// An unrecoverable error occurred with this stream.
	shutdownStream(rtspClient);
}

// By default, we request that the server stream its data using RTP/UDP.
// If, instead, you want to request that the server stream via RTP-over-TCP, change the following to True:
#define REQUEST_STREAMING_OVER_TCP false

void setupNextSubsession(RTSPClient* rtspClient) {
	UsageEnvironment& env = rtspClient->envir(); // alias
	StreamClientState& scs = ((ROCRTSPClient*)rtspClient)->scs; // alias

	scs.subsession = scs.iter->next();
	if (scs.subsession != NULL) {
		if (!scs.subsession->initiate()) {
			theApp.getClientStatusChangeCallback()(((ROCRTSPClient*)rtspClient)->id, false);
			env << "Failed to initiate the \"" <<  "\" subsession: " << env.getResultMsg() << "\n";
			setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
		}
		else {
			env << "Initiated the \""  << "\" subsession (client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum() + 1 << ")\n";

			// Continue setting up this subsession, by sending a RTSP "SETUP" command:
			rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, REQUEST_STREAMING_OVER_TCP);
		}
		return;
	}

	// We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
	if (scs.session->absStartTime() != NULL) {
		// Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
		rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime());
	}
	else {
		scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
		rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
	}
}

void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString) {
	do {
		UsageEnvironment& env = rtspClient->envir(); // alias
		StreamClientState& scs = ((ROCRTSPClient*)rtspClient)->scs; // alias

		if (resultCode != 0) {
			theApp.getClientStatusChangeCallback()(((ROCRTSPClient*)rtspClient)->id, false);
			env  << "Failed to set up the \""  << "\" subsession: " << resultString << "\n";
			break;
		}

		env  << "Set up the \"" << "\" subsession (client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum() + 1 << ")\n";

		const char *sprop = scs.subsession->fmtp_spropparametersets();
		uint8_t const* sps = NULL;
		unsigned spsSize = 0;
		uint8_t const* pps = NULL;
		unsigned ppsSize = 0;

		if (sprop != NULL) {
			unsigned numSPropRecords;
			SPropRecord* sPropRecords = parseSPropParameterSets(sprop, numSPropRecords);
			for (unsigned i = 0; i < numSPropRecords; ++i) {
				if (sPropRecords[i].sPropLength == 0) continue; // bad data
				u_int8_t nal_unit_type = (sPropRecords[i].sPropBytes[0]) & 0x1F;
				if (nal_unit_type == 7/*SPS*/) {
					sps = sPropRecords[i].sPropBytes;
					spsSize = sPropRecords[i].sPropLength;
				}
				else if (nal_unit_type == 8/*PPS*/) {
					pps = sPropRecords[i].sPropBytes;
					ppsSize = sPropRecords[i].sPropLength;
				}
			}
		}

		// Having successfully setup the subsession, create a data sink for it, and call "startPlaying()" on it.
		// (This will prepare the data sink to receive data; the actual flow of data from the client won't start happening until later,
		// after we've sent a RTSP "PLAY" command.)
		scs.subsession->sink = ROCSink::createNew(env, *scs.subsession, ((ROCRTSPClient*)rtspClient)->id , rtspClient->url());
		// perhaps use your own custom "MediaSink" subclass instead
		if (scs.subsession->sink == NULL) {
			theApp.getClientStatusChangeCallback()(((ROCRTSPClient*)rtspClient)->id, false);
			env  << "Failed to create a data sink for the \"" << "\" subsession: " << env.getResultMsg() << "\n";
			break;
		}

		env << "Created a data sink for the \"" << "\" subsession\n";
		scs.subsession->miscPtr = rtspClient; // a hack to let subsession handle functions get the "RTSPClient" from the subsession 
		if (sps != NULL) {
			((ROCSink *)scs.subsession->sink)->setSprop(sps, spsSize);
		}
		if (pps != NULL) {
			((ROCSink *)scs.subsession->sink)->setSprop(pps, ppsSize);
		}
		scs.subsession->sink->startPlaying(*(scs.subsession->readSource()),
			subsessionAfterPlaying, scs.subsession);
		// Also set a handler to be called if a RTCP "BYE" arrives for this subsession:
		if (scs.subsession->rtcpInstance() != NULL) {
			scs.subsession->rtcpInstance()->setByeHandler(subsessionByeHandler, scs.subsession);
		}
	} while (0);
	delete[] resultString;

	// Set up the next subsession, if any:
	setupNextSubsession(rtspClient);
}

void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString) {
	Boolean success = False;

	do {
		UsageEnvironment& env = rtspClient->envir(); // alias
		StreamClientState& scs = ((ROCRTSPClient*)rtspClient)->scs; // alias

		if (resultCode != 0) {
			theApp.getClientStatusChangeCallback()(((ROCRTSPClient*)rtspClient)->id, false);
			env << "Failed to start playing session: " << resultString << "\n";
			break;
		}

		theApp.getClientStatusChangeCallback()(((ROCRTSPClient*)rtspClient)->id, true);
		// Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end
		// using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later
		// 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.
		// (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
		if (scs.duration > 0) {

			env << "Setting timer";
			unsigned const delaySlop = 2; // number of seconds extra to delay, after the stream's expected duration.  (This is optional.)
			scs.duration += delaySlop;
			unsigned uSecsToDelay = (unsigned)(scs.duration * 1000000);
			scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
		}

		env << "Started playing session";
		if (scs.duration > 0) {
			env << " (for up to " << scs.duration << " seconds)";
		}
		env << "...\n";

		success = True;
		env << "Everything went ok..." << "\n";
	} while (0);
	delete[] resultString;

	if (!success) {
		// An unrecoverable error occurred with this stream.
		shutdownStream(rtspClient);
	}
}


// Implementation of the other event handlers:

void subsessionAfterPlaying(void* clientData) {
	MediaSubsession* subsession = (MediaSubsession*)clientData;
	RTSPClient* rtspClient = (RTSPClient*)(subsession->miscPtr);

	// Begin by closing this subsession's stream:
	Medium::close(subsession->sink);
	subsession->sink = NULL;

	// Next, check whether *all* subsessions' streams have now been closed:
	MediaSession& session = subsession->parentSession();
	MediaSubsessionIterator iter(session);
	while ((subsession = iter.next()) != NULL) {
		if (subsession->sink != NULL) return; // this subsession is still active
	}

	// All subsessions' streams have now been closed, so shutdown the client:
	shutdownStream(rtspClient);
}

void subsessionByeHandler(void* clientData) {
	MediaSubsession* subsession = (MediaSubsession*)clientData;
	RTSPClient* rtspClient = (RTSPClient*)subsession->miscPtr;
	UsageEnvironment& env = rtspClient->envir(); // alias

	env << "Received RTCP \"BYE\" on \""  << "\" subsession\n";
	theApp.getClientStatusChangeCallback()(2, false);
	// Now act as if the subsession had closed:
	subsessionAfterPlaying(subsession);
}

void streamTimerHandler(void* clientData) {
	ROCRTSPClient* rtspClient = (ROCRTSPClient*)clientData;
	StreamClientState& scs = rtspClient->scs; // alias

	scs.streamTimerTask = NULL;

	// Shut down the stream:
	shutdownStream(rtspClient);
}


void shutdownStream(RTSPClient* rtspClient, int exitCode) {
	UsageEnvironment& env = rtspClient->envir(); // alias
	StreamClientState& scs = ((ROCRTSPClient*)rtspClient)->scs; // alias

	env << "Shutdown stream ! ";
																// First, check whether any subsessions have still to be closed:
	if (scs.session != NULL) {
		Boolean someSubsessionsWereActive = False;
		MediaSubsessionIterator iter(*scs.session);
		MediaSubsession* subsession;

		while ((subsession = iter.next()) != NULL) {
			if (subsession->sink != NULL) {
				Medium::close(subsession->sink);
				subsession->sink = NULL;

				if (subsession->rtcpInstance() != NULL) {
					subsession->rtcpInstance()->setByeHandler(NULL, NULL); // in case the server sends a RTCP "BYE" while handling "TEARDOWN"
				}

				someSubsessionsWereActive = True;
			}
		}

		if (someSubsessionsWereActive) {
			// Send a RTSP "TEARDOWN" command, to tell the server to shutdown the stream.
			// Don't bother handling the response to the "TEARDOWN".
			rtspClient->sendTeardownCommand(*scs.session, NULL);
		}
	}

	env  << "Closing the stream.\n";
	Medium::close(rtspClient);
	// Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.

	if (--rtspClientCount == 0) {
		// The final stream has ended, so exit the application now.
		// (Of course, if you're embedding this code into your own application, you might want to comment this out,
		// and replace it with "eventLoopWatchVariable = 1;", so that we leave the LIVE555 event loop, and continue running "main()".)
		eventLoopWatchVariable = 1;
	}
}
