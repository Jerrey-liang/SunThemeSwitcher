#pragma once
/*
 * LocalSun.hpp
 *
 * 对外接口声明：
 *  - 获取当地日出 / 日落时间
 *
 * 平台：
 *  - Windows 10+
 *  - C++17
 */

#include <string>

namespace LocalSun {

    // 日出 / 日落结果
    struct SunTime {
        std::string sunrise;  // HH:MM
        std::string sunset;   // HH:MM
        bool valid;           // 是否存在（日极昼 / 极夜 / 定位失败）
    };

    // 对外统一接口
    // 自动：
    //  - 读取系统日期
    //  - 获取系统时区
    //  - 使用 Windows 定位 API 获取经纬度
    SunTime getLocalSunriseSunset();
    
    // 太阳事件枚举
    enum class SunEvent {
        None,     // 非日出 / 日落
        Sunrise,  // 日出
        Sunset    // 日落
    };

    // 检测当前系统时间是否到达日出或日落
    // toleranceSeconds：允许的时间误差（秒）
    SunEvent checkSunEventNow(int toleranceSeconds = 60);

}
