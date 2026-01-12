#include "V2RayParser.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

VPNConfig V2RayParser::parse(const QString& filePath)
{
    VPNConfig config;
    config.type = "v2ray";
    config.configPath = filePath;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open V2Ray config:" << filePath;
        return config;
    }
    
    QFileInfo fileInfo(filePath);
    config.name = fileInfo.baseName();
    
    QString content = QString::fromUtf8(file.readAll());
    file.close();
    
    return parseFromJson(content);
}

VPNConfig V2RayParser::parseFromJson(const QString& jsonString)
{
    VPNConfig config;
    config.type = "v2ray";
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!doc.isObject()) {
        qWarning() << "Invalid V2Ray JSON config";
        return config;
    }
    
    QJsonObject obj = doc.object();
    
    // Parse outbounds
    if (obj.contains("outbounds")) {
        QJsonArray outbounds = obj["outbounds"].toArray();
        if (!outbounds.isEmpty()) {
            QJsonObject outbound = outbounds[0].toObject();
            
            if (outbound.contains("protocol")) {
                config.parameters["protocol"] = outbound["protocol"].toString();
            }
            
            if (outbound.contains("settings")) {
                QJsonObject settings = outbound["settings"].toObject();
                
                if (settings.contains("vnext")) {
                    QJsonArray vnext = settings["vnext"].toArray();
                    if (!vnext.isEmpty()) {
                        QJsonObject server = vnext[0].toObject();
                        
                        config.parameters["server"] = server["address"].toString();
                        config.parameters["port"] = QString::number(server["port"].toInt());
                        
                        if (server.contains("users")) {
                            QJsonArray users = server["users"].toArray();
                            if (!users.isEmpty()) {
                                QJsonObject user = users[0].toObject();
                                config.parameters["id"] = user["id"].toString();
                                config.parameters["alterId"] = QString::number(user["alterId"].toInt());
                                config.parameters["security"] = user["security"].toString();
                            }
                        }
                    }
                }
            }
            
            if (outbound.contains("streamSettings")) {
                QJsonObject streamSettings = outbound["streamSettings"].toObject();
                config.parameters["network"] = streamSettings["network"].toString();
                
                if (streamSettings.contains("security")) {
                    config.parameters["tls"] = streamSettings["security"].toString();
                }
                
                if (streamSettings.contains("wsSettings")) {
                    QJsonObject wsSettings = streamSettings["wsSettings"].toObject();
                    config.parameters["path"] = wsSettings["path"].toString();
                    
                    if (wsSettings.contains("headers")) {
                        QJsonObject headers = wsSettings["headers"].toObject();
                        config.parameters["host"] = headers["Host"].toString();
                    }
                }
            }
        }
    }
    
    return config;
}

VPNConfig V2RayParser::parseFromVmessUrl(const QString& url)
{
    VPNConfig config;
    config.type = "v2ray";
    config.parameters["protocol"] = "vmess";
    
    // Format: vmess://base64(json)
    if (!url.startsWith("vmess://")) {
        qWarning() << "Invalid VMess URL format";
        return config;
    }
    
    QString base64Data = url.mid(8); // Remove "vmess://"
    QByteArray decoded = QByteArray::fromBase64(base64Data.toUtf8());
    QString jsonString = QString::fromUtf8(decoded);
    
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!doc.isObject()) {
        qWarning() << "Invalid VMess JSON";
        return config;
    }
    
    QJsonObject obj = doc.object();
    
    config.name = obj["ps"].toString();
    config.parameters["server"] = obj["add"].toString();
    config.parameters["port"] = obj["port"].toString();
    config.parameters["id"] = obj["id"].toString();
    config.parameters["alterId"] = obj["aid"].toString();
    config.parameters["network"] = obj["net"].toString();
    config.parameters["type"] = obj["type"].toString();
    config.parameters["host"] = obj["host"].toString();
    config.parameters["path"] = obj["path"].toString();
    config.parameters["tls"] = obj["tls"].toString();
    config.parameters["sni"] = obj["sni"].toString();
    
    return config;
}

VPNConfig V2RayParser::parseFromVlessUrl(const QString& url)
{
    VPNConfig config;
    config.type = "v2ray";
    config.parameters["protocol"] = "vless";
    
    // Format: vless://uuid@server:port?params#name
    if (!url.startsWith("vless://")) {
        qWarning() << "Invalid VLESS URL format";
        return config;
    }
    
    QUrl parsedUrl(url);
    
    // Extract name from fragment
    config.name = QUrl::fromPercentEncoding(parsedUrl.fragment().toUtf8());
    
    // Extract UUID (username)
    config.parameters["id"] = parsedUrl.userName();
    
    // Extract server and port
    config.parameters["server"] = parsedUrl.host();
    config.parameters["port"] = QString::number(parsedUrl.port());
    
    // Parse query parameters
    QUrlQuery query(parsedUrl);
    
    if (query.hasQueryItem("type")) {
        config.parameters["network"] = query.queryItemValue("type");
    }
    
    if (query.hasQueryItem("security")) {
        config.parameters["tls"] = query.queryItemValue("security");
    }
    
    if (query.hasQueryItem("path")) {
        config.parameters["path"] = QUrl::fromPercentEncoding(query.queryItemValue("path").toUtf8());
    }
    
    if (query.hasQueryItem("host")) {
        config.parameters["host"] = query.queryItemValue("host");
    }
    
    if (query.hasQueryItem("sni")) {
        config.parameters["sni"] = query.queryItemValue("sni");
    }
    
    if (query.hasQueryItem("encryption")) {
        config.parameters["encryption"] = query.queryItemValue("encryption");
    }
    
    if (query.hasQueryItem("flow")) {
        config.parameters["flow"] = query.queryItemValue("flow");
    }
    
    return config;
}
