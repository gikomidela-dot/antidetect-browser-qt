#include "Application.h"
#include "profiles/ProfileManager.h"
#include "fingerprint/FingerprintManager.h"
#include "proxy/ProxyManager.h"
#include "vpn/VPNManager.h"
#include "api/RestApiServer.h"
#include "BrowserWindowManager.h"
#include "tls/TLSFingerprintManager.h"
#include "CookieManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

Application& Application::instance()
{
    static Application instance;
    return instance;
}

Application::Application()
    : QObject(nullptr)
{
}

Application::~Application()
{
    shutdown();
}

void Application::initialize()
{
    qDebug() << "Initializing Antidetect Browser...";
    
    setupDataDirectory();
    initializeManagers();
    
    // Start API server
    if (m_apiServer) {
        m_apiServer->start(8080);
    }
    
    emit initialized();
    qDebug() << "Antidetect Browser initialized successfully";
}

void Application::shutdown()
{
    qDebug() << "Shutting down Antidetect Browser...";
    
    if (m_apiServer) {
        m_apiServer->stop();
    }
    
    emit shutdownRequested();
}

void Application::setupDataDirectory()
{
    m_dataDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    
    QDir dir;
    if (!dir.exists(m_dataDirectory)) {
        dir.mkpath(m_dataDirectory);
    }
    
    // Create subdirectories
    dir.mkpath(m_dataDirectory + "/profiles");
    dir.mkpath(m_dataDirectory + "/sessions");
    dir.mkpath(m_dataDirectory + "/cookies");
    dir.mkpath(m_dataDirectory + "/cache");
    
    qDebug() << "Data directory:" << m_dataDirectory;
}

void Application::initializeManagers()
{
    m_profileManager = std::make_unique<ProfileManager>(m_dataDirectory + "/profiles");
    m_fingerprintManager = std::make_unique<FingerprintManager>();
    m_proxyManager = std::make_unique<ProxyManager>();
    m_vpnManager = std::make_unique<VPNManager>();
    m_apiServer = std::make_unique<RestApiServer>();
    m_browserWindowManager = std::make_unique<BrowserWindowManager>();
    m_tlsFingerprintManager = std::make_unique<TLSFingerprintManager>();
    m_cookieManager = std::make_unique<CookieManager>();
}
