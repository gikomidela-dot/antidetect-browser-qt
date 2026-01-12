#include "WireGuardParser.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>

VPNConfig WireGuardParser::parse(const QString& filePath)
{
    VPNConfig config;
    config.type = "wireguard";
    config.configPath = filePath;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open WireGuard config:" << filePath;
        return config;
    }
    
    QFileInfo fileInfo(filePath);
    config.name = fileInfo.baseName();
    
    QString content = QString::fromUtf8(file.readAll());
    file.close();
    
    return parseFromString(content);
}

VPNConfig WireGuardParser::parseFromString(const QString& configString)
{
    VPNConfig config;
    config.type = "wireguard";
    
    QStringList lines = configString.split('\n');
    QString currentSection;
    QStringList sectionLines;
    
    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        
        // Skip empty lines and comments
        if (trimmed.isEmpty() || trimmed.startsWith("#")) {
            continue;
        }
        
        // Check for section header
        if (trimmed.startsWith("[") && trimmed.endsWith("]")) {
            // Process previous section
            if (!currentSection.isEmpty()) {
                parseSection(config, currentSection, sectionLines);
                sectionLines.clear();
            }
            
            currentSection = trimmed.mid(1, trimmed.length() - 2).toLower();
        } else {
            sectionLines.append(trimmed);
        }
    }
    
    // Process last section
    if (!currentSection.isEmpty()) {
        parseSection(config, currentSection, sectionLines);
    }
    
    return config;
}

void WireGuardParser::parseSection(VPNConfig& config, const QString& section, const QStringList& lines)
{
    for (const QString& line : lines) {
        int equalPos = line.indexOf('=');
        if (equalPos < 0) continue;
        
        QString key = line.left(equalPos).trimmed();
        QString value = line.mid(equalPos + 1).trimmed();
        
        QString fullKey = section + "." + key;
        config.parameters[fullKey] = value;
        
        // Extract important parameters
        if (section == "interface") {
            if (key.toLower() == "privatekey") {
                config.parameters["privateKey"] = value;
            } else if (key.toLower() == "address") {
                config.parameters["address"] = value;
            } else if (key.toLower() == "dns") {
                config.parameters["dns"] = value;
            }
        } else if (section == "peer") {
            if (key.toLower() == "publickey") {
                config.parameters["publicKey"] = value;
            } else if (key.toLower() == "endpoint") {
                config.parameters["endpoint"] = value;
                
                // Parse endpoint to get server and port
                int colonPos = value.lastIndexOf(':');
                if (colonPos > 0) {
                    config.parameters["server"] = value.left(colonPos);
                    config.parameters["port"] = value.mid(colonPos + 1);
                }
            } else if (key.toLower() == "allowedips") {
                config.parameters["allowedIPs"] = value;
            } else if (key.toLower() == "persistentkeepalive") {
                config.parameters["keepalive"] = value;
            }
        }
    }
}
