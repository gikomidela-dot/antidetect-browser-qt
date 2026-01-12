#ifndef SHADOWSOCKSPARSER_H
#define SHADOWSOCKSPARSER_H

#include <QString>
#include "VPNManager.h"

class ShadowsocksParser
{
public:
    // Parse from JSON file
    static VPNConfig parse(const QString& filePath);
    
    // Parse from ss:// URL
    static VPNConfig parseFromUrl(const QString& url);
    
    // Parse from JSON string
    static VPNConfig parseFromJson(const QString& jsonString);
    
private:
    ShadowsocksParser() = delete;
};

#endif // SHADOWSOCKSPARSER_H
