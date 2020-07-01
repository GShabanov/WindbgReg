#pragma once
//**************************************************************************
//
// WinDbgHooks.h
//
//**************************************************************************

extern HRESULT
regfixhelper_ObtainInterface(
	_In_			IDebugClient         *piClient,
	_COM_Outptr_	IDebugRegisters2    **ppiDebugRegisters2
	);

extern BOOL
regfixhelper_HookInterface(
	_In_			IDebugClient      *piClient,
	_COM_Outptr_	IDebugRegisters2  *piDebugRegisters2
	);

extern BOOL
regfixhelper_ReleaseInterface(
	_In_			IDebugClient       *piClient,
	_COM_Outptr_	IDebugRegisters2    *piDebugRegisters2
	);
