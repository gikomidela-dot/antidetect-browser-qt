# 🎭 Antidetect Browser

Полнофункциональный антидетект браузер на базе Qt6 WebEngine с поддержкой подмены fingerprint, proxy, VPN и многого другого.

![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-blue)
![Qt](https://img.shields.io/badge/Qt-6.5%2B-green)
![License](https://img.shields.io/badge/license-MIT-orange)

## ✨ Возможности

### 🎯 Fingerprint Spoofing
- ✅ User Agent подмена
- ✅ Platform (Win32, MacIntel, Linux, iPhone, iPad)
- ✅ Hardware Concurrency (CPU cores)
- ✅ Device Memory (RAM)
- ✅ Screen Resolution
- ✅ Timezone spoofing (с LD_PRELOAD hook)
- ✅ Locale
- ✅ Canvas fingerprint noise
- ✅ WebGL fingerprint (Vendor, Renderer)
- ✅ Audio fingerprint noise
- ✅ WebRTC protection
- ✅ TLS fingerprinting (JA3/JA4)

### 🌐 Proxy Support
- ✅ HTTP/HTTPS
- ✅ SOCKS4/SOCKS5
- ✅ Авторизация (username/password)
- ✅ Проверка IP

### 🔐 VPN Integration
- ✅ OpenVPN (.ovpn)
- ✅ WireGuard (.conf)
- ✅ **AmneziaWG** (обфусцированный WireGuard)
- ✅ Shadowsocks (ss://)
- ✅ V2Ray/XRay (vmess://, vless://)
- ✅ Импорт из файла/URL/текста

### 🍪 Cookie Management
- ✅ Экспорт cookies (JSON, Netscape)
- ✅ Импорт cookies
- ✅ Поддержка одного или нескольких профилей

### 📱 Шаблоны профилей
- ✅ Windows Chrome
- ✅ macOS Safari
- ✅ Linux Firefox
- ✅ **Android Chrome**
- ✅ **iOS Safari**

### 🔧 Дополнительно
- ✅ REST API (порт 8080)
- ✅ Автоматизация браузера
- ✅ Cookie robots
- ✅ Изолированные профили
- ✅ Темная тема UI

## 🚀 Быстрый старт

### Linux
```bash
cd /home/x/.qodo/antidetect-browser-qt/build
cmake ..
cmake --build . -j$(nproc)
LD_PRELOAD=../src/timezone_hook/libtimezone_hook.so ./AntidetectBrowser
```

### Windows
```powershell
.\build-windows.ps1
cd build-windows\AntidetectBrowser-Portable
.\AntidetectBrowser.exe
```

**Или используйте GitHub Actions для автоматической сборки!**

## 📦 Сборка для Windows БЕЗ Windows

### Способ 1: GitHub Actions (Рекомендуется)

1. Загрузите проект на GitHub
2. GitHub автоматически соберет Windows версию
3. Скачайте готовый ZIP из Artifacts

**Подробнее:** [HOW_TO_GET_WINDOWS_BUILD.txt](HOW_TO_GET_WINDOWS_BUILD.txt)

### Способ 2: Ручная сборка на Windows

**Подробнее:** [BUILD_WINDOWS.md](BUILD_WINDOWS.md)

## 📚 Документация

- [BUILD_WINDOWS.md](BUILD_WINDOWS.md) - Подробная инструкция по сборке для Windows
- [QUICK_START_WINDOWS.md](QUICK_START_WINDOWS.md) - Быстрый старт (5 минут)
- [GITHUB_ACTIONS_BUILD.md](GITHUB_ACTIONS_BUILD.md) - Автоматическая сборка через GitHub
- [VPN_SUPPORT.md](VPN_SUPPORT.md) - Поддержка VPN протоколов
- [PROXY_GUIDE.md](PROXY_GUIDE.md) - Настройка прокси
- [FINGERPRINT_CONSISTENCY.md](FINGERPRINT_CONSISTENCY.md) - Согласованность fingerprint

## 🎯 Использование

### 1. Создание профиля

1. Нажмите **"Create Profile"**
2. Выберите шаблон (Windows Chrome, macOS Safari, Android, iOS, etc.)
3. Настройте fingerprint параметры
4. Добавьте proxy (опционально)
5. Выберите VPN (опционально)
6. Нажми��е **"Save"**

### 2. Запуск браузера

1. Выберите профиль в Dashboard
2. Нажмите **"Launch"**
3. Браузер запустится с вашим fingerprint

### 3. Экспорт/Импорт cookies

1. Выберите профиль
2. Нажмите **"Export Cookies"** или **"Import Cookies"**
3. Выберите формат (JSON или Netscape)

### 4. Импорт VPN

1. Меню **File → Import VPN Configuration**
2. Выберите файл (.ovpn, .conf, .json) или вставьте URL
3. VPN появится в списке при создании профиля

## 🏗️ Архитектура

```
src/
├── core/               # Ядро приложения
│   ├── Application     # Главный синглтон
│   ├── BrowserWindowManager
│   └── CookieManager   # Управление cookies
├── profiles/           # Профили
│   ├── ProfileManager
│   ├── ProfileStorage
│   └── UserAgentDatabase
├── fingerprint/        # Fingerprint spoofing
│   ├── FingerprintManager
│   ├── CanvasSpoofing
│   ├── WebGLSpoofing
│   └── AudioSpoofing
├── proxy/              # Proxy
│   ├── ProxyManager
│   └── ProxyChecker
├── vpn/                # VPN
│   ├── VPNManager
│   ├── OpenVPNParser
│   ├── WireGuardParser
│   ├── AmneziaWGParser  # NEW!
│   ├── ShadowsocksParser
│   └── V2RayParser
├── tls/                # TLS Fingerprinting
│   └── TLSFingerprintManager
├── crypto/             # Шифрование
│   ├── Encryption
│   └── PasswordHash
├── api/                # REST API
│   ├── RestApiServer
│   └── ApiHandler
├── ui/                 # Интерфейс
│   ├── MainWindow
│   ├── Dashboard
│   ├── ProfileEditor
│   ├── BrowserWindow
│   └── VPNImportDialog
└── automation/         # Автоматизация
    ├── BrowserAutomation
    └── CookieRobot
```

## 🔧 Системные требования

### Linux
- Ubuntu 20.04+ / Debian 11+ / Fedora 35+
- Qt 6.5+
- OpenSSL 3.0+
- SQLite3
- 4 GB RAM (рекомендуется 8 GB)

### Windows
- Windows 10/11 (64-bit)
- Qt 6.5+ с MinGW или MSVC
- OpenSSL 3.0+
- 4 GB RAM (рекомендуется 8 GB)

## 📊 Размеры сборки

| Платформа | Размер | С зависимостями |
|-----------|--------|-----------------|
| Linux     | ~50 MB | ~150 MB         |
| Windows   | ~70 MB | ~180 MB         |
| macOS     | ~60 MB | ~170 MB         |

## 🛠️ Разработка

### Зависимости

```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-webengine-dev libssl-dev libsqlite3-dev cmake g++

# Fedora
sudo dnf install qt6-qtbase-devel qt6-qtwebengine-devel openssl-devel sqlite-devel cmake gcc-c++

# Arch
sudo pacman -S qt6-base qt6-webengine openssl sqlite cmake gcc
```

### Сборка

```bash
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
```

### Запуск с timezone hook

```bash
cd build
LD_PRELOAD=../src/timezone_hook/libtimezone_hook.so ./AntidetectBrowser
```

## 🔌 REST API

API доступен на `http://localhost:8080`

### Endpoints

```
GET  /api/profiles          # Список профилей
GET  /api/profiles/:id      # Получить профиль
POST /api/profiles          # Создать профиль
PUT  /api/profiles/:id      # Обновить профиль
DELETE /api/profiles/:id    # Удалить профиль
POST /api/profiles/:id/launch  # Запустить браузер
```

### Пример

```bash
# Получить все профили
curl http://localhost:8080/api/profiles

# Создать профиль
curl -X POST http://localhost:8080/api/profiles \
  -H "Content-Type: application/json" \
  -d '{"name":"Test Profile","fingerprint":{...}}'
```

## 🐛 Troubleshooting

### "Browser Fingerprint is inconsistent"
**Решение:** Используйте готовые шаблоны профилей. Они уже согласованы.
**Подробнее:** [FINGERPRINT_CONSISTENCY.md](FINGERPRINT_CONSISTENCY.md)

### Timezone не меняется (System time)
**Решение:** Используйте LD_PRELOAD hook:
```bash
LD_PRELOAD=./src/timezone_hook/libtimezone_hook.so ./AntidetectBrowser
```

### Proxy не работает
**Решение:** Проверьте формат: `host:port` или `username:password@host:port`

## 📝 TODO

- [ ] Поддержка расширений Chrome
- [ ] Импорт профилей из других антидетектов
- [ ] Синхронизация профилей через облако
- [ ] Mobile view эмуляция
- [ ] Автоматическое обновление User Agent
- [ ] Поддержка Tor
- [ ] Headless режим

## 🤝 Contributing

Pull requests приветствуются! Для больших изменений сначала откройте issue.

## 📄 Лицензия

MIT License - см. [LICENSE](LICENSE)

## 🔗 Ссылки

- [Qt Documentation](https://doc.qt.io/)
- [Chromium Fingerprinting](https://chromium.googlesource.com/chromium/src/+/master/docs/fingerprinting.md)
- [AmneziaWG](https://github.com/amnezia-vpn/amneziawg)

## 💬 Поддержка

- GitHub Issues: [Issues](https://github.com/your-repo/issues)
- Telegram: @your_channel
- Email: support@example.com

---

**Версия:** 1.0.0  
**Дата:** 2024  
**Автор:** Your Name

⭐ Если проект полезен, поставьте звезду на GitHub!
