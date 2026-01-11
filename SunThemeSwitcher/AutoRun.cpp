#include <windows.h>
#include "AutoRun.h"

static const char* kRunKey =
"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const char* kAppName = "SunThemeSwitcher";

void EnableAutoRun()
{
    HKEY hKey;
    RegOpenKeyExA(HKEY_CURRENT_USER, kRunKey, 0, KEY_SET_VALUE, &hKey);

    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);

    RegSetValueExA(
        hKey, kAppName, 0, REG_SZ,
        (BYTE*)exePath, strlen(exePath) + 1);

    RegCloseKey(hKey);
}

void DisableAutoRun()
{
    HKEY hKey;
    RegOpenKeyExA(HKEY_CURRENT_USER, kRunKey, 0, KEY_SET_VALUE, &hKey);
    RegDeleteValueA(hKey, kAppName);
    RegCloseKey(hKey);
}
