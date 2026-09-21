#include <windows.h>
#include <winternl.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "ntdll.lib")

typedef NTSTATUS(NTAPI* pNtQueryInformationProcess)(
    HANDLE, UINT, PVOID, ULONG, PULONG);

bool CheckIsDebuggerPresent() {
    return IsDebuggerPresent();
}

bool CheckRemoteDebugger() {
    BOOL isDebugged = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebugged);
    return isDebugged;
}

bool CheckPEB() {
#ifdef _WIN64
    PPEB peb = (PPEB)__readgsqword(0x60);
#else
    PPEB peb = (PPEB)__readfsdword(0x30);
#endif
    return peb->BeingDebugged;
}

bool CheckDebugPort() {
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) return false;

    auto NtQueryInformationProcess = (pNtQueryInformationProcess)
        GetProcAddress(hNtdll, "NtQueryInformationProcess");
    if (!NtQueryInformationProcess) return false;

    DWORD_PTR debugPort = 0;
    ULONG returnLength = 0;
    NTSTATUS status = NtQueryInformationProcess(
        GetCurrentProcess(), 7 /* ProcessDebugPort */,
        &debugPort, sizeof(debugPort), &returnLength);

    return (status == 0 && debugPort != 0);
}

bool CheckDebugFlags() {
    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) return false;

    auto NtQueryInformationProcess = (pNtQueryInformationProcess)
        GetProcAddress(hNtdll, "NtQueryInformationProcess");
    if (!NtQueryInformationProcess) return false;

    DWORD debugFlags = 1;
    ULONG returnLength = 0;
    NTSTATUS status = NtQueryInformationProcess(
        GetCurrentProcess(), 0x1F /* ProcessDebugFlags */,
        &debugFlags, sizeof(debugFlags), &returnLength);

    return (status == 0 && debugFlags == 0);
}

bool IsBeingDebugged() {
    return CheckIsDebuggerPresent() ||
        CheckRemoteDebugger() ||
        CheckPEB() ||
        CheckDebugPort() ||
        CheckDebugFlags();
}

void debuggerThread()
{
    while (true)
    {
        if (CheckIsDebuggerPresent() ||
            CheckRemoteDebugger() ||
            CheckPEB() ||
            CheckDebugPort() ||
            CheckDebugFlags())
            std::cout << "exit() called in debugger.cpp for flagging debug check";

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

void initAntiDebugger()
{
    std::thread t(debuggerThread);
    t.detach();
}