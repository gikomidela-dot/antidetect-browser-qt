#include "AmneziaWGParser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

bool AmneziaWGParser::parse(const QString& configPath, QString& name, QMap<QString, QString>& parameters)
{
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open AmneziaWG config file:" << configPath;
        return false;
    }
    
    QString content = QTextStream(&file).readAll();
    file.close();
    
    return parseFromText(content, name, parameters);
}

bool AmneziaWGParser::parseFromText(const QString& configText, QString& name, QMap<QString, QString>& parameters)
{
    QString currentSection;
    QStringList lines = configText.split('\n');
    
    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        
        // Skip empty lines and comments
        if (trimmed.isEmpty() || trimmed.startsWith('#')) {
            continue;
        }
        
        // Check for section headers
        if (trimmed.startsWith('[') && trimmed.endsWith(']')) {
            currentSection = trimmed.mid(1, trimmed.length() - 2).toLower();
            continue;
        }
        
        // Parse key-value pairs
        int equalPos = trimmed.indexOf('=');
        if (equalPos > 0) {
            QString key = trimmed.left(equalPos).trimmed();
            QString value = trimmed.mid(equalPos + 1).trimmed();
            
            // Store with section prefix
            QString fullKey = currentSection.isEmpty() ? key : currentSection + "." + key;
            parameters[fullKey.toLower()] = value;
            
            // Also store without prefix for compatibility
            parameters[key.toLower()] = value;
        }
    }
    
    // Extract name from endpoint or generate default
    if (parameters.contains("peer.endpoint")) {
        QString endpoint = parameters["peer.endpoint"];
        int colonPos = endpoint.indexOf(':');
        if (colonPos > 0) {
            name = "AmneziaWG - " + endpoint.left(colonPos);
        } else {
            name = "AmneziaWG - " + endpoint;
        }
    } else if (parameters.contains("endpoint")) {
        QString endpoint = parameters["endpoint"];
        int colonPos = endpoint.indexOf(':');
        if (colonPos > 0) {
            name = "AmneziaWG - " + endpoint.left(colonPos);
        } else {
            name = "AmneziaWG - " + endpoint;
        }
    } else {
        name = "AmneziaWG Config";
    }
    
    // Parse Amnezia-specific parameters
    // Jc (Junk packet count)
    if (parameters.contains("jc")) {
        parameters["amnezia.junkpacketcount"] = parameters["jc"];
    }
    
    // Jmin (Junk packet min size)
    if (parameters.contains("jmin")) {
        parameters["amnezia.junkpacketminsize"] = parameters["jmin"];
    }
    
    // Jmax (Junk packet max size)
    if (parameters.contains("jmax")) {
        parameters["amnezia.junkpacketmaxsize"] = parameters["jmax"];
    }
    
    // S1 (Init packet junk size)
    if (parameters.contains("s1")) {
        parameters["amnezia.initpacketjunksize"] = parameters["s1"];
    }
    
    // S2 (Response packet junk size)
    if (parameters.contains("s2")) {
        parameters["amnezia.responsepacketjunksize"] = parameters["s2"];
    }
    
    // H1, H2, H3, H4 (Init packet magic header)
    if (parameters.contains("h1")) {
        parameters["amnezia.initpacketmagicheader"] = parameters["h1"];
    }
    if (parameters.contains("h2")) {
        parameters["amnezia.responsepacketmagicheader"] = parameters["h2"];
    }
    if (parameters.contains("h3")) {
        parameters["amnezia.underloadpacketmagicheader"] = parameters["h3"];
    }
    if (parameters.contains("h4")) {
        parameters["amnezia.transportpacketmagicheader"] = parameters["h4"];
    }
    
    // Extract server and port
    QString endpoint = parameters.value("peer.endpoint", parameters.value("endpoint", ""));
    if (!endpoint.isEmpty()) {
        int colonPos = endpoint.lastIndexOf(':');
        if (colonPos > 0) {
            parameters["server"] = endpoint.left(colonPos);
            parameters["port"] = endpoint.mid(colonPos + 1);
        } else {
            parameters["server"] = endpoint;
            parameters["port"] = "51820"; // Default WireGuard port
        }
    }
    
    // Check if this is actually AmneziaWG (has Amnezia-specific parameters)
    bool isAmnezia = parameters.contains("jc") || 
                     parameters.contains("jmin") || 
                     parameters.contains("jmax") ||
                     parameters.contains("s1") ||
                     parameters.contains("s2") ||
                     parameters.contains("h1");
    
    if (isAmnezia) {
        parameters["type"] = "amneziawg";
        qDebug() << "Detected AmneziaWG configuration";
    } else {
        parameters["type"] = "wireguard";
        qDebug() << "Detected standard WireGuard configuration";
    }
    
    return !parameters.isEmpty();
}

void AmneziaWGParser::parseSection(const QString& section, const QString& content, QMap<QString, QString>& parameters)
{
    QStringList lines = content.split('\n');
    
    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        
        if (trimmed.isEmpty() || trimmed.startsWith('#')) {
            continue;
        }
        
        int equalPos = trimmed.indexOf('=');
        if (equalPos > 0) {
            QString key = trimmed.left(equalPos).trimmed().toLower();
            QString value = trimmed.mid(equalPos + 1).trimmed();
            
            QString fullKey = section.isEmpty() ? key : section + "." + key;
            parameters[fullKey] = value;
        }
    }
}
