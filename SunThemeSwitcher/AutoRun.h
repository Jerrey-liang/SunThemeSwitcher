#pragma once
#include <string>

// 是否已在注册表中启用自启动（仅检测，不修改）
bool IsAutoRunEnabled();

// 强制写入 / 更新自启动（始终覆盖路径）
bool EnableAutoRun();

// 强制移除自启动
bool DisableAutoRun();

// 根据配置状态确保自启动一致性（推荐在 main 中调用）
void EnsureAutoRun(bool autorunEnabled);
