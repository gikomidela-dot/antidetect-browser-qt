#include "OpenVPNParser.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QRegularExpression>
#include <QDebug>

VPNConfig OpenVPNParser::parse(const QString& filePath)
{
    VPNConfig config;
    config.type = "openvpn";
    config.configPath = filePath;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open OpenVPN config:" << filePath;
        return config;
    }
    
    QTextStream in(&file);
    QFileInfo fileInfo(filePath);
    config.name = fileInfo.baseName();
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }
        
        QStringList parts = line.split(QRegularExpression("\\s+"));
        if (parts.isEmpty()) continue;
        
        QString key = parts[0];
        QString value = parts.size() > 1 ? parts.mid(1).join(" ") : "";
        
        config.parameters[key] = value;
        
        // Extract common parameters
        if (key == "remote") {
            if (parts.size() >= 2) {
                config.parameters["server"] = parts[1];
                if (parts.size() >= 3) {
                    config.parameters["port"] = parts[2];
                }
            }
        }
    }
    
    file.close();
    return config;
}
