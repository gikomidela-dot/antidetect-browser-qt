#ifndef OPENVPNPARSER_H
#define OPENVPNPARSER_H

#include <QString>
#include "VPNManager.h"

class OpenVPNParser
{
public:
    static VPNConfig parse(const QString& filePath);
    
private:
    OpenVPNParser() = delete;
};

#endif // OPENVPNPARSER_H
