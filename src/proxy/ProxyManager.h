#ifndef PROXYMANAGER_H
#define PROXYMANAGER_H

#include <QObject>
#include <QNetworkProxy>
#include "profiles/Profile.h"

class ProxyManager : public QObject
{
    Q_OBJECT
    
public:
    explicit ProxyManager(QObject* parent = nullptr);
    
    // Configure proxy for web engine
    QNetworkProxy createProxy(const ProxyConfig& config);
    
    // Parse proxy string (format: type://user:pass@host:port)
    static ProxyConfig parseProxyString(const QString& proxyStr);
    
    // Format proxy for display
    static QString formatProxy(const ProxyConfig& config);
    
signals:
    void proxyConfigured(const QString& profileId);
    void proxyError(const QString& error);
};

#endif // PROXYMANAGER_H
