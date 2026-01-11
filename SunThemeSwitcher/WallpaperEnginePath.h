#pragma once

#include <string>

// 获取 wallpaper64.exe 的完整路径
// 成功：返回绝对路径
// 失败：返回空字符串
std::string GetWallpaperEngineExePath();

// 启动 Wallpaper Engine 并打开指定播放列表
// playlistName : 播放列表名称（或 ID）
// 成功返回 true，失败返回 false
bool LaunchWallpaperEngineWithPlaylist(const std::string& playlistName);
