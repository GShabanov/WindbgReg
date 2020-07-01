//**************************************************************************
// 
// stdafx.h
//
// Main Internal Header 
//
//**************************************************************************

#include <windows.h>
#include <oaidl.h>

#include <DbgEng.h>
#include <dbgmodel.h>

#include "WinDbgReg.h"

extern IDataModelManager *g_pManager;
extern IDebugHost *g_pHost;
