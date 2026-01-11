#include "SwichTheme.h"

// 辅助函数：修改注册表键值
bool SetRegistryValue(HKEY hKeyRoot, const wchar_t* subKey, const wchar_t* valueName, DWORD data) {
    HKEY hKey;
    LONG lResult = RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_SET_VALUE, &hKey);
    if (lResult != ERROR_SUCCESS) return false;

    lResult = RegSetValueExW(hKey, valueName, 0, REG_DWORD, (const BYTE*)&data, sizeof(data));
    RegCloseKey(hKey);
    return (lResult == ERROR_SUCCESS);
}

// 辅助函数：强制刷新指定窗口及其子窗口
void ForceRefreshWindow(HWND hwnd) {
    if (hwnd) {
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
        PostMessageW(hwnd, WM_THEMECHANGED, 0, 0);
    }
}

void flash() {
    // =========================================================================
    // 关键步骤：刷新逻辑
    // =========================================================================

    std::cout << "正在广播刷新消息..." << std::endl;

    DWORD_PTR result;

    // 4. 广播 ImmersiveColorSet (触发颜色重算)
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"ImmersiveColorSet", SMTO_ABORTIFHUNG, 2000, &result);

    // 5. 广播 Environment (确保传统应用更新)
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 2000, &result);

    // 6. 广播 WM_THEMECHANGED (强制窗口重绘)
    SendMessageTimeoutW(HWND_BROADCAST, WM_THEMECHANGED, 0, 0, SMTO_ABORTIFHUNG, 2000, &result);

    // =========================================================================
    // 手动刷新任务栏
    // =========================================================================

    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
    ForceRefreshWindow(hTaskbar);

    HWND hSecTaskbar = FindWindowW(L"Shell_SecondaryTrayWnd", NULL);
    while (hSecTaskbar) {
        ForceRefreshWindow(hSecTaskbar);
        hSecTaskbar = FindWindowExW(NULL, hSecTaskbar, L"Shell_SecondaryTrayWnd", NULL);
    }
}

// 核心函数：切换主题并修复字体颜色
bool SwitchTheme(bool useDarkMode, const std::string& playlistName) {
    const wchar_t* personalizeKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";

    // 0 = Dark (深色), 1 = Light (浅色) -> 用于 SystemUsesLightTheme / AppsUseLightTheme
    DWORD themeValue = useDarkMode ? 0 : 1;

    // 0 = 不显示颜色, 1 = 显示颜色 -> 用于 ColorPrevalence
    // 逻辑：如果是深色模式，设为 1 (显示颜色)；如果是浅色模式，设为 0 (恢复默认)
    DWORD colorValue = useDarkMode ? 1 : 0;

    std::cout << "正在应用注册表设置 (" << (useDarkMode ? "深色 + 重点色" : "浅色") << ")..." << std::endl;

    // 1. 修改 SystemUsesLightTheme (系统界面：任务栏、开始菜单)
    SetRegistryValue(HKEY_CURRENT_USER, personalizeKey, L"SystemUsesLightTheme", themeValue);

    // 2. 修改 AppsUseLightTheme (应用界面：资源管理器、设置)
    SetRegistryValue(HKEY_CURRENT_USER, personalizeKey, L"AppsUseLightTheme", themeValue);

    LaunchWallpaperEngineWithPlaylist(playlistName);

    SetRegistryValue(HKEY_CURRENT_USER, personalizeKey, L"ColorPrevalence", colorValue);
    flash();

    std::cout << "切换完成。" << std::endl;
    return true;
}