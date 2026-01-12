@echo off
REM Batch script to build Antidetect Browser for Windows
REM Run this on Windows with Qt and CMake installed

setlocal

REM Configuration
set QT_PATH=C:\Qt\6.5.0\mingw_64
set OPENSSL_PATH=C:\Program Files\OpenSSL-Win64
set BUILD_TYPE=Release

echo ========================================
echo Building Antidetect Browser for Windows
echo ========================================
echo.

REM Check Qt
if not exist "%QT_PATH%" (
    echo [ERROR] Qt not found at: %QT_PATH%
    echo Please install Qt from: https://www.qt.io/download-qt-installer
    pause
    exit /b 1
)

REM Check CMake
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake not found
    echo Please install CMake from: https://cmake.org/download/
    pause
    exit /b 1
)

REM Create build directory
set BUILD_DIR=build-windows
if exist %BUILD_DIR% (
    echo Removing old build directory...
    rmdir /s /q %BUILD_DIR%
)

mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM Configure CMake
echo.
echo [1/4] Configuring CMake...
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_PREFIX_PATH="%QT_PATH%" ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DOPENSSL_ROOT_DIR="%OPENSSL_PATH%"

if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed
    pause
    exit /b 1
)

REM Build
echo.
echo [2/4] Building...
cmake --build . --config %BUILD_TYPE% -j8

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed
    pause
    exit /b 1
)

REM Deploy Qt dependencies
echo.
echo [3/4] Deploying Qt dependencies...
"%QT_PATH%\bin\windeployqt.exe" --release --webenginewidgets AntidetectBrowser.exe

REM Create portable package
echo.
echo [4/4] Creating portable package...
set DEPLOY_DIR=AntidetectBrowser-Portable
if exist %DEPLOY_DIR% (
    rmdir /s /q %DEPLOY_DIR%
)

mkdir %DEPLOY_DIR%

REM Copy files
xcopy /E /I /Y *.exe %DEPLOY_DIR%\
xcopy /E /I /Y *.dll %DEPLOY_DIR%\
if exist translations xcopy /E /I /Y translations %DEPLOY_DIR%\translations\
if exist iconengines xcopy /E /I /Y iconengines %DEPLOY_DIR%\iconengines\
if exist imageformats xcopy /E /I /Y imageformats %DEPLOY_DIR%\imageformats\
if exist platforms xcopy /E /I /Y platforms %DEPLOY_DIR%\platforms\
if exist styles xcopy /E /I /Y styles %DEPLOY_DIR%\styles\
if exist QtWebEngineProcess.exe copy QtWebEngineProcess.exe %DEPLOY_DIR%\

REM Copy OpenSSL DLLs
if exist "%OPENSSL_PATH%\bin\libssl-3-x64.dll" (
    copy "%OPENSSL_PATH%\bin\libssl-3-x64.dll" %DEPLOY_DIR%\
)
if exist "%OPENSSL_PATH%\bin\libcrypto-3-x64.dll" (
    copy "%OPENSSL_PATH%\bin\libcrypto-3-x64.dll" %DEPLOY_DIR%\
)

REM Copy MinGW runtime
copy "%QT_PATH%\bin\libgcc_s_seh-1.dll" %DEPLOY_DIR%\ 2>nul
copy "%QT_PATH%\bin\libstdc++-6.dll" %DEPLOY_DIR%\ 2>nul
copy "%QT_PATH%\bin\libwinpthread-1.dll" %DEPLOY_DIR%\ 2>nul

REM Create README
echo Antidetect Browser - Portable Edition > %DEPLOY_DIR%\README.txt
echo ====================================== >> %DEPLOY_DIR%\README.txt
echo. >> %DEPLOY_DIR%\README.txt
echo Double-click AntidetectBrowser.exe to run >> %DEPLOY_DIR%\README.txt
echo. >> %DEPLOY_DIR%\README.txt
echo Features: >> %DEPLOY_DIR%\README.txt
echo - Browser fingerprint spoofing >> %DEPLOY_DIR%\README.txt
echo - Proxy support >> %DEPLOY_DIR%\README.txt
echo - VPN integration >> %DEPLOY_DIR%\README.txt
echo - Cookie import/export >> %DEPLOY_DIR%\README.txt
echo - TLS fingerprinting >> %DEPLOY_DIR%\README.txt
echo. >> %DEPLOY_DIR%\README.txt
echo Version: 1.0.0 >> %DEPLOY_DIR%\README.txt

echo.
echo ========================================
echo Build Complete!
echo ========================================
echo.
echo Portable folder: %CD%\%DEPLOY_DIR%
echo.
echo To test, run:
echo   cd %DEPLOY_DIR%
echo   AntidetectBrowser.exe
echo.
pause

cd ..
