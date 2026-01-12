# 🚀 Quick Start - Windows Build

## Быстрая сборка (5 минут)

### Шаг 1: Установите зависимости

1. **Qt 6.5+** (с MinGW)
   - Скачать: https://www.qt.io/download-qt-installer
   - При установке выберите:
     - ✅ MinGW 11.2.0 64-bit
     - ✅ Qt WebEngine
     - ✅ Qt Network
     - ✅ Qt SQL

2. **CMake**
   - Скачать: https://cmake.org/download/
   - Выберите "Add CMake to PATH" при установке

3. **OpenSSL**
   - Скачать: https://slproweb.com/products/Win32OpenSSL.html
   - Установите "Win64 OpenSSL v3.x.x"

### Шаг 2: Соберите проект

Откройте PowerShell или CMD в папке проекта и запустите:

**PowerShell:**
```powershell
.\build-windows.ps1
```

**CMD:**
```cmd
build-windows.bat
```

### Шаг 3: Запустите

```cmd
cd build-windows\AntidetectBrowser-Portable
AntidetectBrowser.exe
```

---

## Ручная сборка

Если автоматический скрипт не работает:

```cmd
REM 1. Создать build директорию
mkdir build-windows
cd build-windows

REM 2. Настроить CMake
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/mingw_64" ^
    -DCMAKE_BUILD_TYPE=Release

REM 3. Собрать
cmake --build . --config Release -j8

REM 4. Развернуть зависимости
C:\Qt\6.5.0\mingw_64\bin\windeployqt.exe --release --webenginewidgets AntidetectBrowser.exe
```

---

## Troubleshooting

### ❌ "Qt6 not found"
**Решение:** Укажите правильный путь к Qt:
```powershell
.\build-windows.ps1 -QtPath "C:\Qt\6.5.0\mingw_64"
```

### ❌ "OpenSSL not found"
**Решение:** Установите OpenSSL или укажите путь:
```powershell
.\build-windows.ps1 -OpenSSLPath "C:\OpenSSL-Win64"
```

### ❌ "Missing DLL"
**Решение:** Запустите windeployqt еще раз:
```cmd
cd build-windows
C:\Qt\6.5.0\mingw_64\bin\windeployqt.exe --release --webenginewidgets --compiler-runtime AntidetectBrowser.exe
```

---

## Размер сборки

- **Полная сборка**: ~180 MB
- **После сжатия (ZIP)**: ~70 MB
- **С UPX сжатием**: ~50 MB

---

## Создание установщика

Использ��йте NSIS или Inno Setup для создания установщика:

```cmd
REM Установите NSIS
choco install nsis

REM Создайте установщик
makensis installer.nsi
```

---

**Готово!** Теперь у вас есть portable версия Antidetect Browser для Windows без зависимостей.

Все DLL включены в папку `AntidetectBrowser-Portable`.
