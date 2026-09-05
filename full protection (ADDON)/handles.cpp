#include <Windows.h>
#include <winternl.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <thread>

#include "encryption.h"

#ifndef STATUS_INFO_LENGTH_MISMATCH
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#endif

#pragma comment(lib, "ntdll.lib")

#ifndef SystemExtendedHandleInformation
#define SystemExtendedHandleInformation 64
#endif

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX
{
    PVOID Object;
    ULONG_PTR UniqueProcessId;
    ULONG_PTR HandleValue;
    ULONG GrantedAccess;
    USHORT CreatorBackTraceIndex;
    USHORT ObjectTypeIndex;
    ULONG HandleAttributes;
    ULONG Reserved;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX;

typedef struct _SYSTEM_HANDLE_INFORMATION_EX
{
    ULONG_PTR NumberOfHandles;
    ULONG_PTR Reserved;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX Handles[1];
} SYSTEM_HANDLE_INFORMATION_EX;

using NtQuerySystemInformation_t =
NTSTATUS(NTAPI*)(
    SYSTEM_INFORMATION_CLASS,
    PVOID,
    ULONG,
    PULONG
    );

std::wstring GetProcessName(DWORD pid)
{
    std::wstring result = L"<unknown>";

    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        pid
    );

    if (!hProcess)
        return result;

    wchar_t buffer[MAX_PATH];
    DWORD size = MAX_PATH;

    if (QueryFullProcessImageNameW(
        hProcess,
        0,
        buffer,
        &size))
    {
        result.assign(buffer, size);
    }

    CloseHandle(hProcess);
    return result;
}

std::vector<std::wstring> FindProcessesWithHandleToMe()
{
    const DWORD myPid = GetCurrentProcessId();

    std::vector<std::wstring> results;

    auto NtQuerySystemInformation =
        reinterpret_cast<NtQuerySystemInformation_t>(
            GetProcAddress(
                GetModuleHandleW(L"ntdll.dll"),
                "NtQuerySystemInformation"
            )
            );

    if (!NtQuerySystemInformation)
        return results;

    ULONG bufferSize = 1024 * 1024;
    std::vector<BYTE> buffer(bufferSize);

    NTSTATUS status;

    while (true)
    {
        status = NtQuerySystemInformation(
            (SYSTEM_INFORMATION_CLASS)SystemExtendedHandleInformation,
            buffer.data(),
            bufferSize,
            &bufferSize
        );

        if (status == STATUS_INFO_LENGTH_MISMATCH)
        {
            buffer.resize(bufferSize + 1024 * 1024);
            continue;
        }

        break;
    }

    if (status < 0)
        return results;

    auto* handleInfo =
        reinterpret_cast<SYSTEM_HANDLE_INFORMATION_EX*>(
            buffer.data()
            );

    for (ULONG_PTR i = 0;
        i < handleInfo->NumberOfHandles;
        ++i)
    {
        const auto& entry = handleInfo->Handles[i];

        DWORD ownerPid =
            static_cast<DWORD>(entry.UniqueProcessId);

        if (ownerPid == myPid)
            continue;

        HANDLE ownerProcess = OpenProcess(
            PROCESS_DUP_HANDLE,
            FALSE,
            ownerPid
        );

        if (!ownerProcess)
            continue;

        HANDLE duplicatedHandle = nullptr;

        BOOL result = ::DuplicateHandle(
            ownerProcess,
            reinterpret_cast<HANDLE>(entry.HandleValue),
            GetCurrentProcess(),
            &duplicatedHandle,
            0,
            FALSE,
            DUPLICATE_SAME_ACCESS
        );

        CloseHandle(ownerProcess);

        if (!result)
            continue;

        DWORD targetPid = GetProcessId(duplicatedHandle);

        if (targetPid == myPid)
        {
            std::wstring exePath = GetProcessName(ownerPid);

            if (!exePath.empty() &&
                std::find(results.begin(), results.end(), exePath)
                == results.end())
            {
                results.push_back(exePath);
            }
        }

        CloseHandle(duplicatedHandle);
    }

    return results;
}

EncKey key;

double initEncKey()
{
    return key.getKey();
}

const double encKey = initEncKey();
const std::vector<std::wstring> initHandles = encryption::encryptWVector(FindProcessesWithHandleToMe(), encKey);

// you dont need to use this method (creating and detaching a thread), its just an example :)

void checkHandles() {
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (!(encryption::decryptWVector(initHandles, encKey) == FindProcessesWithHandleToMe()))
            std::cout << "CALL: called exit() due to handle mistmatch";
    
    }

}
void initHandlesThread()
{
    initHandles;

    std::thread ht(checkHandles);

    ht.detach();
}