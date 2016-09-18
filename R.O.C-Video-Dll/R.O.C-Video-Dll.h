// R.O.C-Video-Dll.h : fichier d'en-tête principal pour la DLL R.O.C-Video-Dll
//

#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"		// symboles principaux
#include "default.h"
#include "unityAPI.h"

// CROCVideoDllApp
// Consultez R.O.C-Video-Dll.cpp pour l'implémentation de cette classe

class CROCVideoDllApp : public CWinApp
{
public:
	CROCVideoDllApp();

// Substitutions
public:

	virtual	BOOL InitInstance();

public:

	unsigned int width;
	unsigned int height;
	

private:

	DECLARE_MESSAGE_MAP()
};
