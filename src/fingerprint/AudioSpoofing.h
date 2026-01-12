#ifndef AUDIOSPOOFING_H
#define AUDIOSPOOFING_H

#include <QString>

class AudioSpoofing
{
public:
    static QString generateScript(bool enabled);
    
private:
    AudioSpoofing() = delete;
};

#endif // AUDIOSPOOFING_H
