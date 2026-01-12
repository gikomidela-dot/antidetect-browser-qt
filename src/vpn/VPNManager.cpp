#include "VPNManager.h"
#include "OpenVPNParser.h"
#include "WireGuardParser.h"
#include "AmneziaWGParser.h"
#include "ShadowsocksParser.h"
#include "V2RayParser.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>

VPNManager::VPNManager(QObject* parent)
    : QObject(parent)
{
}

bool VPNManager::importConfig(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();
    
    QString type;
    if (ext == "ovpn") {
        type = "openvpn";
    } else if (ext == "conf") {
        type = "wireguard";
    } else if (ext == "json") {
        // Try to detect if it's Shadowsocks or V2Ray
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QString content = QString::fromUtf8(file.readAll());
            file.close();
            
            if (content.contains("\"outbounds\"") || content.contains("\"inbounds\"")) {
                type = "v2ray";
            } else {
                type = "shadowsocks";
            }
        } else {
            type = "shadowsocks";
        }
    } else {
        emit importError("Unsupported file type: " + ext);
        return false;
    }
    
    VPNConfig config = parseConfig(filePath, type);
    
    if (config.name.isEmpty()) {
        config.name = fileInfo.baseName();
    }
    
    m_configs[config.name] = config;
    emit configImported(config.name);
    
    return true;
}

VPNConfig VPNManager::parseConfig(const QString& filePath, const QString& type)
{
    VPNConfig config;
    config.type = type;
    config.configPath = filePath;
    
    if (type == "openvpn") {
        config = OpenVPNParser::parse(filePath);
    } else if (type == "wireguard") {
        // Try AmneziaWG first, fallback to standard WireGuard
        QString name;
        QMap<QString, QString> params;
        if (AmneziaWGParser::parse(filePath, name, params)) {
            config.name = name;
            config.type = params.value("type", "wireguard");
            config.parameters = params;
            config.configPath = filePath;
        } else {
            config = WireGuardParser::parse(filePath);
        }
    } else if (type == "shadowsocks") {
        config = ShadowsocksParser::parse(filePath);
    } else if (type == "v2ray") {
        config = V2RayParser::parse(filePath);
    } else {
        qWarning() << "Unknown VPN type:" << type;
    }
    
    return config;
}

bool VPNManager::importFromUrl(const QString& url)
{
    VPNConfig config = parseFromUrl(url);
    
    if (config.name.isEmpty()) {
        config.name = "Imported VPN " + QString::number(m_configs.size() + 1);
    }
    
    if (config.type.isEmpty()) {
        emit importError("Failed to parse VPN URL");
        return false;
    }
    
    m_configs[config.name] = config;
    emit configImported(config.name);
    
    return true;
}

VPNConfig VPNManager::parseFromUrl(const QString& url)
{
    VPNConfig config;
    
    if (url.startsWith("ss://")) {
        config = ShadowsocksParser::parseFromUrl(url);
    } else if (url.startsWith("vmess://")) {
        config = V2RayParser::parseFromVmessUrl(url);
    } else if (url.startsWith("vless://")) {
        config = V2RayParser::parseFromVlessUrl(url);
    } else {
        qWarning() << "Unknown VPN URL scheme:" << url;
    }
    
    return config;
}

bool VPNManager::removeConfig(const QString& name)
{
    if (m_configs.contains(name)) {
        m_configs.remove(name);
        return true;
    }
    return false;
}
