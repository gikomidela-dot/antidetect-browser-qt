#ifndef FINGERPRINTMANAGER_H
#define FINGERPRINTMANAGER_H

#include <QObject>
#include <QString>
#include <QWebEngineProfile>
#include "profiles/Profile.h"

class FingerprintManager : public QObject
{
    Q_OBJECT
    
public:
    explicit FingerprintManager(QObject* parent = nullptr);
    
    // Apply fingerprint to web engine profile
    void applyFingerprint(QWebEngineProfile* profile, const FingerprintConfig& config);
    
    // Generate JavaScript injection code
    QString generateInjectionScript(const FingerprintConfig& config);
    
private:
    QString generateTimezoneScript(const FingerprintConfig& config);
    QString generateCanvasScript(const FingerprintConfig& config);
    QString generateWebGLScript(const FingerprintConfig& config);
    QString generateAudioScript(const FingerprintConfig& config);
    QString generateNavigatorScript(const FingerprintConfig& config);
    QString generateScreenScript(const FingerprintConfig& config);
    QString generateWebRTCScript(const FingerprintConfig& config);
};

#endif // FINGERPRINTMANAGER_H
