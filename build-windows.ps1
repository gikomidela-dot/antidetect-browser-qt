# PowerShell script to build Antidetect Browser for Windows
# Run this on Windows with Qt and CMake installed

param(
    [string]$QtPath = "C:\Qt\6.5.0\mingw_64",
    [string]$OpenSSLPath = "C:\Program Files\OpenSSL-Win64",
    [string]$BuildType = "Release"
)

Write-Host "🔨 Building Antidetect Browser for Windows..." -ForegroundColor Green

# Check if Qt exists
if (-not (Test-Path $QtPath)) {
    Write-Host "❌ Qt not found at: $QtPath" -ForegroundColor Red
    Write-Host "Please install Qt from: https://www.qt.io/download-qt-installer" -ForegroundColor Yellow
    exit 1
}

# Check if CMake exists
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "❌ CMake not found" -ForegroundColor Red
    Write-Host "Please install CMake from: https://cmake.org/download/" -ForegroundColor Yellow
    exit 1
}

# Create build directory
$BuildDir = "build-windows"
if (Test-Path $BuildDir) {
    Write-Host "🗑️  Removing old build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $BuildDir
}

New-Item -ItemType Directory -Path $BuildDir | Out-Null
Set-Location $BuildDir

# Configure CMake
Write-Host "⚙️  Configuring CMake..." -ForegroundColor Cyan
cmake .. -G "MinGW Makefiles" `
    -DCMAKE_PREFIX_PATH="$QtPath" `
    -DCMAKE_BUILD_TYPE=$BuildType `
    -DOPENSSL_ROOT_DIR="$OpenSSLPath"

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ CMake configuration failed" -ForegroundColor Red
    exit 1
}

# Build
Write-Host "🔧 Building..." -ForegroundColor Cyan
cmake --build . --config $BuildType -j8

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Build failed" -ForegroundColor Red
    exit 1
}

# Deploy Qt dependencies
Write-Host "📦 Deploying Qt dependencies..." -ForegroundColor Cyan
$ExePath = "AntidetectBrowser.exe"
if ($BuildType -eq "Release") {
    $ExePath = "Release\AntidetectBrowser.exe"
}

& "$QtPath\bin\windeployqt.exe" --release --webenginewidgets $ExePath

# Create deployment directory
Write-Host "📁 Creating portable package..." -ForegroundColor Cyan
$DeployDir = "AntidetectBrowser-Portable"
if (Test-Path $DeployDir) {
    Remove-Item -Recurse -Force $DeployDir
}

New-Item -ItemType Directory -Path $DeployDir | Out-Null

# Copy executable and dependencies
if ($BuildType -eq "Release") {
    Copy-Item -Recurse "Release\*" $DeployDir
} else {
    Copy-Item $ExePath $DeployDir
}

# Copy OpenSSL DLLs
if (Test-Path "$OpenSSLPath\bin") {
    Copy-Item "$OpenSSLPath\bin\libssl-3-x64.dll" $DeployDir -ErrorAction SilentlyContinue
    Copy-Item "$OpenSSLPath\bin\libcrypto-3-x64.dll" $DeployDir -ErrorAction SilentlyContinue
}

# Copy MinGW runtime
Copy-Item "$QtPath\bin\libgcc_s_seh-1.dll" $DeployDir -ErrorAction SilentlyContinue
Copy-Item "$QtPath\bin\libstdc++-6.dll" $DeployDir -ErrorAction SilentlyContinue
Copy-Item "$QtPath\bin\libwinpthread-1.dll" $DeployDir -ErrorAction SilentlyContinue

# Create README
$ReadmeContent = @"
Antidetect Browser - Portable Edition
======================================

How to run:
1. Double-click AntidetectBrowser.exe
2. Create a profile
3. Launch browser with your fingerprint

Features:
- Browser fingerprint spoofing
- Proxy support (HTTP/HTTPS/SOCKS4/SOCKS5)
- VPN integration (OpenVPN, WireGuard, AmneziaWG)
- Cookie import/export
- TLS fingerprinting
- Timezone spoofing
- Canvas/WebGL/Audio noise

System Requirements:
- Windows 10/11 (64-bit)
- 4 GB RAM minimum
- 500 MB disk space

Support: https://github.com/your-repo

Version: 1.0.0
"@

Set-Content -Path "$DeployDir\README.txt" -Value $ReadmeContent

# Create ZIP archive
Write-Host "🗜️  Creating ZIP archive..." -ForegroundColor Cyan
$ZipPath = "AntidetectBrowser-Windows-x64.zip"
if (Test-Path $ZipPath) {
    Remove-Item $ZipPath
}

Compress-Archive -Path $DeployDir -DestinationPath $ZipPath

Write-Host ""
Write-Host "✅ Build complete!" -ForegroundColor Green
Write-Host "📦 Portable folder: $BuildDir\$DeployDir" -ForegroundColor Cyan
Write-Host "🗜️  ZIP archive: $BuildDir\$ZipPath" -ForegroundColor Cyan
Write-Host ""
Write-Host "To test, run:" -ForegroundColor Yellow
Write-Host "  cd $BuildDir\$DeployDir" -ForegroundColor Yellow
Write-Host "  .\AntidetectBrowser.exe" -ForegroundColor Yellow

Set-Location ..
