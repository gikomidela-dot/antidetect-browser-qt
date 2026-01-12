cmake_minimum_required(VERSION 3.16)
project(SECTAAntiDetect VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Gui 
    Widgets 
    Network 
    Sql 
    WebEngineWidgets
    WebEngineCore
)

# OpenSSL for encryption (optional, Qt has built-in SSL)
find_package(OpenSSL)

# SQLite3 (optional, Qt has built-in SQLite)
find_package(SQLite3)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/profiles
    ${CMAKE_SOURCE_DIR}/src/crypto
    ${CMAKE_SOURCE_DIR}/src/fingerprint
    ${CMAKE_SOURCE_DIR}/src/proxy
    ${CMAKE_SOURCE_DIR}/src/vpn
    ${CMAKE_SOURCE_DIR}/src/tls
    ${CMAKE_SOURCE_DIR}/src/api
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/automation
)

# Source files
set(SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/core/BrowserWindowManager.cpp
    src/core/CookieManager.cpp
    src/profiles/ProfileManager.cpp
    src/profiles/ProfileStorage.cpp
    src/profiles/Profile.cpp
    src/profiles/UserAgentDatabase.cpp
    src/crypto/Encryption.cpp
    src/crypto/PasswordHash.cpp
    src/fingerprint/FingerprintManager.cpp
    src/fingerprint/CanvasSpoofing.cpp
    src/fingerprint/WebGLSpoofing.cpp
    src/fingerprint/AudioSpoofing.cpp
    src/proxy/ProxyManager.cpp
    src/proxy/ProxyChecker.cpp
    src/vpn/VPNManager.cpp
    src/vpn/OpenVPNParser.cpp
    src/vpn/WireGuardParser.cpp
    src/vpn/AmneziaWGParser.cpp
    src/vpn/ShadowsocksParser.cpp
    src/vpn/V2RayParser.cpp
    src/tls/TLSFingerprintManager.cpp
    src/api/RestApiServer.cpp
    src/api/ApiHandler.cpp
    src/ui/MainWindow.cpp
    src/ui/ProfileEditor.cpp
    src/ui/Dashboard.cpp
    src/ui/BrowserWindow.cpp
    src/ui/VPNImportDialog.cpp
    src/ui/ProxyImportDialog.cpp
    src/automation/BrowserAutomation.cpp
    src/automation/CookieRobot.cpp
)

set(HEADERS
    src/core/Application.h
    src/core/BrowserWindowManager.h
    src/core/CookieManager.h
    src/profiles/ProfileManager.h
    src/profiles/ProfileStorage.h
    src/profiles/Profile.h
    src/profiles/UserAgentDatabase.h
    src/crypto/Encryption.h
    src/crypto/PasswordHash.h
    src/fingerprint/FingerprintManager.h
    src/fingerprint/CanvasSpoofing.h
    src/fingerprint/WebGLSpoofing.h
    src/fingerprint/AudioSpoofing.h
    src/proxy/ProxyManager.h
    src/proxy/ProxyChecker.h
    src/vpn/VPNManager.h
    src/vpn/OpenVPNParser.h
    src/vpn/WireGuardParser.h
    src/vpn/AmneziaWGParser.h
    src/vpn/ShadowsocksParser.h
    src/vpn/V2RayParser.h
    src/tls/TLSFingerprintManager.h
    src/api/RestApiServer.h
    src/api/ApiHandler.h
    src/ui/MainWindow.h
    src/ui/ProfileEditor.h
    src/ui/Dashboard.h
    src/ui/BrowserWindow.h
    src/ui/VPNImportDialog.h
    src/ui/ProxyImportDialog.h
    src/automation/BrowserAutomation.h
    src/automation/CookieRobot.h
)

# UI files
set(UI_FILES
    src/ui/MainWindow.ui
    src/ui/ProfileEditor.ui
    src/ui/Dashboard.ui
)

# Create executable
add_executable(${PROJECT_NAME} 
    ${SOURCES} 
    ${HEADERS}
    ${UI_FILES}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
    Qt6::WebEngineWidgets
    Qt6::WebEngineCore
)

# Link OpenSSL if found
if(OpenSSL_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
    )
endif()

# Link SQLite3 if found
if(SQLite3_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        SQLite::SQLite3
    )
endif()

# Install
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
