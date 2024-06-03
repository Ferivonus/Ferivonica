#include "DllInjector.h"

std::wstring DllInjector::CreateOrGetDLLPath(const std::wstring& fileName) {
    wchar_t userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName) / sizeof(userName[0]);
    if (!GetUserNameW(userName, &userNameSize)) {
        std::cerr << "Failed to retrieve user name." << std::endl;
        return L"";
    }

    std::wstring desiredPath = L"C:\\Users\\" + std::wstring(userName) + L"\\Desktop\\" + fileName;
    if (GetFileAttributesW(desiredPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return desiredPath;
    }
    else {
        std::wofstream dllFile(desiredPath.c_str());
        if (!dllFile.is_open()) {
            std::wcerr << "Failed to create DLL file at: " << desiredPath << std::endl;
            std::cerr << "Error code: " << GetLastError() << std::endl;
            return L"";
        }
        dllFile.close();
        std::wcout << "DLL file created at: " << desiredPath << std::endl;
        return desiredPath;
    }
}

DWORD DllInjector::GetProcIdByName(const wchar_t* procName) {
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32W procEntry{};
    procEntry.dwSize = sizeof(PROCESSENTRY32W);
    if (Process32FirstW(hSnap, &procEntry)) {
        do {
            if (_wcsicmp(procEntry.szExeFile, procName) == 0) {
                procId = procEntry.th32ProcessID;
                break;
            }
        } while (Process32NextW(hSnap, &procEntry));
    }
    CloseHandle(hSnap);
    return procId;
}

std::wstring DllInjector::GetModuleFileExeName() {
    constexpr size_t MAX_PATH_LEN = 260;
    std::vector<wchar_t> pathBuf(MAX_PATH_LEN);
    DWORD copied = GetModuleFileNameW(NULL, pathBuf.data(), static_cast<DWORD>(pathBuf.size()));

    if (copied == 0 || copied >= pathBuf.size()) {
        throw std::runtime_error("Failed to get module file name. Error: " + std::to_string(GetLastError()));
    }

    std::wstring filePath(pathBuf.data());
    size_t pos = filePath.find_last_of(L"\\");
    if (pos != std::wstring::npos && pos + 1 < filePath.size()) {
        return filePath.substr(pos + 1);
    }
    else {
        throw std::runtime_error("Invalid module file name.");
    }
}

bool DllInjector::DoesFileExist(const std::wstring& filePath) {
    DWORD fileAttributes = GetFileAttributesW(filePath.c_str());
    return (fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

std::wstring DllInjector::GetValidFileName(const std::wstring& fileName) {
    std::wstring validFileName;
    for (wchar_t ch : fileName) {
        if (ch == L'<' || ch == L'>' || ch == L':' || ch == L'"' || ch == L'/' || ch == L'\\' || ch == L'|' || ch == L'?' || ch == L'*') {
            validFileName += L'_';
        }
        else {
            validFileName += ch;
        }
    }
    return validFileName;
}

bool DllInjector::InjectDLL() {
    std::wstring dllPath = CreateOrGetDLLPath(L"dl.dll");

    if (dllPath.empty()) {
        std::cerr << "Failed to get DLL path." << std::endl;
        return false;
    }

    std::wcout << "DLL path: " << dllPath << std::endl;

    std::wstring targetProcName = GetModuleFileExeName();
    if (targetProcName.empty()) {
        return false;
    }

    DWORD procId = GetProcIdByName(targetProcName.c_str());
    if (procId == 0) {
        std::cerr << "Failed to get target process ID." << std::endl;
        return false;
    }

    if (!SetFileAttributesW(targetProcName.c_str(), FILE_ATTRIBUTE_HIDDEN)) {
        std::cerr << "Failed to set file attributes. Error: " << GetLastError() << std::endl;
        return false;
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
    if (hProc == NULL) {
        std::cerr << "Failed to open target process. Error: " << GetLastError() << std::endl;
        return false;
    }

    LPVOID remoteMem = VirtualAllocEx(hProc, nullptr, dllPath.size() * sizeof(wchar_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (remoteMem == NULL) {
        std::cerr << "Failed to allocate remote memory. Error: " << GetLastError() << std::endl;
        CloseHandle(hProc);
        return false;
    }

    if (!WriteProcessMemory(hProc, remoteMem, dllPath.c_str(), dllPath.size() * sizeof(wchar_t), nullptr)) {
        std::cerr << "Failed to write DLL path to remote process. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (kernel32 == NULL) {
        std::cerr << "Failed to get handle to kernel32.dll. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    LPTHREAD_START_ROUTINE loadLibraryAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32, "LoadLibraryW");
    if (loadLibraryAddr == NULL) {
        std::cerr << "Failed to get address of LoadLibraryW. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, loadLibraryAddr, remoteMem, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "Failed to create remote thread. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);
    std::cout << "DLL injected successfully." << std::endl;

    CloseHandle(hThread);
    VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProc);
    return true;
}
