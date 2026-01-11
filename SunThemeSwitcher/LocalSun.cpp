#include "LocalSun.h"

/* =========================
   Windows & C++ 标准库
   ========================= */

#include <windows.h>
#include <cmath>
#include <sstream>
#include <iomanip>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Geolocation.h>

#pragma comment(lib, "windowsapp.lib")

   /* =========================
      内部工具结构
      ========================= */

namespace {

    struct Location {
        double latitude;
        double longitude;
        bool valid;
    };

    constexpr double PI = 3.141592653589793;

    double deg2rad(double x) { return x * PI / 180.0; }
    double rad2deg(double x) { return x * 180.0 / PI; }

    std::string formatTime(double t) {
        int h = static_cast<int>(t);
        int m = static_cast<int>((t - h) * 60 + 0.5);
        if (m >= 60) { m -= 60; h++; }
        h = (h + 24) % 24;

        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << h
            << ":" << std::setw(2) << m;
        return oss.str();
    }

    int dayOfYear(int y, int m, int d) {
        static const int mdays[] =
        { 31,28,31,30,31,30,31,31,30,31,30,31 };

        bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        int n = d;
        for (int i = 0; i < m - 1; ++i) n += mdays[i];
        if (leap && m > 2) n++;
        return n;
    }

    void getSystemDate(int& y, int& m, int& d) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        y = st.wYear;
        m = st.wMonth;
        d = st.wDay;
    }

    int getTimezoneOffsetHours() {
        DYNAMIC_TIME_ZONE_INFORMATION tz{};
        GetDynamicTimeZoneInformation(&tz);
        return -tz.Bias / 60;
    }

    Location getCurrentLocation() {
        Location loc{ 0, 0, false };

        try {
            winrt::init_apartment();

            winrt::Windows::Devices::Geolocation::Geolocator geo;
            geo.DesiredAccuracy(
                winrt::Windows::Devices::Geolocation::PositionAccuracy::High
            );

            auto pos = geo.GetGeopositionAsync().get();
            auto p = pos.Coordinate().Point().Position();

            loc.latitude = p.Latitude;
            loc.longitude = p.Longitude;
            loc.valid = true;
        }
        catch (...) {
            loc.valid = false;
        }

        return loc;
    }

    LocalSun::SunTime computeSunriseSunset(
        int y, int m, int d,
        double lat, double lon,
        int tz
    ) {
        const double zenith = 90.833;
        int N = dayOfYear(y, m, d);
        double lngHour = lon / 15.0;

        auto calc = [&](bool sunrise) -> double {
            double t = N + ((sunrise ? 6.0 : 18.0) - lngHour) / 24.0;

            double M = 0.9856 * t - 3.289;
            double L = M + 1.916 * sin(deg2rad(M))
                + 0.020 * sin(deg2rad(2 * M))
                + 282.634;
            L = fmod(L + 360.0, 360.0);

            double RA = rad2deg(atan(0.91764 * tan(deg2rad(L))));
            RA = fmod(RA + 360.0, 360.0);

            double Lq = floor(L / 90.0) * 90.0;
            double RAq = floor(RA / 90.0) * 90.0;
            RA = (RA + Lq - RAq) / 15.0;

            double sinDec = 0.39782 * sin(deg2rad(L));
            double cosDec = cos(asin(sinDec));

            double cosH =
                (cos(deg2rad(zenith)) -
                    sinDec * sin(deg2rad(lat))) /
                (cosDec * cos(deg2rad(lat)));

            if (cosH > 1 || cosH < -1)
                return NAN;

            double H = sunrise
                ? 360.0 - rad2deg(acos(cosH))
                : rad2deg(acos(cosH));
            H /= 15.0;

            double T = H + RA - 0.06571 * t - 6.622;
            return fmod(T - lngHour + tz + 24.0, 24.0);
            };

        double sr = calc(true);
        double ss = calc(false);

        if (std::isnan(sr) || std::isnan(ss))
            return { "", "", false };

        return { formatTime(sr), formatTime(ss), true };
    }

    int timeToSeconds(const std::string& t) {
        // t 格式：HH:MM
        int h = std::stoi(t.substr(0, 2));
        int m = std::stoi(t.substr(3, 2));
        return h * 3600 + m * 60;
    }

    int getCurrentTimeSeconds() {
        SYSTEMTIME st;
        GetLocalTime(&st);
        return st.wHour * 3600 + st.wMinute * 60 + st.wSecond;
    }

} // unnamed namespace

/* =========================
   对外接口实现
   ========================= */

namespace LocalSun {

    SunTime getLocalSunriseSunset() {
        int y, m, d;
        getSystemDate(y, m, d);

        int tz = getTimezoneOffsetHours();
        Location loc = getCurrentLocation();

        if (!loc.valid)
            return { "", "", false };

        return computeSunriseSunset(
            y, m, d,
            loc.latitude,
            loc.longitude,
            tz
        );
    }

    SunEvent checkSunEventNow(int toleranceSeconds) {
        SunTime t = getLocalSunriseSunset();
        if (!t.valid)
            return SunEvent::None;

        int now = getCurrentTimeSeconds();

        int sunrise = timeToSeconds(t.sunrise);
        int sunset = timeToSeconds(t.sunset);

        if (std::abs(now - sunrise) <= toleranceSeconds)
            return SunEvent::Sunrise;

        if (std::abs(now - sunset) <= toleranceSeconds)
            return SunEvent::Sunset;

        return SunEvent::None;
    }
}
