#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QString>
#include <memory>

class ProfileManager;
class FingerprintManager;
class ProxyManager;
class VPNManager;
class RestApiServer;
class BrowserWindowManager;
class TLSFingerprintManager;
class CookieManager;

class Application : public QObject
{
    Q_OBJECT
    
public:
    static Application& instance();
    
    void initialize();
    void shutdown();
    
    ProfileManager* profileManager() const { return m_profileManager.get(); }
    FingerprintManager* fingerprintManager() const { return m_fingerprintManager.get(); }
    ProxyManager* proxyManager() const { return m_proxyManager.get(); }
    VPNManager* vpnManager() const { return m_vpnManager.get(); }
    RestApiServer* apiServer() const { return m_apiServer.get(); }
    BrowserWindowManager* browserWindowManager() const { return m_browserWindowManager.get(); }
    TLSFingerprintManager* tlsFingerprintManager() const { return m_tlsFingerprintManager.get(); }
    CookieManager* cookieManager() const { return m_cookieManager.get(); }
    
    QString dataDirectory() const { return m_dataDirectory; }
    
signals:
    void initialized();
    void shutdownRequested();
    
private:
    Application();
    ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    
    void setupDataDirectory();
    void initializeManagers();
    
    QString m_dataDirectory;
    std::unique_ptr<ProfileManager> m_profileManager;
    std::unique_ptr<FingerprintManager> m_fingerprintManager;
    std::unique_ptr<ProxyManager> m_proxyManager;
    std::unique_ptr<VPNManager> m_vpnManager;
    std::unique_ptr<RestApiServer> m_apiServer;
    std::unique_ptr<BrowserWindowManager> m_browserWindowManager;
    std::unique_ptr<TLSFingerprintManager> m_tlsFingerprintManager;
    std::unique_ptr<CookieManager> m_cookieManager;
};

#endif // APPLICATION_H
