#pragma once
#include <string>

struct AppConfig {
    std::string playlistWhite;
    std::string playlistBlack;
    std::string wallpaperPath;
    bool autoRunEnabled = false;
    bool autoRunSet = false;
};

bool LoadConfig(AppConfig& cfg);
bool SaveConfig(const AppConfig& cfg);
bool ShouldUninstall();
std::string GetConfigPath();
bool IsConfigComplete(const AppConfig& cfg);

