#include "UserAgentDatabase.h"

UserAgentDatabase::UserAgentDatabase()
{
    initializeDatabase();
}

QStringList UserAgentDatabase::getCategories() const
{
    return m_database.keys();
}

QList<UserAgentProfile> UserAgentDatabase::getProfilesByCategory(const QString& category) const
{
    return m_database.value(category);
}

UserAgentProfile UserAgentDatabase::getProfile(const QString& name) const
{
    for (const auto& profiles : m_database) {
        for (const auto& profile : profiles) {
            if (profile.name == name) {
                return profile;
            }
        }
    }
    return UserAgentProfile();
}

FingerprintConfig UserAgentProfile::toFingerprintConfig() const
{
    FingerprintConfig config;
    config.userAgent = userAgent;
    config.platform = platform;
    config.vendor = vendor;
    config.renderer = webglVendor;
    config.hardwareConcurrency = hardwareConcurrency;
    config.deviceMemory = deviceMemory;
    config.screenWidth = screenWidth;
    config.screenHeight = screenHeight;
    config.colorDepth = 24;
    config.timezone = timezone;
    config.locale = locale;
    config.webglVendor = webglVendor;
    config.webglRenderer = webglRenderer;
    config.canvasNoise = true;
    config.audioNoise = true;
    config.webrtcProtection = true;
    return config;
}

void UserAgentDatabase::initializeDatabase()
{
    // ========== WINDOWS ==========
    QList<UserAgentProfile> windowsProfiles;
    
    // Chrome 120 Windows 10
    windowsProfiles.append({
        "Chrome 120 - Windows 10 (1920x1080)",
        "Windows",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
        "Win32",
        "Google Inc.",
        "Google Inc. (NVIDIA)",
        "ANGLE (NVIDIA, NVIDIA GeForce RTX 3060 Direct3D11 vs_5_0 ps_5_0)",
        1920, 1080, 8, 8,
        "America/New_York", "en-US"
    });
    
    // Chrome 120 Windows 11
    windowsProfiles.append({
        "Chrome 120 - Windows 11 (2560x1440)",
        "Windows",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
        "Win32",
        "Google Inc.",
        "Google Inc. (NVIDIA)",
        "ANGLE (NVIDIA, NVIDIA GeForce RTX 4070 Direct3D11 vs_5_0 ps_5_0)",
        2560, 1440, 12, 16,
        "America/Los_Angeles", "en-US"
    });
    
    // Edge 120 Windows
    windowsProfiles.append({
        "Edge 120 - Windows 10 (1920x1080)",
        "Windows",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36 Edg/120.0.0.0",
        "Win32",
        "Google Inc.",
        "Google Inc. (NVIDIA)",
        "ANGLE (NVIDIA, NVIDIA GeForce GTX 1660 Direct3D11 vs_5_0 ps_5_0)",
        1920, 1080, 6, 8,
        "America/Chicago", "en-US"
    });
    
    // Firefox 121 Windows
    windowsProfiles.append({
        "Firefox 121 - Windows 10 (1920x1080)",
        "Windows",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:121.0) Gecko/20100101 Firefox/121.0",
        "Win32",
        "",
        "Google Inc. (NVIDIA)",
        "ANGLE (NVIDIA, NVIDIA GeForce RTX 3060 Direct3D11 vs_5_0 ps_5_0)",
        1920, 1080, 8, 8,
        "America/New_York", "en-US"
    });
    
    m_database["Windows"] = windowsProfiles;
    
    // ========== MACOS ==========
    QList<UserAgentProfile> macosProfiles;
    
    // Safari 17 macOS Sonoma
    macosProfiles.append({
        "Safari 17 - macOS Sonoma (1920x1080)",
        "macOS",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Safari/605.1.15",
        "MacIntel",
        "Apple Computer, Inc.",
        "Apple Inc.",
        "Apple M1",
        1920, 1080, 8, 8,
        "America/Los_Angeles", "en-US"
    });
    
    // Safari 17 macOS (Retina)
    macosProfiles.append({
        "Safari 17 - macOS Sonoma (2560x1600 Retina)",
        "macOS",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Safari/605.1.15",
        "MacIntel",
        "Apple Computer, Inc.",
        "Apple Inc.",
        "Apple M2",
        2560, 1600, 10, 16,
        "America/Los_Angeles", "en-US"
    });
    
    // Chrome 120 macOS
    macosProfiles.append({
        "Chrome 120 - macOS Sonoma (1920x1080)",
        "macOS",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
        "MacIntel",
        "Google Inc.",
        "Apple Inc.",
        "Apple M1",
        1920, 1080, 8, 8,
        "America/Los_Angeles", "en-US"
    });
    
    // Firefox 121 macOS
    macosProfiles.append({
        "Firefox 121 - macOS Sonoma (1920x1080)",
        "macOS",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:121.0) Gecko/20100101 Firefox/121.0",
        "MacIntel",
        "",
        "Apple Inc.",
        "Apple M1",
        1920, 1080, 8, 8,
        "America/Los_Angeles", "en-US"
    });
    
    m_database["macOS"] = macosProfiles;
    
    // ========== LINUX ==========
    QList<UserAgentProfile> linuxProfiles;
    
    // Chrome 120 Linux
    linuxProfiles.append({
        "Chrome 120 - Ubuntu 22.04 (1920x1080)",
        "Linux",
        "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
        "Linux x86_64",
        "Google Inc.",
        "Mesa",
        "Mesa Intel(R) UHD Graphics 620 (KBL GT2)",
        1920, 1080, 4, 8,
        "Europe/London", "en-GB"
    });
    
    // Firefox 121 Linux
    linuxProfiles.append({
        "Firefox 121 - Ubuntu 22.04 (1920x1080)",
        "Linux",
        "Mozilla/5.0 (X11; Linux x86_64; rv:121.0) Gecko/20100101 Firefox/121.0",
        "Linux x86_64",
        "",
        "Mesa",
        "Mesa Intel(R) UHD Graphics 620 (KBL GT2)",
        1920, 1080, 4, 8,
        "Europe/London", "en-GB"
    });
    
    // Chrome 120 Linux (AMD)
    linuxProfiles.append({
        "Chrome 120 - Debian 12 (1920x1080)",
        "Linux",
        "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
        "Linux x86_64",
        "Google Inc.",
        "Mesa",
        "Mesa AMD Radeon RX 580 Series (polaris10, LLVM 15.0.6, DRM 3.49, 6.1.0)",
        1920, 1080, 6, 16,
        "Europe/Berlin", "de-DE"
    });
    
    m_database["Linux"] = linuxProfiles;
    
    // ========== ANDROID ==========
    QList<UserAgentProfile> androidProfiles;
    
    // Chrome 120 Android (Samsung)
    androidProfiles.append({
        "Chrome 120 - Android 13 Samsung (1080x2400)",
        "Android",
        "Mozilla/5.0 (Linux; Android 13; SM-G998B) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Mobile Safari/537.36",
        "Linux armv8l",
        "Google Inc.",
        "Qualcomm",
        "Adreno (TM) 660",
        1080, 2400, 8, 8,
        "America/New_York", "en-US"
    });
    
    // Chrome 120 Android (Pixel)
    androidProfiles.append({
        "Chrome 120 - Android 14 Pixel (1080x2400)",
        "Android",
        "Mozilla/5.0 (Linux; Android 14; Pixel 7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Mobile Safari/537.36",
        "Linux armv8l",
        "Google Inc.",
        "Google",
        "Mali-G78",
        1080, 2400, 8, 8,
        "America/Los_Angeles", "en-US"
    });
    
    // Chrome 120 Android (Xiaomi)
    androidProfiles.append({
        "Chrome 120 - Android 13 Xiaomi (1080x2400)",
        "Android",
        "Mozilla/5.0 (Linux; Android 13; M2102J20SG) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Mobile Safari/537.36",
        "Linux armv8l",
        "Google Inc.",
        "Qualcomm",
        "Adreno (TM) 650",
        1080, 2400, 8, 6,
        "Asia/Shanghai", "zh-CN"
    });
    
    m_database["Android"] = androidProfiles;
    
    // ========== IOS ==========
    QList<UserAgentProfile> iosProfiles;
    
    // Safari iOS 17 (iPhone 15 Pro)
    iosProfiles.append({
        "Safari - iOS 17 iPhone 15 Pro (1179x2556)",
        "iOS",
        "Mozilla/5.0 (iPhone; CPU iPhone OS 17_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Mobile/15E148 Safari/604.1",
        "iPhone",
        "Apple Computer, Inc.",
        "Apple Inc.",
        "Apple A17 Pro GPU",
        1179, 2556, 6, 8,
        "America/New_York", "en-US"
    });
    
    // Safari iOS 17 (iPhone 14)
    iosProfiles.append({
        "Safari - iOS 17 iPhone 14 (1170x2532)",
        "iOS",
        "Mozilla/5.0 (iPhone; CPU iPhone OS 17_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Mobile/15E148 Safari/604.1",
        "iPhone",
        "Apple Computer, Inc.",
        "Apple Inc.",
        "Apple A15 Bionic GPU",
        1170, 2532, 6, 6,
        "America/Los_Angeles", "en-US"
    });
    
    // Safari iOS 17 (iPad Pro)
    iosProfiles.append({
        "Safari - iOS 17 iPad Pro (2048x2732)",
        "iOS",
        "Mozilla/5.0 (iPad; CPU OS 17_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Mobile/15E148 Safari/604.1",
        "iPad",
        "Apple Computer, Inc.",
        "Apple Inc.",
        "Apple M2 GPU",
        2048, 2732, 8, 8,
        "America/New_York", "en-US"
    });
    
    m_database["iOS"] = iosProfiles;
}
