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
	this->height = DEFAULT_WIDTH;
	this->width  = DEFAULT_HEIGHT;
}


// Seul et unique objet CROCVideoDllApp

CROCVideoDllApp theApp;


// initialisation de CROCVideoDllApp

BOOL CROCVideoDllApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

