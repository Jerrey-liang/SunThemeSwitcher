#include "Config.h"
#include <windows.h>
#include <shlobj.h>
#include <fstream>
#include <vector>

#pragma comment(lib, "Shell32.lib")

bool IsConfigComplete(const AppConfig& cfg)
{
    return
        !cfg.playlistWhite.empty() &&
        !cfg.playlistBlack.empty() &&
        !cfg.wallpaperPath.empty();
}

std::string GetConfigPath()
{
    char path[MAX_PATH];
    SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path);

    std::string dir = std::string(path) + "\\SunThemeSwitcher";
    CreateDirectoryA(dir.c_str(), nullptr);

    return dir + "\\config.ini";
}

bool ShouldUninstall()
{
    std::ifstream file(GetConfigPath());
    if (!file.is_open())
        return false;

    std::string line, last;
    while (std::getline(file, line))
    {
        if (!line.empty())
            last = line;
    }

    return last == "uninstall";
}

bool LoadConfig(AppConfig& cfg)
{
    std::ifstream file(GetConfigPath());
    if (!file.is_open())
        return false;

    std::string line;
    while (std::getline(file, line))
    {
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);

        if (key == "playlist_white") cfg.playlistWhite = val;
        else if (key == "playlist_black") cfg.playlistBlack = val;
        else if (key == "wallpaper_path") cfg.wallpaperPath = val;
        else if (key == "autorun")
        {
            cfg.autoRunEnabled = (val == "1");
            cfg.autoRunSet = true;
        }
    }

    return true; // ← 关键：只要文件存在并可读
}


bool SaveConfig(const AppConfig& cfg)
{
    std::ofstream file(GetConfigPath(), std::ios::trunc);
    if (!file.is_open())
        return false;

    file << "playlist_white=" << cfg.playlistWhite << "\n";
    file << "playlist_black=" << cfg.playlistBlack << "\n";
    file << "wallpaper_path=" << cfg.wallpaperPath << "\n";

    if (cfg.autoRunSet)
        file << "autorun=" << (cfg.autoRunEnabled ? "1" : "0") << "\n";

    return true;
}
