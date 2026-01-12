#include "Profile.h"
#include <QUuid>
#include <QJsonArray>

// FingerprintConfig implementation
QJsonObject FingerprintConfig::toJson() const
{
    QJsonObject json;
    json["userAgent"] = userAgent;
    json["platform"] = platform;
    json["vendor"] = vendor;
    json["renderer"] = renderer;
    json["hardwareConcurrency"] = hardwareConcurrency;
    json["deviceMemory"] = deviceMemory;
    json["screenWidth"] = screenWidth;
    json["screenHeight"] = screenHeight;
    json["colorDepth"] = colorDepth;
    json["timezone"] = timezone;
    json["locale"] = locale;
    json["webglVendor"] = webglVendor;
    json["webglRenderer"] = webglRenderer;
    json["canvasNoise"] = canvasNoise;
    json["audioNoise"] = audioNoise;
    json["webrtcProtection"] = webrtcProtection;
    json["tlsProfile"] = tlsProfile;
    return json;
}

FingerprintConfig FingerprintConfig::fromJson(const QJsonObject& json)
{
    FingerprintConfig config;
    config.userAgent = json["userAgent"].toString();
    config.platform = json["platform"].toString();
    config.vendor = json["vendor"].toString();
    config.renderer = json["renderer"].toString();
    config.hardwareConcurrency = json["hardwareConcurrency"].toInt(4);
    config.deviceMemory = json["deviceMemory"].toInt(8);
    config.screenWidth = json["screenWidth"].toInt(1920);
    config.screenHeight = json["screenHeight"].toInt(1080);
    config.colorDepth = json["colorDepth"].toInt(24);
    config.timezone = json["timezone"].toString();
    config.locale = json["locale"].toString();
    config.webglVendor = json["webglVendor"].toString();
    config.webglRenderer = json["webglRenderer"].toString();
    config.canvasNoise = json["canvasNoise"].toBool(true);
    config.audioNoise = json["audioNoise"].toBool(true);
    config.webrtcProtection = json["webrtcProtection"].toBool(true);
    config.tlsProfile = json["tlsProfile"].toString();
    return config;
}

// ProxyConfig implementation
QJsonObject ProxyConfig::toJson() const
{
    QJsonObject json;
    json["type"] = static_cast<int>(type);
    json["host"] = host;
    json["port"] = port;
    json["username"] = username;
    json["password"] = password;
    return json;
}

ProxyConfig ProxyConfig::fromJson(const QJsonObject& json)
{
    ProxyConfig config;
    config.type = static_cast<Type>(json["type"].toInt(0));
    config.host = json["host"].toString();
    config.port = json["port"].toInt(0);
    config.username = json["username"].toString();
    config.password = json["password"].toString();
    return config;
}

QString ProxyConfig::toString() const
{
    if (type == None) return "";
    
    QString typeStr;
    switch (type) {
        case HTTP: typeStr = "http"; break;
        case HTTPS: typeStr = "https"; break;
        case SOCKS4: typeStr = "socks4"; break;
        case SOCKS5: typeStr = "socks5"; break;
        default: return "";
    }
    
    QString result = QString("%1://%2:%3").arg(typeStr).arg(host).arg(port);
    if (!username.isEmpty()) {
        result = QString("%1://%2:%3@%4:%5")
            .arg(typeStr).arg(username).arg(password).arg(host).arg(port);
    }
    return result;
}

// Profile implementation
Profile::Profile()
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_createdAt(QDateTime::currentDateTime())
    , m_updatedAt(QDateTime::currentDateTime())
{
}

Profile::Profile(const QString& id, const QString& name)
    : m_id(id)
    , m_name(name)
    , m_createdAt(QDateTime::currentDateTime())
    , m_updatedAt(QDateTime::currentDateTime())
{
}

QJsonObject Profile::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["notes"] = m_notes;
    json["tags"] = QJsonArray::fromStringList(m_tags);
    json["fingerprint"] = m_fingerprint.toJson();
    json["proxy"] = m_proxy.toJson();
    json["vpnConfig"] = m_vpnConfig;
    json["createdAt"] = m_createdAt.toString(Qt::ISODate);
    json["updatedAt"] = m_updatedAt.toString(Qt::ISODate);
    return json;
}

Profile Profile::fromJson(const QJsonObject& json)
{
    Profile profile;
    profile.m_id = json["id"].toString();
    profile.m_name = json["name"].toString();
    profile.m_notes = json["notes"].toString();
    
    QJsonArray tagsArray = json["tags"].toArray();
    for (const auto& tag : tagsArray) {
        profile.m_tags.append(tag.toString());
    }
    
    profile.m_fingerprint = FingerprintConfig::fromJson(json["fingerprint"].toObject());
    profile.m_proxy = ProxyConfig::fromJson(json["proxy"].toObject());
    profile.m_vpnConfig = json["vpnConfig"].toString();
    profile.m_createdAt = QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate);
    profile.m_updatedAt = QDateTime::fromString(json["updatedAt"].toString(), Qt::ISODate);
    
    return profile;
}

bool Profile::isValid() const
{
    return !m_id.isEmpty() && !m_name.isEmpty();
}
