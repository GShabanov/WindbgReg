#pragma once

//**************************************************************
//
// I actually have no idea how to make it easier. 
// Unfortunately, I had to copy the interface here.
//
//**************************************************************

#define STDMETHODmy(method)       HRESULT (STDMETHODCALLTYPE * method)
#define STDMETHODmy_(type, method) type (STDMETHODCALLTYPE * method)
#define THISmy                   void *This

typedef struct IDebugRegisters2Vtbl
{
    //const struct IDebugRegisters2myVtbl *lpVtbl;
    // IUnknown.
    STDMETHODmy(QueryInterface)(
        THISmy,
        _In_ REFIID InterfaceId,
        _Out_ PVOID* Interface
        );

    STDMETHODmy_(ULONG,AddRef)(
        THISmy
        );

    STDMETHODmy_(ULONG, Release)(
        THISmy
        );

    // IDebugRegisters.

    STDMETHODmy(GetNumberRegisters)(
        THISmy,
        _Out_ PULONG Number
        );
    STDMETHODmy(GetDescription)(
        THISmy,
        _In_ ULONG Register,
        _Out_writes_opt_(NameBufferSize) PSTR NameBuffer,
        _In_ ULONG NameBufferSize,
        _Out_opt_ PULONG NameSize,
        _Out_opt_ PDEBUG_REGISTER_DESCRIPTION Desc
        );

    STDMETHODmy(GetIndexByName)(
        THISmy,
        _In_ PCSTR Name,
        _Out_ PULONG Index
        );

    STDMETHODmy(GetValue)(
        THISmy,
        _In_ ULONG Register,
        _Out_ PDEBUG_VALUE Value
        ) PURE;
    // SetValue makes a best effort at coercing
    // the given value into the given registers
    // value type.  If the given value is larger
    // than the register can hold the least
    // significant bits will be dropped.  Float
    // to int and int to float will be done
    // if necessary.  Subregister bits will be
    // inserted into the master register.
    STDMETHODmy(SetValue)(
        THISmy,
        _In_ ULONG Register,
        _In_ PDEBUG_VALUE Value
        );
    // Gets Count register values.  If Indices is
    // non-NULL it must contain Count register
    // indices which control the registers affected.
    // If Indices is NULL the registers from Start
    // to Start + Count  1 are retrieved.
    STDMETHODmy(GetValues)(
        THISmy,
        _In_ ULONG Count,
        _In_reads_opt_(Count) PULONG Indices,
        _In_ ULONG Start,
        _Out_writes_(Count) PDEBUG_VALUE Values
        );

    STDMETHODmy(SetValues)(
        THISmy,
        _In_ ULONG Count,
        _In_reads_opt_(Count) PULONG Indices,
        _In_ ULONG Start,
        _In_reads_(Count) PDEBUG_VALUE Values
        );

    // Outputs a group of registers in a well-formatted
    // way thats specific to the platforms register set.
    // Uses the line prefix.
    STDMETHODmy(OutputRegisters)(
        THISmy,
        _In_ ULONG OutputControl,
        _In_ ULONG Flags
        );

    // Abstracted pieces of processor information.
    // The mapping of these values to architectural
    // registers is architecture-specific and their
    // interpretation and existence may vary.  They
    // are intended to be directly compatible with
    // calls which take this information, such as
    // stack walking.
    STDMETHODmy(GetInstructionOffset)(
        THISmy,
        _Out_ PULONG64 Offset
        );

    STDMETHODmy(GetStackOffset)(
        THISmy,
        _Out_ PULONG64 Offset
        );

    STDMETHODmy(GetFrameOffset)(
        THISmy,
        _Out_ PULONG64 Offset
        );

    // IDebugRegisters2.

    STDMETHODmy(GetDescriptionWide)(
        THISmy,
        _In_ ULONG Register,
        _Out_writes_opt_(NameBufferSize) PWSTR NameBuffer,
        _In_ ULONG NameBufferSize,
        _Out_opt_ PULONG NameSize,
        _Out_opt_ PDEBUG_REGISTER_DESCRIPTION Desc
        );

    STDMETHODmy(GetIndexByNameWide)(
        THISmy,
        _In_ PCWSTR Name,
        _Out_ PULONG Index
        );

    // Pseudo-registers are synthetic values derived
    // by the engine that are presented in a manner
    // similar to regular registers.  They are simple
    // value holders, similar to actual registers.
    // Pseudo-registers are defined for concepts,
    // such as current-instruction-pointer or
    // current-thread-data.  As such they have
    // types appropriate for their data.
    STDMETHODmy(GetNumberPseudoRegisters)(
        THISmy,
        _Out_ PULONG Number
        );

    STDMETHODmy(GetPseudoDescription)(
        THISmy,
        _In_ ULONG Register,
        _Out_writes_opt_(NameBufferSize) PSTR NameBuffer,
        _In_ ULONG NameBufferSize,
        _Out_opt_ PULONG NameSize,
        _Out_opt_ PULONG64 TypeModule,
        _Out_opt_ PULONG TypeId
        );

    STDMETHODmy(GetPseudoDescriptionWide)(
        THISmy,
        _In_ ULONG Register,
        _Out_writes_opt_(NameBufferSize) PWSTR NameBuffer,
        _In_ ULONG NameBufferSize,
        _Out_opt_ PULONG NameSize,
        _Out_opt_ PULONG64 TypeModule,
        _Out_opt_ PULONG TypeId
        );

    STDMETHODmy(GetPseudoIndexByName)(
        THISmy,
        _In_ PCSTR Name,
        _Out_ PULONG Index
        );

    STDMETHODmy(GetPseudoIndexByNameWide)(
        THISmy,
        _In_ PCWSTR Name,
        _Out_ PULONG Index
        );
    // Some pseudo-register values are affected
    // by the register source, others are not.
    STDMETHODmy(GetPseudoValues)(
        THISmy,
        _In_ ULONG Source,
        _In_ ULONG Count,
        _In_reads_opt_(Count) PULONG Indices,
        _In_ ULONG Start,
        _Out_writes_(Count) PDEBUG_VALUE Values
        );
    // Many pseudo-registers are read-only and cannot be set.
    STDMETHODmy(SetPseudoValues)(
        THISmy,
        _In_ ULONG Source,
        _In_ ULONG Count,
        _In_reads_opt_(Count) PULONG Indices,
        _In_ ULONG Start,
        _In_reads_(Count) PDEBUG_VALUE Values
        );

    // These expanded methods allow selection
    // of the source of register information.
    STDMETHODmy(GetValues2)(
        THISmy,
        _In_ ULONG Source,
        _In_ ULONG Count,
        _In_reads_opt_(Count) PULONG Indices,
        _In_ ULONG Start,
        _Out_writes_(Count) PDEBUG_VALUE Values
        );

    STDMETHODmy(SetValues2)(
        THISmy,
        _In_ ULONG Source,
        _In_ ULONG Count,
        _In_reads_opt_(Count) PULONG Indices,
        _In_ ULONG Start,
        _In_reads_(Count) PDEBUG_VALUE Values
        );

    STDMETHODmy(OutputRegisters2)(
        THISmy,
        _In_ ULONG OutputControl,
        _In_ ULONG Source,
        _In_ ULONG Flags
        );

    STDMETHODmy(GetInstructionOffset2)(
        THISmy,
        _In_ ULONG Source,
        _Out_ PULONG64 Offset
        );

    STDMETHODmy(GetStackOffset2)(
        THISmy,
        _In_ ULONG Source,
        _Out_ PULONG64 Offset
        );

    STDMETHODmy(GetFrameOffset2)(
        THISmy,
        _In_ ULONG Source,
        _Out_ PULONG64 Offset
        );
} IDebugRegisters2Vtbl;

//typedef struct IDebugRegisters2myVtbl IDebugRegisters2myVtbl;
//struct ifaceIDebugRegisters2myVtbl;
