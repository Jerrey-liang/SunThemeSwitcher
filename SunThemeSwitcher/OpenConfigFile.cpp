#include <windows.h>
#include <shellapi.h>
#include <string>
#include "OpenConfigFile.h"

void OpenConfigFile(const std::string& path)
{
    ShellExecuteA(
        nullptr,
        "open",
        "explorer.exe",
        ("/select,\"" + GetConfigPath() + "\"").c_str(),
        nullptr,
        SW_SHOWNORMAL
    );

}
