#include "stdafx.h"

#include <stdarg.h>
#include "Utils.h"


HRESULT UTILS_OutputString(
    _In_	IDebugClient *	piClient,
    _In_	ULONG			nMask,
    _In_	PCSTR			pszFormat,
    ...
    )
{
    HRESULT			hrResult			= E_FAIL;
    IDebugControl  *piDebugControl		= NULL;
    va_list			vaArguments			= NULL;
    BOOL			bArgsInitialized	= FALSE;

    hrResult = piClient->QueryInterface(
        __uuidof(IDebugControl),
        (void **)&piDebugControl);

    if (FAILED(hrResult))
    {
        goto lblCleanup;
    }

    va_start(vaArguments, pszFormat);
    bArgsInitialized = TRUE;

    hrResult = piDebugControl->OutputVaList(
        nMask,
        pszFormat,
        vaArguments);

    if (FAILED(hrResult))
    {
        goto lblCleanup;
    }

    hrResult = S_OK;

lblCleanup:
    if (bArgsInitialized)
    {
        va_end(vaArguments);
        bArgsInitialized = FALSE;
    }

    piDebugControl->Release();

    return hrResult;
}
