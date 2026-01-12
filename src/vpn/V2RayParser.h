#ifndef V2RAYPARSER_H
#define V2RAYPARSER_H

#include <QString>
#include "VPNManager.h"

class V2RayParser
{
public:
    // Parse from JSON file
    static VPNConfig parse(const QString& filePath);
    
    // Parse from vmess:// URL
    static VPNConfig parseFromVmessUrl(const QString& url);
    
    // Parse from vless:// URL
    static VPNConfig parseFromVlessUrl(const QString& url);
    
    // Parse from JSON string
    static VPNConfig parseFromJson(const QString& jsonString);
    
private:
    V2RayParser() = delete;
};

#endif // V2RAYPARSER_H
