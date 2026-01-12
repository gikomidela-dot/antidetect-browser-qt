# Fingerprint Consistency Guide

## ⚠️ Проблема: "Browser Fingerprint is inconsistent"

Эта ошибка возникает когда параметры браузера не соответствуют друг другу. Например:
- User Agent говорит "Windows", а Platform говорит "Linux"
- User Agent говорит "Chrome", а WebGL Vendor говорит "Apple"
- Timezone не соответствует locale

## ✅ Решение: Используйте согласованные шаблоны

### Шаблон 1: Windows Chrome
```
User Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 Chrome/120.0.0.0
Platform: Win32
Vendor: Google Inc.
WebGL Vendor: Google Inc. (NVIDIA)
WebGL Renderer: ANGLE (NVIDIA, NVIDIA GeForce RTX 3060...)
Hardware Concurrency: 8
Device Memory: 8 GB
Screen: 1920x1080
Timezone: America/New_York
Locale: en-US
TLS Profile: Chrome 120 Windows
```

### Шаблон 2: macOS Safari
```
User Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 Version/17.0 Safari/605.1.15
Platform: MacIntel
Vendor: Apple Computer, Inc.
WebGL Vendor: Apple Inc.
WebGL Renderer: Apple M1
Hardware Concurrency: 8
Device Memory: 8 GB
Screen: 1920x1080
Timezone: America/Los_Angeles
Locale: en-US
TLS Profile: Safari 17 macOS
```

### Шаблон 3: Linux Firefox
```
User Agent: Mozilla/5.0 (X11; Linux x86_64; rv:120.0) Gecko/20100101 Firefox/120.0
Platform: Linux x86_64
Vendor: (empty)
WebGL Vendor: Mesa
WebGL Renderer: Mesa Intel(R) UHD Graphics 620
Hardware Concurrency: 4
Device Memory: 8 GB
Screen: 1920x1080
Timezone: Europe/London
Locale: en-GB
TLS Profile: Firefox 121 Windows
```

## 🎯 Правила согласованности

### 1. User Agent ↔ Platform
| User Agent содержит | Platform должен быть |
|---------------------|----------------------|
| Windows NT          | Win32                |
| Macintosh           | MacIntel             |
| X11; Linux          | Linux x86_64         |

### 2. User Agent ↔ Vendor
| Браузер | Vendor                  |
|---------|-------------------------|
| Chrome  | Google Inc.             |
| Safari  | Apple Computer, Inc.    |
| Firefox | (пусто)                 |
| Edge    | Google Inc.             |

### 3. User Agent ↔ WebGL
| Браузер + ОС      | WebGL Vendor        | WebGL Renderer                    |
|-------------------|---------------------|-----------------------------------|
| Chrome Windows    | Google Inc. (NVIDIA)| ANGLE (NVIDIA, GeForce...)        |
| Safari macOS      | Apple Inc.          | Apple M1 / Apple M2               |
| Firefox Linux     | Mesa                | Mesa Intel(R) UHD Graphics...     |

### 4. Timezone ↔ Locale
| Timezone              | Locale  |
|-----------------------|---------|
| America/New_York      | en-US   |
| America/Los_Angeles   | en-US   |
| Europe/London         | en-GB   |
| Europe/Paris          | fr-FR   |
| Asia/Tokyo            | ja-JP   |

### 5. Hardware Consistency
- **Windows/macOS**: 4-16 cores, 8-16 GB RAM
- **Linux**: 2-8 cores, 4-8 GB RAM
- **Mobile**: 4-8 cores, 2-4 GB RAM

### 6. Screen Resolution
Используйте реальные разрешения:
- 1920x1080 (Full HD)
- 1366x768 (HD)
- 2560x1440 (2K)
- 3840x2160 (4K)

## 🚫 Частые ошибки

### ❌ Ошибка 1: Несоответствие ОС
```
User Agent: Windows NT 10.0
Platform: Linux x86_64  ← НЕПРАВИЛЬНО!
```

### ❌ Ошибка 2: Несоответствие браузера
```
User Agent: Chrome/120.0
Vendor: Apple Computer, Inc.  ← НЕПРАВИЛЬНО!
```

### ❌ Ошибка 3: Несоответствие WebGL
```
User Agent: Safari/605.1.15
WebGL Vendor: Google Inc.  ← НЕПРАВИЛЬНО!
```

### ❌ Ошибка 4: Нереальные параметры
```
Hardware Concurrency: 128  ← НЕПРАВИЛЬНО!
Device Memory: 256 GB  ← НЕПРАВИЛЬНО!
Screen: 9999x9999  ← НЕПРАВИЛЬНО!
```

## ✅ Как создать правильный профиль

### Способ 1: Использовать шаблоны
1. Нажмите "Create Profile"
2. Выберите шаблон: "Windows Chrome", "macOS Safari" или "Linux Firefox"
3. Все параметры уже согласованы!

### Способ 2: Ручная настройка
1. Выберите User Agent
2. Установите Platform согласно таблице выше
3. Установите Vendor согласно браузеру
4. Установите WebGL Vendor/Renderer согласно ОС
5. Установите Timezone и Locale согласно региону
6. Проверьте Hardware параметры

## 🔍 Проверка fingerprint

Проверьте свой fingerprint на этих сайтах:
- https://browserleaks.com/javascript
- https://whoer.net
- https://iphey.com
- https://pixelscan.net
- https://abrahamjuliot.github.io/creepjs/

### Что проверять:
✅ User Agent соответствует Platform
✅ WebGL Vendor соответствует браузеру
✅ Timezone соответствует Locale
✅ Hardware параметры реалистичны
✅ Screen resolution реальное
✅ Canvas/Audio fingerprint уникальны
✅ WebRTC заблокирован

## 📊 Примеры реальных fingerprints

### Chrome 120 на Windows 10
```json
{
  "userAgent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
  "platform": "Win32",
  "vendor": "Google Inc.",
  "hardwareConcurrency": 8,
  "deviceMemory": 8,
  "screenWidth": 1920,
  "screenHeight": 1080,
  "colorDepth": 24,
  "timezone": "America/New_York",
  "locale": "en-US",
  "webglVendor": "Google Inc. (NVIDIA)",
  "webglRenderer": "ANGLE (NVIDIA, NVIDIA GeForce RTX 3060 Direct3D11 vs_5_0 ps_5_0)",
  "canvasNoise": true,
  "audioNoise": true,
  "webrtcProtection": true
}
```

### Safari 17 на macOS Sonoma
```json
{
  "userAgent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Safari/605.1.15",
  "platform": "MacIntel",
  "vendor": "Apple Computer, Inc.",
  "hardwareConcurrency": 8,
  "deviceMemory": 8,
  "screenWidth": 1920,
  "screenHeight": 1080,
  "colorDepth": 24,
  "timezone": "America/Los_Angeles",
  "locale": "en-US",
  "webglVendor": "Apple Inc.",
  "webglRenderer": "Apple M1",
  "canvasNoise": true,
  "audioNoise": true,
  "webrtcProtection": true
}
```

## 🛠️ Troubleshooting

### Проблема: "Automated behavior detected"
**Причина:** WebDriver флаг обнаружен
**Решение:** Используйте stealth режим (уже включен)

### Проблема: "Fingerprint is inconsistent"
**Причина:** Параметры не соответствуют друг другу
**Решение:** Используйте готовые шаблоны или следуйте таблицам выше

### Проблема: "Timezone mismatch"
**Причина:** Timezone не соответствует locale или IP
**Решение:** Установите timezone согласно вашему прокси/VPN

### ��роблема: "WebGL mismatch"
**Причина:** WebGL Vendor не соответствует User Agent
**Решение:** Используйте правильный WebGL Vendor для вашего браузера

## 📝 Checklist перед запуском

- [ ] User Agent соответствует Platform
- [ ] Vendor соответствует браузеру
- [ ] WebGL Vendor/Renderer соответствуют ОС и браузеру
- [ ] Timezone соответствует Locale
- [ ] Hardware параметры реалистичны (4-16 cores, 4-16 GB RAM)
- [ ] Screen resolution реальное (1920x1080, 1366x768, etc.)
- [ ] Canvas Noise включен
- [ ] Audio Noise включен
- [ ] WebRTC Protection включен
- [ ] TLS Profile выбран правильный

## 🎯 Рекомендации

1. **Используйте готовые шаблоны** - они уже согласованы
2. **Не смешивайте параметры** разных ОС/браузеров
3. **Проверяйте fingerprint** на специальных сайтах
4. **Используйте прокси** из той же страны что и timezone
5. **Меняйте fingerprint** регулярно для разных задач

---

**Успешной работ�� с Antidetect Browser! 🎉**
