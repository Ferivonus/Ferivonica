#include "BrowserCookiesPaths.h"
#include <iostream>

bool BrowserCookiesPaths::DoesFileExist(const std::wstring& filePath) {
    DWORD fileAttributes = GetFileAttributesW(filePath.c_str());
    return (fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

std::wstring BrowserCookiesPaths::GetUsername() {
    wchar_t username[MAX_PATH];
    size_t len;
    if (_wgetenv_s(&len, username, MAX_PATH, L"USERNAME") != 0 || len == 0) {
        std::cerr << "Failed to retrieve username from environment variables." << std::endl;
        return L"";
    }
    return std::wstring(username);
}

std::wstring BrowserCookiesPaths::GetChromeCookiesPath() {
    std::wstring username = GetUsername();
    if (username.empty()) return L"";

    std::wstring desiredPath = L"C:\\Users\\" + username + L"\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Network\\Cookies";

    if (DoesFileExist(desiredPath)) {
        std::wcout << "Chrome cookies file found at: " << desiredPath << std::endl;
        return desiredPath;
    }
    else {
        std::wcerr << "Failed to find Chrome cookies file at: " << desiredPath << std::endl;
        std::cerr << "Error code: " << GetLastError() << std::endl;
    }
    return L"";
}

std::wstring BrowserCookiesPaths::GetFirefoxProfilesPath() {
    std::wstring username = GetUsername();
    if (username.empty()) return L"";

    std::wstring desiredPath = L"C:\\Users\\" + username + L"\\AppData\\Roaming\\Mozilla\\Firefox\\Profiles";

    if (DoesFileExist(desiredPath)) {
        std::wcout << "Firefox profiles directory found at: " << desiredPath << std::endl;
        return desiredPath;
    }
    else {
        std::wcerr << "Failed to find Firefox profiles directory at: " << desiredPath << std::endl;
        std::cerr << "Error code: " << GetLastError() << std::endl;
    }
    return L"";
}

std::wstring BrowserCookiesPaths::GetEdgeCookiesPath() {
    std::wstring username = GetUsername();
    if (username.empty()) return L"";

    std::wstring desiredPath = L"C:\\Users\\" + username + L"\\AppData\\Local\\Microsoft\\Edge\\User Data\\Default\\Network\\Cookies";

    if (DoesFileExist(desiredPath)) {
        std::wcout << "Edge cookies file found at: " << desiredPath << std::endl;
        return desiredPath;
    }
    else {
        std::wcerr << "Failed to find Edge cookies file at: " << desiredPath << std::endl;
        std::cerr << "Error code: " << GetLastError() << std::endl;
    }
    return L"";
}

std::wstring BrowserCookiesPaths::GetOperaStableCookiesPath() {
    wchar_t appDataPath[MAX_PATH];
    DWORD result = GetEnvironmentVariable(L"APPDATA", appDataPath, MAX_PATH);
    if (result > 0 && result < MAX_PATH) {
        std::wstring desiredPath = std::wstring(appDataPath) + L"\\Opera Software\\Opera Stable\\Network\\Cookies";

        if (DoesFileExist(desiredPath)) {
            std::wcout << "Opera Stable cookies file found at: " << desiredPath << std::endl;
            return desiredPath;
        }
        else {
            std::wcerr << "Failed to find Opera Stable cookies file at: " << desiredPath << std::endl;
            std::cerr << "Error code: " << GetLastError() << std::endl;
        }
    }
    else {
        std::cerr << "Failed to retrieve AppData directory path." << std::endl;
    }
    return L"";
}

std::wstring BrowserCookiesPaths::GetOperaGXStableCookiesPath() {
    wchar_t appDataPath[MAX_PATH];
    DWORD result = GetEnvironmentVariable(L"APPDATA", appDataPath, MAX_PATH);
    if (result > 0 && result < MAX_PATH) {
        std::wstring desiredPath = std::wstring(appDataPath) + L"\\Opera Software\\Opera GX Stable\\Network\\Cookies";

        if (DoesFileExist(desiredPath)) {
            std::wcout << "Opera GX Stable cookies file found at: " << desiredPath << std::endl;
            return desiredPath;
        }
        else {
            std::wcerr << "Failed to find Opera GX Stable cookies file at: " << desiredPath << std::endl;
            std::cerr << "Error code: " << GetLastError() << std::endl;
        }
    }
    else {
        std::cerr << "Failed to retrieve AppData directory path." << std::endl;
    }
    return L"";
}

std::wstring BrowserCookiesPaths::GetBraveCookiesPath() {
    std::wstring username = GetUsername();
    if (username.empty()) return L"";

    std::wstring desiredPath = L"C:\\Users\\" + username + L"\\AppData\\Local\\BraveSoftware\\Brave-Browser\\User Data\\Default\\Service Worker\\Database\\CURRENT";

    if (DoesFileExist(desiredPath)) {
        std::wcout << "Brave cookies file found at: " << desiredPath << std::endl;
        return desiredPath;
    }
    else {
        std::wcerr << "Failed to find Brave cookies file at: " << desiredPath << std::endl;
        std::cerr << "Error code: " << GetLastError() << std::endl;
    }
    return L"";
}

std::vector<std::wstring> BrowserCookiesPaths::GetCookiePaths() {
    std::vector<std::wstring> cookiePaths;

    std::wstring chromeCookiesPath = GetChromeCookiesPath();
    if (!chromeCookiesPath.empty()) {
        cookiePaths.push_back(chromeCookiesPath);
    }
    else {
        std::wcerr << "Failed to retrieve Chrome cookies path." << std::endl;
    }

    std::wstring firefoxProfilesPath = GetFirefoxProfilesPath();
    if (!firefoxProfilesPath.empty()) {
        cookiePaths.push_back(firefoxProfilesPath);
    }
    else {
        std::wcerr << "Failed to retrieve Firefox profiles path." << std::endl;
    }

    std::wstring edgeCookiesPath = GetEdgeCookiesPath();
    if (!edgeCookiesPath.empty()) {
        cookiePaths.push_back(edgeCookiesPath);
    }
    else {
        std::wcerr << "Failed to retrieve Edge cookies path." << std::endl;
    }

    std::wstring operaStableCookiesPath = GetOperaStableCookiesPath();
    if (!operaStableCookiesPath.empty()) {
        cookiePaths.push_back(operaStableCookiesPath);
    }
    else {
        std::wcerr << "Failed to retrieve Opera Stable cookies path." << std::endl;
    }

    std::wstring operaGXStableCookiesPath = GetOperaGXStableCookiesPath();
    if (!operaGXStableCookiesPath.empty()) {
        cookiePaths.push_back(operaGXStableCookiesPath);
    }
    else {
        std::wcerr << "Failed to retrieve Opera GX Stable cookies path." << std::endl;
    }

    std::wstring braveCookiesPath = GetBraveCookiesPath();
    if (!braveCookiesPath.empty()) {
        cookiePaths.push_back(braveCookiesPath);
    }
    else {
        std::wcerr << "Failed to retrieve Brave cookies path." << std::endl;
    }

    return cookiePaths;
}

void BrowserCookiesPaths::PrintExistedCookiePaths(const std::vector<std::wstring>& cookiePaths) {
    std::wcout << "Cookie paths:" << std::endl;
    for (const auto& path : cookiePaths) {
        std::wcout << path << std::endl;
    }
}
