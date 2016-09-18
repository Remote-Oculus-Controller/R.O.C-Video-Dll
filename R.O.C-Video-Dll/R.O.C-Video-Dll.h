// R.O.C-Video-Dll.h : fichier d'en-tête principal pour la DLL R.O.C-Video-Dll
//

#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'stdafx.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"		// symboles principaux


// CROCVideoDllApp
// Consultez R.O.C-Video-Dll.cpp pour l'implémentation de cette classe
//

class CROCVideoDllApp : public CWinApp
{
public:
	CROCVideoDllApp();

// Substitutions
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
