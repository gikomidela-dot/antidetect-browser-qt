#ifndef UNDETECTABLESTEALTH_H
#define UNDETECTABLESTEALTH_H

#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QString>

class UndetectableStealth
{
public:
    static void applyStealthMode(QWebEnginePage* page);
    static void applyStealthProfile(QWebEngineProfile* profile);
    static QString getStealthScript();
    
private:
    static QString getChromeDriverPatch();
    static QString getNavigatorPatch();
    static QString getWebGLPatch();
    static QString getCanvasPatch();
    static QString getPermissionsPatch();
    static QString getPluginsPatch();
    static QString getWebRTCPatch();
};

#endif // UNDETECTABLESTEALTH_H
