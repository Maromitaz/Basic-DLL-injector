#include "stdafx.h"
DWORD GetProcId(const wchar_t* procName)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                if (!_wcsicmp(procEntry.szExeFile, procName))
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procId;
}
std::string ws2s(const std::wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}
int main()
{
    wchar_t proc[100], dll[100];
    std::cout << "Procces name: ";
    std::wcin.get(proc, 100);
    Sleep(25);
    std::cin.ignore();
    std::cout << "DLL name: ";
    std::wcin.get(dll, 100);
    std::experimental::filesystem::path dllDir = std::experimental::filesystem::current_path();
    std::wstring wdllPath = dllDir.wstring() + L"\\" + dll;
    char *dllPath;
    std::string sdllPath(ws2s(wdllPath));
    dllPath = &sdllPath[0];
    DWORD procId = 0;
    while (!procId)
    {
        procId = GetProcId(proc);
        Sleep(30);
    }
    if (!procId)
    {
        std::cout << "Process not found, press enter to exit.";
        std::cin.get();
    }
    if (!std::experimental::filesystem::path(dllPath).has_filename())
    {
        std::cout << "DLL not found, press enter to exit.";
        std::cin.get();
    }
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {
        void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);

        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

        if (hThread)
        {
            CloseHandle(hThread);
        }
    }
    if (hProc)
    {
        CloseHandle(hProc);
    }
    /*
    system("cls");
    std::cout << "Injecting " << dll << " in the process " << proc << "\n";
    */
    return 0;
}
