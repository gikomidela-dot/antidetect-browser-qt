#include "ShadowsocksParser.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QUrl>
#include <QDebug>

VPNConfig ShadowsocksParser::parse(const QString& filePath)
{
    VPNConfig config;
    config.type = "shadowsocks";
    config.configPath = filePath;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open Shadowsocks config:" << filePath;
        return config;
    }
    
    QFileInfo fileInfo(filePath);
    config.name = fileInfo.baseName();
    
    QString content = QString::fromUtf8(file.readAll());
    file.close();
    
    return parseFromJson(content);
}

VPNConfig ShadowsocksParser::parseFromJson(const QString& jsonString)
{
    VPNConfig config;
    config.type = "shadowsocks";
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!doc.isObject()) {
        qWarning() << "Invalid Shadowsocks JSON config";
        return config;
    }
    
    QJsonObject obj = doc.object();
    
    // Parse standard Shadowsocks JSON format
    if (obj.contains("server")) {
        config.parameters["server"] = obj["server"].toString();
    }
    
    if (obj.contains("server_port")) {
        config.parameters["port"] = QString::number(obj["server_port"].toInt());
    }
    
    if (obj.contains("password")) {
        config.parameters["password"] = obj["password"].toString();
    }
    
    if (obj.contains("method")) {
        config.parameters["method"] = obj["method"].toString();
    }
    
    if (obj.contains("plugin")) {
        config.parameters["plugin"] = obj["plugin"].toString();
    }
    
    if (obj.contains("plugin_opts")) {
        config.parameters["plugin_opts"] = obj["plugin_opts"].toString();
    }
    
    if (obj.contains("remarks")) {
        config.name = obj["remarks"].toString();
    }
    
    if (obj.contains("timeout")) {
        config.parameters["timeout"] = QString::number(obj["timeout"].toInt());
    }
    
    return config;
}

VPNConfig ShadowsocksParser::parseFromUrl(const QString& url)
{
    VPNConfig config;
    config.type = "shadowsocks";
    
    // Format: ss://base64(method:password)@server:port#tag
    if (!url.startsWith("ss://")) {
        qWarning() << "Invalid Shadowsocks URL format";
        return config;
    }
    
    QString urlData = url.mid(5); // Remove "ss://"
    
    // Extract tag if present
    int hashPos = urlData.indexOf('#');
    if (hashPos > 0) {
        config.name = QUrl::fromPercentEncoding(urlData.mid(hashPos + 1).toUtf8());
        urlData = urlData.left(hashPos);
    }
    
    // Split by @
    int atPos = urlData.indexOf('@');
    if (atPos < 0) {
        qWarning() << "Invalid Shadowsocks URL: missing @";
        return config;
    }
    
    QString encodedAuth = urlData.left(atPos);
    QString serverPart = urlData.mid(atPos + 1);
    
    // Decode base64 auth (method:password)
    QByteArray decodedAuth = QByteArray::fromBase64(encodedAuth.toUtf8());
    QString authString = QString::fromUtf8(decodedAuth);
    
    int colonPos = authString.indexOf(':');
    if (colonPos > 0) {
        config.parameters["method"] = authString.left(colonPos);
        config.parameters["password"] = authString.mid(colonPos + 1);
    }
    
    // Parse server:port
    colonPos = serverPart.lastIndexOf(':');
    if (colonPos > 0) {
        config.parameters["server"] = serverPart.left(colonPos);
        config.parameters["port"] = serverPart.mid(colonPos + 1);
    }
    
    return config;
}
