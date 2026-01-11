#include "SwichTheme.h"
#include "LocalSun.h"
#include "Config.h"
#include "WallpaperEnginePath.h"
#include "AutoRun.h"
#include "OpenConfigFile.h"
#include "uninstall.h"

#include <iostream>
#include <windows.h>

void DebugPrint(const std::string& s)
{
#ifdef _DEBUG
    std::cout << s << std::endl;
#endif
}

int main()
{
    // ===== 防止多实例 =====
    HANDLE hMutex = CreateMutexA(nullptr, TRUE, "Global\\SunThemeSwitcherMutex");
    if (!hMutex || GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBoxA(
            nullptr,
            "SunThemeSwitcher is already running.\nOnly one instance can run at a time.",
            "Warning",
            MB_OK | MB_ICONWARNING
        );
        if (hMutex) CloseHandle(hMutex);
        return 0;
    }

    // ===== 卸载优先级最高（不依赖 cfg）=====
    if (ShouldUninstall())
    {
        SelfUninstall();
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return 0;
    }

    AppConfig cfg;
    bool loaded = LoadConfig(cfg);

    // ===== 自动修复 Wallpaper Engine 路径 =====
    if (loaded && cfg.wallpaperPath.empty())
    {
        cfg.wallpaperPath = GetWallpaperEngineExePath();
        if (!cfg.wallpaperPath.empty())
            SaveConfig(cfg);
    }

    // ===== 配置不存在 或 配置不完整 =====
    if (!loaded || !IsConfigComplete(cfg))
    {
        if (!loaded)
            SaveConfig(cfg);   // 写模板

        MessageBoxA(
            nullptr,
            "Configuration is incomplete.\n\n"
            "Please complete the configuration file.\n"
            "The program will exit after you click OK.",
            "SunThemeSwitcher",
            MB_OK | MB_ICONWARNING
        );

        OpenConfigFile(GetConfigPath());

        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return 0;
    }

    // ===== 是否启用自启动（只询问一次）=====
    if (!cfg.autoRunSet)
    {
        int ret = MessageBoxA(
            nullptr,
            "Enable auto start when Windows boots?",
            "SunThemeSwitcher",
            MB_YESNO | MB_ICONQUESTION
        );

        cfg.autoRunEnabled = (ret == IDYES);
        cfg.autoRunSet = true;

        if (cfg.autoRunEnabled)
            EnableAutoRun();
        else
            DisableAutoRun();

        SaveConfig(cfg);
    }

    DebugPrint("SunThemeSwitcher started.");

    // ===== 主循环（事件去抖）=====
    LocalSun::SunEvent lastEvent = LocalSun::SunEvent::None;

    while (true)
    {
        LocalSun::SunEvent ev = LocalSun::checkSunEventNow();

        if (ev != lastEvent)
        {
            lastEvent = ev;

            switch (ev)
            {
            case LocalSun::SunEvent::Sunrise:
                DebugPrint("Sunrise");
                SwitchTheme(false, cfg.playlistWhite);
                break;

            case LocalSun::SunEvent::Sunset:
                DebugPrint("Sunset");
                SwitchTheme(true, cfg.playlistBlack);
                break;

            default:
                break;
            }
        }
        else {
            DebugPrint("not yet");
        }

        Sleep(10 * 1000); // 10 秒轮询
    }

    // ===== 理论上不会执行到这里 =====
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return 0;
}
