#include "ProxyManager.h"
#include <QRegularExpression>
#include <QDebug>

ProxyManager::ProxyManager(QObject* parent)
    : QObject(parent)
{
}

QNetworkProxy ProxyManager::createProxy(const ProxyConfig& config)
{
    QNetworkProxy proxy;
    
    if (config.type == ProxyConfig::None) {
        proxy.setType(QNetworkProxy::NoProxy);
        return proxy;
    }
    
    switch (config.type) {
        case ProxyConfig::HTTP:
        case ProxyConfig::HTTPS:
            proxy.setType(QNetworkProxy::HttpProxy);
            break;
        case ProxyConfig::SOCKS4:
        case ProxyConfig::SOCKS5:
            proxy.setType(QNetworkProxy::Socks5Proxy);
            break;
        default:
            proxy.setType(QNetworkProxy::NoProxy);
            return proxy;
    }
    
    proxy.setHostName(config.host);
    proxy.setPort(config.port);
    
    if (!config.username.isEmpty()) {
        proxy.setUser(config.username);
        proxy.setPassword(config.password);
    }
    
    return proxy;
}

ProxyConfig ProxyManager::parseProxyString(const QString& proxyStr)
{
    ProxyConfig config;
    config.type = ProxyConfig::None;
    
    // Format: type://[user:pass@]host:port
    QRegularExpression re(R"(^(http|https|socks4|socks5)://(?:([^:]+):([^@]+)@)?([^:]+):(\d+)$)");
    QRegularExpressionMatch match = re.match(proxyStr);
    
    if (!match.hasMatch()) {
        qWarning() << "Invalid proxy string:" << proxyStr;
        return config;
    }
    
    QString type = match.captured(1).toLower();
    if (type == "http") config.type = ProxyConfig::HTTP;
    else if (type == "https") config.type = ProxyConfig::HTTPS;
    else if (type == "socks4") config.type = ProxyConfig::SOCKS4;
    else if (type == "socks5") config.type = ProxyConfig::SOCKS5;
    
    config.username = match.captured(2);
    config.password = match.captured(3);
    config.host = match.captured(4);
    config.port = match.captured(5).toInt();
    
    return config;
}

QString ProxyManager::formatProxy(const ProxyConfig& config)
{
    if (config.type == ProxyConfig::None) {
        return "No proxy";
    }
    
    QString typeStr;
    switch (config.type) {
        case ProxyConfig::HTTP: typeStr = "HTTP"; break;
        case ProxyConfig::HTTPS: typeStr = "HTTPS"; break;
        case ProxyConfig::SOCKS4: typeStr = "SOCKS4"; break;
        case ProxyConfig::SOCKS5: typeStr = "SOCKS5"; break;
        default: return "Unknown";
    }
    
    QString result = QString("%1://%2:%3").arg(typeStr).arg(config.host).arg(config.port);
    
    if (!config.username.isEmpty()) {
        result += QString(" (auth: %1)").arg(config.username);
    }
    
    return result;
}
