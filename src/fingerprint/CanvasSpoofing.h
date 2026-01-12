#ifndef CANVASSPOOFING_H
#define CANVASSPOOFING_H

#include <QString>

class CanvasSpoofing
{
public:
    static QString generateScript(bool enabled);
    
private:
    CanvasSpoofing() = delete;
};

#endif // CANVASSPOOFING_H
