// R.O.C-Video-Dll.cpp : définit les routines d'initialisation pour la DLL.
//

#include "stdafx.h"
#include "R.O.C-Video-Dll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: si cette DLL est liée dynamiquement aux DLL MFC,
//		toute fonction exportée de cette DLL qui appelle
//		MFC doit avoir la macro AFX_MANAGE_STATE ajoutée au
//		tout début de la fonction.
//
//		Par exemple :
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// corps de fonction normal ici
//		}
//
//		Il est très important que cette macro se trouve dans chaque
//		fonction, avant tout appel à MFC.  Cela signifie qu'elle
//		doit être la première instruction dans la 
//		fonction, avant toute déclaration de variable objet
//		dans la mesure où leurs constructeurs peuvent générer des appels à la DLL
//		MFC.
//
//		Consultez les notes techniques MFC 33 et 58 pour plus de
//		détails.
//

// CROCVideoDllApp

BEGIN_MESSAGE_MAP(CROCVideoDllApp, CWinApp)
END_MESSAGE_MAP()


// construction CROCVideoDllApp

CROCVideoDllApp::CROCVideoDllApp()
{
	this->_height = DEFAULT_WIDTH;
	this->_width  = DEFAULT_HEIGHT;

	this->_newVideoFrameCallback		= NULL;
	this->_clientStatusChangeCallback	= NULL;

	this->thread = NULL;
	this->_isStarted = false;
}


// Seul et unique objet CROCVideoDllApp

CROCVideoDllApp theApp;


// initialisation de CROCVideoDllApp

BOOL CROCVideoDllApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


int CROCVideoDllApp::pushAddr(char * addr)
{
	this->_addrs.push_back(addr);
	return this->_addrs.size() - 1;
}

bool CROCVideoDllApp::isStarted()
{
	return this->_isStarted;
}

int CROCVideoDllApp::start(bool isTCP)
{
	if (_clientStatusChangeCallback == NULL || _newVideoFrameCallback == NULL)
		return 1;
	if (this->_isStarted == true)
		return 1;
	this->thread = new  Thread<CROCVideoDllApp>(this, &CROCVideoDllApp::print);
	if (this->thread->start()) {
		this->_isStarted = true;
		return 0;
	}
	delete this->thread;
	return 1;
}

int CROCVideoDllApp::stop()
{
	if (this->_isStarted == false)
		return 1;
	if (this->thread) {
		this->thread->join();
		delete this->thread;
	}
	this->_isStarted = false;
	return 0;
}

bool CROCVideoDllApp::setResolution(unsigned int width, unsigned int height)
{
	if (width == 0 || height == 0)
		return false;
	this->_width	= width;
	this->_height	= height;
	return true;
}

void CROCVideoDllApp::setNewVideoFrameCallback(NewVideoFrameCallback callback)
{
	this->_newVideoFrameCallback = callback;
}

NewVideoFrameCallback CROCVideoDllApp::getNewVideoFrameCallback()
{
	return this->_newVideoFrameCallback;
}

void CROCVideoDllApp::setClientStatusChangeCallback(ClientStatusChangeCallback callback)
{
	this->_clientStatusChangeCallback = callback;
}

ClientStatusChangeCallback CROCVideoDllApp::getClientStatusChangeCallback()
{
	return this->_clientStatusChangeCallback;
}

uint8_t * CROCVideoDllApp::generateNewFrame()
{
	uint8_t * frame = new uint8_t[_width * _height * 3];
	memset(frame, 0, _width * _height * 3);
	for (unsigned int i = 0; i < _width * _height * 3; i += 3) {
		frame[i] = 255;
	}
	return frame;
}

DWORD CROCVideoDllApp::print()
{
	uint8_t * frame;
	this->_clientStatusChangeCallback(0, true);
	while (this->_isStarted == true)
	{
		frame = generateNewFrame();
		_newVideoFrameCallback(0, frame, _width, _height);
		free (frame);
		Sleep(30);
	}
	this->_clientStatusChangeCallback(0, false);
	return 0;
}