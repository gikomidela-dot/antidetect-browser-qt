# Building Antidetect Browser for Windows

## Метод 1: Сборка на Windows (Рекомендуется)

### Требования:
1. **Qt 6.5+** с MinGW или MSVC
   - Скачать: https://www.qt.io/download-qt-installer
   - Компоненты: Qt WebEngine, Qt Network, Qt SQL
   
2. **CMake 3.16+**
   - Скачать: https://cmake.org/download/
   
3. **OpenSSL для Windows**
   - Скачать: https://slproweb.com/products/Win32OpenSSL.html
   - Установить Win64 OpenSSL v3.x.x
   
4. **SQLite3**
   - Обычно включен в Qt

### Шаги сборки:

#### 1. Клонировать проект
```cmd
git clone <repository-url>
cd antidetect-browser-qt
```

#### 2. Создать build директорию
```cmd
mkdir build-windows
cd build-windows
```

#### 3. Настроить CMake
```cmd
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/mingw_64" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DOPENSSL_ROOT_DIR="C:/Program Files/OpenSSL-Win64"
```

Или для MSVC:
```cmd
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2019_64" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DOPENSSL_ROOT_DIR="C:/Program Files/OpenSSL-Win64"
```

#### 4. Собрать проект
```cmd
cmake --build . --config Release -j8
```

#### 5. Собрать все зависимости в один каталог
```cmd
windeployqt --release --webenginewidgets AntidetectBrowser.exe
```

### Создание portable версии:

Создайте `deploy.bat`:
```batch
@echo off
echo Deploying Antidetect Browser...

REM Create deployment directory
mkdir deploy
copy Release\AntidetectBrowser.exe deploy\

REM Deploy Qt dependencies
cd deploy
windeployqt --release --webenginewidgets AntidetectBrowser.exe

REM Copy OpenSSL DLLs
copy "C:\Program Files\OpenSSL-Win64\bin\libssl-3-x64.dll" .
copy "C:\Program Files\OpenSSL-Win64\bin\libcrypto-3-x64.dll" .

REM Copy MinGW runtime (if using MinGW)
copy "C:\Qt\6.5.0\mingw_64\bin\libgcc_s_seh-1.dll" .
copy "C:\Qt\6.5.0\mingw_64\bin\libstdc++-6.dll" .
copy "C:\Qt\6.5.0\mingw_64\bin\libwinpthread-1.dll" .

echo Done! Portable version in 'deploy' folder
pause
```

Запустите:
```cmd
deploy.bat
```

---

## Метод 2: Docker сборка (Экспериментально)

### Требования:
- Docker Desktop для Windows
- WSL2

### Dockerfile для Windows сборки:

```dockerfile
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    mingw-w64 \
    git \
    wget \
    unzip \
    python3

# Download and install Qt for Windows
RUN wget https://download.qt.io/official_releases/qt/6.5/6.5.0/single/qt-everywhere-src-6.5.0.tar.xz
# ... (сложная настройка Qt)

# Build project
WORKDIR /build
COPY . .
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../windows-toolchain.cmake && \
    cmake --build .
```

**Примечание:** Qt WebEngine очень сложно собрать статически. Рекомендуется использовать Метод 1.

---

## Метод 3: GitHub Actions (CI/CD)

Создайте `.github/workflows/build-windows.yml`:

```yaml
name: Build Windows

on: [push, pull_request]

jobs:
  build:
    runs-on: windows-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install Qt
      uses: jurplel/install-qt-action@v3
      with:
        version: '6.5.0'
        host: 'windows'
        target: 'desktop'
        arch: 'win64_mingw'
        modules: 'qtwebengine'
    
    - name: Install OpenSSL
      run: |
        choco install openssl
    
    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
    
    - name: Build
      run: |
        cd build
        cmake --build . --config Release
    
    - name: Deploy
      run: |
        cd build
        windeployqt --release --webenginewidgets AntidetectBrowser.exe
    
    - name: Upload artifact
      uses: actions/upload-artifact@v3
      with:
        name: AntidetectBrowser-Windows
        path: build/
```

---

## Уменьшение размера сборки

### 1. Удалить ненужные Qt модули
В `CMakeLists.txt` оставьте только необходимые:
```cmake
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Gui 
    Widgets 
    Network 
    Sql 
    WebEngineWidgets
    WebEngineCore
)
```

### 2. Использовать UPX для сжатия
```cmd
upx --best --lzma AntidetectBrowser.exe
```

### 3. Удалить debug символы
```cmd
strip AntidetectBrowser.exe
```

---

## Размер финальной сборки

- **С Qt WebEngine**: ~150-200 MB
- **Без WebEngine** (только WebView): ~50-80 MB
- **После UPX сжатия**: ~60-100 MB

---

## Troubleshooting

### Ошибка: "Qt6WebEngine not found"
**Решение:** Установите Qt WebEngine через Qt Maintenance Tool

### Ошибка: "OpenSSL not found"
**Решение:** 
```cmd
set OPENSSL_ROOT_DIR=C:\Program Files\OpenSSL-Win64
```

### Ошибка: "Missing DLLs"
**Решение:** Запустите `windeployqt` еще раз:
```cmd
windeployqt --release --webenginewidgets --compiler-runtime AntidetectBrowser.exe
```

### Приложение не запускается
**Решение:** Проверьте зависимости с помощью Dependency Walker:
- Скачать: http://www.dependencywalker.com/
- Откройте AntidetectBrowser.exe и проверьте отсутствующие DLL

---

## Автоматический установщик (NSIS)

Создайте `installer.nsi`:

```nsis
!define APP_NAME "Antidetect Browser"
!define APP_VERSION "1.0.0"
!define APP_PUBLISHER "Your Company"
!define APP_EXE "AntidetectBrowser.exe"

Name "${APP_NAME}"
OutFile "AntidetectBrowser-Setup.exe"
InstallDir "$PROGRAMFILES64\${APP_NAME}"

Section "Install"
    SetOutPath "$INSTDIR"
    File /r "deploy\*.*"
    
    CreateDirectory "$SMPROGRAMS\${APP_NAME}"
    CreateShortcut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}"
    CreateShortcut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}"
    
    WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

Section "Uninstall"
    Delete "$INSTDIR\*.*"
    RMDir /r "$INSTDIR"
    Delete "$SMPROGRAMS\${APP_NAME}\*.*"
    RMDir "$SMPROGRAMS\${APP_NAME}"
    Delete "$DESKTOP\${APP_NAME}.lnk"
SectionEnd
```

Собрать установщик:
```cmd
makensis installer.nsi
```

---

## Рекомендации

1. **Используйте Qt 6.5+ LTS** для стабильности
2. **Тестируйте на чистой Windows** без установленного Qt
3. **Включите все DLL** в папку с exe
4. **Подпишите exe** цифровой подписью для избежания SmartScreen
5. **Создайте portable версию** в ZIP архиве

---

**Готово!** Теперь у вас есть полностью автономная Windows сборка Antidetect Browser.
