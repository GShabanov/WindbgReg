//**************************************************************************
//
// WinDbgReg.cpp
//
//
//**************************************************************************

#include "stdafx.h"

#include "Utils.h"
#include "WinDbgHooks.h"

IDataModelManager *g_pManager = nullptr;
IDebugHost *g_pHost = nullptr;
IDebugRegisters2 *g_hookedRegisters = nullptr;
BOOL              g_hooked = FALSE;

//**************************************************************************
// Standard DbgEng Extension Exports:
//

void 
PrintHelp(PDEBUG_CLIENT	piClient)
{
    UTILS_OutputString(
        piClient,
        DEBUG_OUTPUT_NORMAL,
        "WinDBG regfix extension is loaded\n\n"     \
        "avalaible options is:\n"                 \
        "!regfix           - get help\n"           \
        "!regfix hook      - hook interface\n"     \
        "!regfix unhook    - cleanup hooks\n");

}

//
// DebugExtensionInitialize:
//
// Called to initialize the debugger extension.  For a data model extension, this acquires
// the necessary data model interfaces from the debugger and instantiates singleton instances
// of any of the extension classes which provide the functionality of the debugger extension.
//
EXTERN_C
HRESULT CALLBACK
DebugExtensionInitialize(PULONG /*pVersion*/, PULONG /*pFlags*/)
{
    HRESULT hr = S_OK;

    __try
    {
        IDebugClient *spClient;
        IHostDataModelAccess *spAccess;

        //
        // Create a client interface to the debugger and ask for the data model interfaces.  The client
        // library requires an implementation of Debugger::DataModel::ClientEx::(GetManager and ::GetHost)
        // which return these interfaces when called.
        //
        hr = DebugCreate(__uuidof(IDebugClient), (void **)&spClient);

        if (FAILED(hr))
        {
            return hr;
        }

        hr = spClient->QueryInterface(__uuidof(IHostDataModelAccess), (void **)&spAccess);

        if (FAILED(hr))
        {
            spClient->Release();
            return hr;
        }


        hr = spAccess->GetDataModel(&g_pManager, &g_pHost);

        spAccess->Release();

        if (FAILED(hr))
        {
            spClient->Release();
            g_pManager = nullptr;
            g_pHost = nullptr;
            return hr;
        }

        PrintHelp(spClient);

        spClient->Release();

    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return E_FAIL;
    }


    return hr;
}

//
// regfix:
//
// entry point for our API
//
EXTERN_C
HRESULT CALLBACK
regfix(
	_In_		PDEBUG_CLIENT	piClient,
	_In_opt_	PCSTR			pszArgs
	)
{
    HRESULT hr = S_OK;

    BOOL    hookMode = FALSE;
    BOOL    unhookMode = FALSE;

    __try
    {
        if (pszArgs == NULL ||
            pszArgs[0] == NULL)
        {
            PrintHelp(piClient);
            return S_OK;
        }

        if (0 == strcmp(pszArgs, "hook"))
        {
            UTILS_OutputString(
                piClient,
                DEBUG_OUTPUT_NORMAL, "WinDbg regfix try to hook interface\n");

            if (g_hookedRegisters == nullptr)
            {
                hookMode = TRUE;
            }
            else
            {
                UTILS_OutputString(
                    piClient,
                    DEBUG_OUTPUT_NORMAL, "WinDbg interface is already hooked\n");
            }
        }

        if (0 == strcmp(pszArgs, "unhook"))
        {
            UTILS_OutputString(
                piClient,
                DEBUG_OUTPUT_NORMAL, "WinDbg regfix try to unhook interface\n");

            if (g_hookedRegisters != nullptr)
            {
                unhookMode = TRUE;
            }
            else
            {
                UTILS_OutputString(
                    piClient,
                    DEBUG_OUTPUT_NORMAL, "WinDbg nothing to unhook\n");
            }
        }

        if (hookMode)
        {
            hr = regfixhelper_ObtainInterface(
                piClient,
                &g_hookedRegisters);

            if (FAILED(hr))
            {
                UTILS_OutputString(
                    piClient,
                    DEBUG_OUTPUT_NORMAL, "WinDbg hook failed\n");

            }
            else
            {
                if (regfixhelper_HookInterface(
                    piClient,
                    g_hookedRegisters))
                {
                    g_hooked = TRUE;

                    UTILS_OutputString(
                        piClient,
                        DEBUG_OUTPUT_NORMAL, "WinDbg hook success\n");

                }
                else
                {

                    g_hookedRegisters->Release();
                    g_hookedRegisters = nullptr;
                }
            }

        }

        if (unhookMode)
        {
            if (regfixhelper_ReleaseInterface(
                NULL,
                g_hookedRegisters))
            {
                g_hooked = FALSE;
            }

            g_hookedRegisters->Release();
            g_hookedRegisters = nullptr;

        }

    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return E_FAIL;
    }

    return hr;

}

//
// DebugExtensionCanUnload:
//
// Called after DebugExtensionUninitialize to determine whether the debugger extension can
// be unloaded.  A return of S_OK indicates that it can.  A failure (or return of S_FALSE) indicates
// that it cannot.
//
// Extension libraries are responsible for ensuring that there are no live interfaces back into the
// extension before unloading!
//
EXTERN_C
HRESULT CALLBACK
DebugExtensionCanUnload(void)
{
    //return (objCount == 0) ? S_OK : S_FALSE;
    return S_OK;
}

//
// DebugExtensionUninitialize:
//
// Called before unloading (and before DebugExtensionCanUnload) to prepare the debugger extension for
// unloading.  Any manipulations done during DebugExtensionInitialize should be undone and any interfaces
// released.
//
// Deleting the singleton instances of extension classes should unlink them from the data model.  There still
// may be references into the extension alive from scripts, other debugger extensions, debugger variables,
// etc...  The extension cannot return S_OK from DebugExtensionCanUnload until there are no such live
// references.
//
// If DebugExtensionCanUnload returns a "do not unload" indication, it is possible that DebugExtensionInitialize
// will be called without an interveining unload.
//
EXTERN_C
void CALLBACK
DebugExtensionUninitialize()
{
    if (g_hooked == TRUE)
    {
        if (regfixhelper_ReleaseInterface(
            NULL,
            g_hookedRegisters))
        {
            g_hooked = FALSE;
        }

        g_hookedRegisters->Release();
        g_hookedRegisters = nullptr;
    }

    if (g_hookedRegisters != nullptr)
    {
        g_hookedRegisters->Release();
        g_hookedRegisters = nullptr;
    }                            

    if (g_pManager != nullptr)
    {
        g_pManager->Release();
        g_pManager = nullptr;
    }

    if (g_pHost != nullptr)
    {
        g_pHost->Release();
        g_pHost = nullptr;
    }
}

//
// DebugExtensionUnload:
//
// A final callback immediately before the DLL is unloaded.  This will only happen after a successful
// DebugExtensionCanUnload.
//
EXTERN_C
void CALLBACK
DebugExtensionUnload()
{
}

