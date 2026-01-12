#ifndef WEBGLSPOOFING_H
#define WEBGLSPOOFING_H

#include <QString>

class WebGLSpoofing
{
public:
    static QString generateScript(const QString& vendor, const QString& renderer);
    
private:
    WebGLSpoofing() = delete;
};

#endif // WEBGLSPOOFING_H
