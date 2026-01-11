#include "Uninstall.h"

std::string GetConfigDir()
{
    char path[MAX_PATH];
    SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path);
    return std::string(path) + "\\SunThemeSwitcher";
}

bool DeleteConfigDirectory()
{
    std::string dir = GetConfigDir();

    // SHFileOperation 要求双 '\0' 结尾
    char from[MAX_PATH + 2] = {};
    strcpy_s(from, dir.c_str());
    from[dir.size() + 1] = '\0';

    SHFILEOPSTRUCTA op{};
    op.wFunc = FO_DELETE;
    op.pFrom = from;
    op.fFlags = FOF_NO_UI | FOF_NOCONFIRMATION | FOF_SILENT;

    return SHFileOperationA(&op) == 0;
}

void SelfUninstall()
{
    // 1. 移除开机自启动
    DisableAutoRun();

    // 2. 删除配置目录
    DeleteConfigDirectory();

    // 3. 延迟删除自身 EXE
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);

    std::string cmd =
        "/C timeout 2 > nul & del \"" + std::string(exePath) + "\"";

    ShellExecuteA(
        nullptr,
        "open",
        "cmd.exe",
        cmd.c_str(),
        nullptr,
        SW_HIDE
    );

    ExitProcess(0);
}