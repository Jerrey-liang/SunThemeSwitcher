#include "WallpaperEnginePath.h"

#include <windows.h>
#include <vector>
#include <fstream>

#pragma comment(lib, "Advapi32.lib")

// 判断文件是否存在
static bool FileExists(const std::string& path)
{
    DWORD attr = GetFileAttributesA(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES &&
        !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

// 从注册表读取 Steam 安装路径
static std::string GetSteamInstallPath()
{
    HKEY hKey;
    char buffer[MAX_PATH] = {};
    DWORD bufferSize = sizeof(buffer);

    if (RegOpenKeyExA(
        HKEY_LOCAL_MACHINE,
        "SOFTWARE\\WOW6432Node\\Valve\\Steam",
        0,
        KEY_READ,
        &hKey) != ERROR_SUCCESS)
    {
        return {};
    }

    if (RegQueryValueExA(
        hKey,
        "InstallPath",
        nullptr,
        nullptr,
        reinterpret_cast<LPBYTE>(buffer),
        &bufferSize) != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return {};
    }

    RegCloseKey(hKey);
    return buffer;
}

// 解析 Steam 库路径
static std::vector<std::string> ParseSteamLibraries(const std::string& steamPath)
{
    std::vector<std::string> libraries;

    std::string vdfPath = steamPath + "\\steamapps\\libraryfolders.vdf";
    std::ifstream file(vdfPath);
    if (!file.is_open())
        return libraries;

    std::string line;
    while (std::getline(file, line))
    {
        auto pos = line.find("\"path\"");
        if (pos != std::string::npos)
        {
            auto first = line.find('"', pos + 6);
            auto second = line.find('"', first + 1);
            if (first != std::string::npos && second != std::string::npos)
            {
                libraries.emplace_back(
                    line.substr(first + 1, second - first - 1));
            }
        }
    }

    // Steam 主目录本身也是库
    libraries.push_back(steamPath);
    return libraries;
}

// 对外接口：获取 wallpaper64.exe 路径
std::string GetWallpaperEngineExePath()
{
    std::string steamPath = GetSteamInstallPath();
    if (steamPath.empty())
        return {};

    auto libraries = ParseSteamLibraries(steamPath);

    for (const auto& lib : libraries)
    {
        std::string exePath =
            lib + "\\steamapps\\common\\wallpaper_engine\\wallpaper64.exe";

        if (FileExists(exePath))
            return exePath;
    }

    return {};
}

// 对外接口：启动 Wallpaper Engine 并打开播放列表
bool LaunchWallpaperEngineWithPlaylist(const std::string& playlistName)
{
    std::string exePath = GetWallpaperEngineExePath();
    if (exePath.empty())
        return false;

    // 构造命令行（CreateProcessA 要求可写缓冲区）
    std::string commandLine =
        "\"" + exePath + "\""
        " -control openPlaylist"
        " -playlist \"" + playlistName + "\"";

    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    BOOL result = CreateProcessA(
        nullptr,                       // lpApplicationName
        commandLine.data(),            // lpCommandLine（必须可写）
        nullptr,                       // lpProcessAttributes
        nullptr,                       // lpThreadAttributes
        FALSE,                         // bInheritHandles
        CREATE_NO_WINDOW,              // 可改为 0 显示窗口
        nullptr,                       // lpEnvironment
        nullptr,                       // lpCurrentDirectory
        &si,
        &pi
    );

    if (!result)
        return false;

    // 关闭句柄，避免资源泄露
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}
