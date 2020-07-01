//**************************************************************************
//
// WinDbgHooks.cpp
//
//
//**************************************************************************

#include "stdafx.h"
#include "Utils.h"
#include "WinDbgHooks.h"
#include "DebugRegistersCOM.h"
#include <DbgEng.h>

static IDebugRegisters2Vtbl  originalFunctions;


HRESULT 
regfixhelper_ObtainInterface(
	_In_			IDebugClient *		piClient,
	_COM_Outptr_	IDebugRegisters2 **	ppiDebugRegisters2
	)
{
	HRESULT              hrResult           = E_FAIL;
	IDebugRegisters     *piDebugRegisters   = NULL;
	IDebugRegisters2    *piDebugRegisters2  = NULL;

	// COM pointers should be NULL on failure.
	*ppiDebugRegisters2 = NULL;

	hrResult = piClient->QueryInterface(__uuidof(IDebugRegisters), (void **)&piDebugRegisters);

    if (FAILED(hrResult))
	{
		(VOID)UTILS_OutputString(piClient,
								 DEBUG_OUTPUT_EXTENSION_WARNING,
								 "failed to obtain IDebugRegisters\n");
		return hrResult;
	}

	hrResult = piDebugRegisters->QueryInterface(__uuidof(IDebugRegisters2), (void **)&piDebugRegisters2);

	if (FAILED(hrResult))
	{
		(VOID)UTILS_OutputString(piClient,
								 DEBUG_OUTPUT_EXTENSION_WARNING,
								 "failed to obtain IDebugRegisters2\n");

        piDebugRegisters->Release();
		return hrResult;
	}

	if (*((PVOID *)piDebugRegisters) != *((PVOID *)piDebugRegisters2))
	{
		(VOID)UTILS_OutputString(piClient,
								 DEBUG_OUTPUT_EXTENSION_WARNING,
								 "Huh? IDebugRegisters and IDebugRegisters2 are implemented by two separate objects.\n");
	}

    piDebugRegisters->Release();
	// Transfer ownership:
	*ppiDebugRegisters2 = (IDebugRegisters2 *)piDebugRegisters2;
	piDebugRegisters2 = NULL;
	
	hrResult = S_OK;

	return hrResult;
}

//
// Functions
//
static 
HRESULT
regfixhelper_HandleGet(
	_In_					IDebugRegisters2 *	piDebugRegisters,
	_In_					ULONG				nCount,
	_In_reads_opt_(nCount)	PULONG				pnIndices,
	_In_					ULONG				nStartIndex,
	_Out_writes_(nCount)	PDEBUG_VALUE		ptValues
	)
{
	HRESULT	hrResult				= S_OK;
	ULONG	nBadRegisterIndex		= 0;
	ULONG	nCurrentElement			= 0;
	ULONG	nCurrentRegisterIndex	= 0;

	hrResult = piDebugRegisters->GetIndexByName(
	    "xcr0",
	    &nBadRegisterIndex);

	if (FAILED(hrResult))
	{
		return hrResult;
	}

	for (nCurrentElement = 0; nCurrentElement < nCount; ++nCurrentElement)
	{
		nCurrentRegisterIndex = (NULL == pnIndices) ? (nCurrentElement + nStartIndex) : (pnIndices[nCurrentElement]);

		if (DEBUG_VALUE_INVALID == ptValues[nCurrentElement].Type)
		{
			if (nCurrentRegisterIndex != nBadRegisterIndex)
			{
				// Function failed on some other register.
				// Be conservative and don't do anything.
				hrResult = E_NOTIMPL;
				break;
			}
			else
			{

				// This is the bad register.
				// Set it to some valid value.
				ptValues[nCurrentElement].Type = DEBUG_VALUE_INT64;
				ptValues[nCurrentElement].I64 = 0x0;
                ptValues[nCurrentElement].Nat = FALSE;
			}
		}
	}

	return hrResult;
}

static 
HRESULT STDMETHODCALLTYPE
regfixhelper_GetValueHook(
	_In_	IDebugRegisters2 *	piThis,
	_In_	ULONG				nRegister,
	_Out_	PDEBUG_VALUE		ptValue
	)
{
	HRESULT			hrResult	= E_FAIL;

	// First, zero the output buffer to parse the results
	// when the original method returns
	SecureZeroMemory(ptValue, sizeof(*ptValue));

	// Invoke the original method
	hrResult = originalFunctions.GetValue(piThis, nRegister, ptValue);

	if (E_INVALIDARG == hrResult)
	{
		if (SUCCEEDED(regfixhelper_HandleGet(piThis,
											 1,
											 NULL,
											 nRegister,
											 ptValue)))
		{
			// Handled, so return success to the caller.
			hrResult = S_OK;
		}
	}

	// Keep last status

	return hrResult;
}

static 
HRESULT
STDMETHODCALLTYPE
regfixhelper_GetValuesHook(
	_In_					IDebugRegisters2 *	piThis,
	_In_					ULONG				nCount,
	_In_reads_opt_(nCount)	PULONG				pnIndices,
	_In_					ULONG				nStartIndex,
	_Out_writes_(nCount)	PDEBUG_VALUE		ptValues
	)
{
	HRESULT			hrResult		= E_FAIL;

	// First, zero the output buffer to parse the results
	// when the original method returns
	SecureZeroMemory(ptValues, nCount * sizeof(ptValues[0]));

	// Invoke the original method
	hrResult = originalFunctions.GetValues(piThis, nCount, pnIndices, nStartIndex, ptValues);

	if (E_INVALIDARG == hrResult)
	{
		if (SUCCEEDED(regfixhelper_HandleGet(piThis,
											nCount,
											pnIndices,
											nStartIndex,
											ptValues)))
		{
			// Handled, so return success to the caller.
			hrResult = S_OK;
		}
	}

	// Keep last status

	return hrResult;
}

static
HRESULT STDMETHODCALLTYPE
regfixhelper_GetValues2Hook(
	_In_					IDebugRegisters2 *	piThis,
	_In_					ULONG				eSource,
	_In_					ULONG				nCount,
	_In_reads_opt_(nCount)	PULONG				pnIndices,
	_In_					ULONG				nStartIndex,
	_Out_writes_(nCount)	PDEBUG_VALUE		ptValues
	)
{
	HRESULT			hrResult		= E_FAIL;

	// First, zero the output buffer to parse the results
	// when the original method returns
	SecureZeroMemory(ptValues, nCount * sizeof(ptValues[0]));

	// Invoke the original method
	hrResult = originalFunctions.GetValues2(piThis, eSource, nCount, pnIndices, nStartIndex, ptValues);

	if (E_INVALIDARG == hrResult)
	{

		if (SUCCEEDED(regfixhelper_HandleGet(piThis,
											 nCount,
											 pnIndices,
											 nStartIndex,
											 ptValues)))
		{
			// Handled, so return success to the caller.
			hrResult = S_OK;
		}
	}

	// Keep last status

	return hrResult;
}

BOOL
regfixhelper_HookInterface(
	_In_			IDebugClient     *piClient,
	_COM_Outptr_	IDebugRegisters2 *piDebugRegisters2
	)
{

    if (piDebugRegisters2 == nullptr)
        return FALSE;

    IDebugRegisters2Vtbl *pointer = *(IDebugRegisters2Vtbl **)piDebugRegisters2;

    DWORD  dwOldProtection = 0;

    if (VirtualProtect(pointer, sizeof(IDebugRegisters2Vtbl), PAGE_EXECUTE_READWRITE, &dwOldProtection) != TRUE)
    {
        return FALSE;
    }

    //
    // save originals
    //
    memcpy(&originalFunctions, pointer, sizeof(IDebugRegisters2Vtbl));

    //
    // install a hooks
    //
    InterlockedExchangePointer((volatile PVOID *)&pointer->GetValue, &regfixhelper_GetValueHook);
    InterlockedExchangePointer((volatile PVOID *)&pointer->GetValues, &regfixhelper_GetValuesHook);
    InterlockedExchangePointer((volatile PVOID *)&pointer->GetValues2, &regfixhelper_GetValues2Hook);

    VirtualProtect(pointer, sizeof(IDebugRegisters2Vtbl), dwOldProtection, &dwOldProtection);

    return TRUE;

    UNREFERENCED_PARAMETER(piClient);
}

BOOL
regfixhelper_ReleaseInterface(
	_In_			IDebugClient     *piClient,
	_COM_Outptr_	IDebugRegisters2 *piDebugRegisters2
	)
{
    UNREFERENCED_PARAMETER(piClient);

    if (piDebugRegisters2 == nullptr)
        return FALSE;

    IDebugRegisters2Vtbl *pointer = *(IDebugRegisters2Vtbl **)piDebugRegisters2;

    DWORD  dwOldProtection = 0;

    if (VirtualProtect(pointer, sizeof(IDebugRegisters2Vtbl), PAGE_EXECUTE_READWRITE, &dwOldProtection) != TRUE)
    {
        return FALSE;
    }

    InterlockedExchangePointer((volatile PVOID *)&pointer->GetValue, &originalFunctions.GetValue);
    InterlockedExchangePointer((volatile PVOID *)&pointer->GetValues, &originalFunctions.GetValues);
    InterlockedExchangePointer((volatile PVOID *)&pointer->GetValues2, &originalFunctions.GetValues2);

    VirtualProtect(pointer, sizeof(IDebugRegisters2Vtbl), dwOldProtection, &dwOldProtection);

    return TRUE;
}
