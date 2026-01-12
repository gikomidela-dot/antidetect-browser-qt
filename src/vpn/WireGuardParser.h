#ifndef WIREGUARDPARSER_H
#define WIREGUARDPARSER_H

#include <QString>
#include "VPNManager.h"

class WireGuardParser
{
public:
    static VPNConfig parse(const QString& filePath);
    static VPNConfig parseFromString(const QString& configString);
    
private:
    WireGuardParser() = delete;
    static void parseSection(VPNConfig& config, const QString& section, const QStringList& lines);
};

#endif // WIREGUARDPARSER_H
