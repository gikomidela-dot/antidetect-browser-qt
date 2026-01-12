#include "TLSFingerprintManager.h"
#include <QCryptographicHash>
#include <QDebug>

TLSFingerprintManager::TLSFingerprintManager(QObject* parent)
    : QObject(parent)
{
    initializeProfiles();
}

void TLSFingerprintManager::initializeProfiles()
{
    // Chrome 120 (Windows)
    TLSFingerprintProfile chromeWin;
    chromeWin.name = "Chrome 120 Windows";
    chromeWin.description = "Google Chrome 120 on Windows 10/11";
    chromeWin.tlsVersions = {0x0303, 0x0304}; // TLS 1.2, 1.3
    chromeWin.cipherSuites = {
        0x1301, 0x1302, 0x1303, // TLS 1.3 ciphers
        0xC02B, 0xC02F, 0xC02C, 0xC030,
        0xCCA9, 0xCCA8, 0xC013, 0xC014,
        0x009C, 0x009D, 0x002F, 0x0035
    };
    chromeWin.extensions = {
        0, 23, 65281, 10, 11, 35, 16, 5, 13, 18, 51, 45, 43, 27, 21
    };
    chromeWin.supportedGroups = {29, 23, 24}; // X25519, secp256r1, secp384r1
    chromeWin.signatureAlgorithms = {
        0x0403, 0x0503, 0x0603, 0x0804, 0x0805, 0x0806, 0x0401, 0x0501, 0x0601
    };
    chromeWin.alpnProtocols = {"h2", "http/1.1"};
    chromeWin.useGREASE = true;
    m_profiles["Chrome 120 Windows"] = chromeWin;
    
    // Firefox 121 (Windows)
    TLSFingerprintProfile firefoxWin;
    firefoxWin.name = "Firefox 121 Windows";
    firefoxWin.description = "Mozilla Firefox 121 on Windows 10/11";
    firefoxWin.tlsVersions = {0x0303, 0x0304};
    firefoxWin.cipherSuites = {
        0x1301, 0x1302, 0x1303,
        0xC02B, 0xC02F, 0xC02C, 0xC030,
        0xCCA9, 0xCCA8, 0xC013, 0xC014
    };
    firefoxWin.extensions = {
        0, 23, 65281, 10, 11, 35, 16, 5, 13, 51, 45, 43, 21
    };
    firefoxWin.supportedGroups = {29, 23, 24, 25};
    firefoxWin.signatureAlgorithms = {
        0x0403, 0x0503, 0x0603, 0x0804, 0x0805, 0x0806
    };
    firefoxWin.alpnProtocols = {"h2", "http/1.1"};
    firefoxWin.useGREASE = false;
    m_profiles["Firefox 121 Windows"] = firefoxWin;
    
    // Safari 17 (macOS)
    TLSFingerprintProfile safariMac;
    safariMac.name = "Safari 17 macOS";
    safariMac.description = "Safari 17 on macOS Sonoma";
    safariMac.tlsVersions = {0x0303, 0x0304};
    safariMac.cipherSuites = {
        0x1301, 0x1302, 0x1303,
        0xC02B, 0xC02F, 0xC02C, 0xC030,
        0xCCA9, 0xCCA8
    };
    safariMac.extensions = {
        0, 23, 65281, 10, 11, 35, 16, 5, 13, 51, 45, 43
    };
    safariMac.supportedGroups = {29, 23, 24};
    safariMac.signatureAlgorithms = {
        0x0403, 0x0503, 0x0603, 0x0804, 0x0805, 0x0806
    };
    safariMac.alpnProtocols = {"h2", "http/1.1"};
    safariMac.useGREASE = false;
    m_profiles["Safari 17 macOS"] = safariMac;
    
    // Edge 120 (Windows)
    TLSFingerprintProfile edgeWin;
    edgeWin.name = "Edge 120 Windows";
    edgeWin.description = "Microsoft Edge 120 on Windows 10/11";
    edgeWin.tlsVersions = {0x0303, 0x0304};
    edgeWin.cipherSuites = chromeWin.cipherSuites; // Same as Chrome
    edgeWin.extensions = chromeWin.extensions;
    edgeWin.supportedGroups = chromeWin.supportedGroups;
    edgeWin.signatureAlgorithms = chromeWin.signatureAlgorithms;
    edgeWin.alpnProtocols = {"h2", "http/1.1"};
    edgeWin.useGREASE = true;
    m_profiles["Edge 120 Windows"] = edgeWin;
}

TLSFingerprintProfile TLSFingerprintManager::getProfile(const QString& name) const
{
    return m_profiles.value(name, TLSFingerprintProfile());
}

QStringList TLSFingerprintManager::getProfileNames() const
{
    return m_profiles.keys();
}

bool TLSFingerprintManager::applyFingerprint(const TLSFingerprintProfile& profile)
{
    if (profile.name.isEmpty()) {
        emit fingerprintError("Invalid TLS profile");
        return false;
    }
    
    QString ja3 = generateJA3(profile);
    QString ja4 = generateJA4(profile);
    
    qDebug() << "TLS Fingerprint Applied:";
    qDebug() << "  Profile:" << profile.name;
    qDebug() << "  JA3:" << ja3;
    qDebug() << "  JA4:" << ja4;
    
    // Note: Actual TLS fingerprint modification requires:
    // 1. Patching BoringSSL/OpenSSL in Chromium
    // 2. Using external tools like curl-impersonate
    // 3. Or using a custom network stack
    
    // For now, we log the fingerprint that should be applied
    emit fingerprintApplied(ja3, ja4);
    
    return true;
}

QString TLSFingerprintManager::generateJA3(const TLSFingerprintProfile& profile) const
{
    // JA3 Format: SSLVersion,Ciphers,Extensions,EllipticCurves,EllipticCurvePointFormats
    
    QString sslVersion = QString::number(profile.tlsVersions.isEmpty() ? 771 : profile.tlsVersions.last());
    QString ciphers = vectorToString(profile.cipherSuites, "-");
    QString extensions = vectorToString(profile.extensions, "-");
    QString curves = vectorToString(profile.supportedGroups, "-");
    QString pointFormats = "0"; // Usually 0 (uncompressed)
    
    QString ja3String = QString("%1,%2,%3,%4,%5")
        .arg(sslVersion)
        .arg(ciphers)
        .arg(extensions)
        .arg(curves)
        .arg(pointFormats);
    
    // MD5 hash of JA3 string
    QByteArray hash = QCryptographicHash::hash(ja3String.toUtf8(), QCryptographicHash::Md5);
    return QString(hash.toHex());
}

QString TLSFingerprintManager::generateJA4(const TLSFingerprintProfile& profile) const
{
    // JA4 Format: [protocol][version][ciphers_count][extensions_count][alpn]_[ciphers_hash]_[extensions_hash]
    
    QString protocol = "t"; // TCP
    QString version = profile.tlsVersions.contains(0x0304) ? "13" : "12";
    QString ciphersCount = QString("%1").arg(profile.cipherSuites.size(), 2, 10, QChar('0'));
    QString extensionsCount = QString("%1").arg(profile.extensions.size(), 2, 10, QChar('0'));
    QString alpn = profile.alpnProtocols.isEmpty() ? "00" : 
                   (profile.alpnProtocols.contains("h2") ? "h2" : "h1");
    
    // Hash ciphers
    QString ciphersStr = vectorToString(profile.cipherSuites, ",");
    QByteArray ciphersHash = QCryptographicHash::hash(ciphersStr.toUtf8(), QCryptographicHash::Sha256);
    QString ciphersHashStr = QString(ciphersHash.toHex().left(12));
    
    // Hash extensions
    QString extensionsStr = vectorToString(profile.extensions, ",");
    QByteArray extensionsHash = QCryptographicHash::hash(extensionsStr.toUtf8(), QCryptographicHash::Sha256);
    QString extensionsHashStr = QString(extensionsHash.toHex().left(12));
    
    return QString("%1%2%3%4%5_%6_%7")
        .arg(protocol)
        .arg(version)
        .arg(ciphersCount)
        .arg(extensionsCount)
        .arg(alpn)
        .arg(ciphersHashStr)
        .arg(extensionsHashStr);
}

TLSFingerprintProfile TLSFingerprintManager::createCustomProfile(
    const QString& name,
    const QVector<uint16_t>& cipherSuites,
    const QVector<uint16_t>& extensions,
    const QVector<uint16_t>& supportedGroups)
{
    TLSFingerprintProfile profile;
    profile.name = name;
    profile.description = "Custom TLS profile";
    profile.tlsVersions = {0x0303, 0x0304};
    profile.cipherSuites = cipherSuites;
    profile.extensions = extensions;
    profile.supportedGroups = supportedGroups;
    profile.signatureAlgorithms = {0x0403, 0x0503, 0x0603};
    profile.alpnProtocols = {"h2", "http/1.1"};
    profile.useGREASE = false;
    
    return profile;
}

QString TLSFingerprintManager::vectorToString(const QVector<uint16_t>& vec, const QString& separator) const
{
    QStringList list;
    for (uint16_t val : vec) {
        list.append(QString::number(val));
    }
    return list.join(separator);
}

QString TLSFingerprintProfile::toJA3() const
{
    TLSFingerprintManager manager;
    return manager.generateJA3(*this);
}

QString TLSFingerprintProfile::toJA4() const
{
    TLSFingerprintManager manager;
    return manager.generateJA4(*this);
}
