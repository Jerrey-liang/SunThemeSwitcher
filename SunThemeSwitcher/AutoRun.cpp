#include "AutoRun.h"
#include <windows.h>

static const char* kRunKeyPath =
"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const char* kRunValueName = "SunThemeSwitcher";

static std::string GetCurrentExePath()
{
    char path[MAX_PATH] = { 0 };
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return std::string(path);
}

bool IsAutoRunEnabled()
{
    HKEY hKey;
    if (RegOpenKeyExA(
        HKEY_CURRENT_USER,
        kRunKeyPath,
        0,
        KEY_QUERY_VALUE,
        &hKey
    ) != ERROR_SUCCESS)
    {
        return false;
    }

    LONG ret = RegQueryValueExA(
        hKey,
        kRunValueName,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    RegCloseKey(hKey);
    return ret == ERROR_SUCCESS;
}

bool EnableAutoRun()
{
    HKEY hKey;
    if (RegOpenKeyExA(
        HKEY_CURRENT_USER,
        kRunKeyPath,
        0,
        KEY_SET_VALUE,
        &hKey
    ) != ERROR_SUCCESS)
    {
        return false;
    }

    std::string exePath = GetCurrentExePath();

    // 关键：注册表 Run 项必须是完整命令行，路径需加引号
    std::string command = "\"" + exePath + "\"";

    LONG ret = RegSetValueExA(
        hKey,
        kRunValueName,
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(command.c_str()),
        static_cast<DWORD>(command.size() + 1)
    );

    RegCloseKey(hKey);
    return ret == ERROR_SUCCESS;
}

bool DisableAutoRun()
{
    HKEY hKey;
    if (RegOpenKeyExA(
        HKEY_CURRENT_USER,
        kRunKeyPath,
        0,
        KEY_SET_VALUE,
        &hKey
    ) != ERROR_SUCCESS)
    {
        return false;
    }

    LONG ret = RegDeleteValueA(hKey, kRunValueName);
    RegCloseKey(hKey);

    // 不存在也视为成功（状态已符合预期）
    return ret == ERROR_SUCCESS || ret == ERROR_FILE_NOT_FOUND;
}

void EnsureAutoRun(bool autorunEnabled)
{
    if (autorunEnabled)
    {
        // 始终覆盖写，修复路径变化
        EnableAutoRun();
    }
    else
    {
        DisableAutoRun();
    }
}
