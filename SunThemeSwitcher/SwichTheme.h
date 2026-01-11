#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>
#include <sstream>

#include "WallpaperEnginePath.h"

bool SetRegistryValue(HKEY hKeyRoot, const wchar_t* subKey, const wchar_t* valueName, DWORD data);
void ForceRefreshWindow(HWND hwnd);
void flash();
bool SwitchTheme(bool useDarkMode, const std::string& playlistName);