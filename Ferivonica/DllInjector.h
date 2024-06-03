#ifndef DLLINJECTOR_H
#define DLLINJECTOR_H

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <TlHelp32.h>

class DllInjector {
public:
    static std::wstring CreateOrGetDLLPath(const std::wstring& fileName);
    static DWORD GetProcIdByName(const wchar_t* procName);
    static bool DoesFileExist(const std::wstring& filePath);
    static std::wstring GetModuleFileExeName();
    static bool InjectDLL();

private:
    static std::wstring GetValidFileName(const std::wstring& fileName);
};

#endif // DLLINJECTOR_H
