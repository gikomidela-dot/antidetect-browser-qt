#ifndef TLSFINGERPRINTMANAGER_H
#define TLSFINGERPRINTMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>

// TLS Cipher Suites
enum class TLSCipherSuite : uint16_t {
    // TLS 1.3
    TLS_AES_128_GCM_SHA256 = 0x1301,
    TLS_AES_256_GCM_SHA384 = 0x1302,
    TLS_CHACHA20_POLY1305_SHA256 = 0x1303,
    
    // TLS 1.2
    TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 = 0xC02B,
    TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 = 0xC02F,
    TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 = 0xC02C,
    TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 = 0xC030,
    TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 = 0xCCA9,
    TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 = 0xCCA8,
    TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA = 0xC013,
    TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA = 0xC014,
    TLS_RSA_WITH_AES_128_GCM_SHA256 = 0x009C,
    TLS_RSA_WITH_AES_256_GCM_SHA384 = 0x009D,
    TLS_RSA_WITH_AES_128_CBC_SHA = 0x002F,
    TLS_RSA_WITH_AES_256_CBC_SHA = 0x0035
};

// TLS Extensions
enum class TLSExtension : uint16_t {
    SERVER_NAME = 0,
    STATUS_REQUEST = 5,
    SUPPORTED_GROUPS = 10,
    EC_POINT_FORMATS = 11,
    SIGNATURE_ALGORITHMS = 13,
    APPLICATION_LAYER_PROTOCOL_NEGOTIATION = 16,
    SIGNED_CERTIFICATE_TIMESTAMP = 18,
    ENCRYPT_THEN_MAC = 22,
    EXTENDED_MASTER_SECRET = 23,
    SESSION_TICKET = 35,
    SUPPORTED_VERSIONS = 43,
    PSK_KEY_EXCHANGE_MODES = 45,
    KEY_SHARE = 51,
    RENEGOTIATION_INFO = 65281,
    GREASE = 0x0A0A  // GREASE values
};

// Supported Groups (Elliptic Curves)
enum class TLSSupportedGroup : uint16_t {
    SECP256R1 = 23,
    SECP384R1 = 24,
    SECP521R1 = 25,
    X25519 = 29,
    X448 = 30,
    FFDHE2048 = 256,
    FFDHE3072 = 257,
    FFDHE4096 = 258,
    FFDHE6144 = 259,
    FFDHE8192 = 260
};

// TLS Fingerprint Profile
struct TLSFingerprintProfile {
    QString name;
    QString description;
    QVector<uint16_t> tlsVersions;        // TLS versions (0x0303 = TLS 1.2, 0x0304 = TLS 1.3)
    QVector<uint16_t> cipherSuites;       // Cipher suites
    QVector<uint16_t> extensions;         // Extensions
    QVector<uint16_t> supportedGroups;    // Elliptic curves
    QVector<uint16_t> signatureAlgorithms; // Signature algorithms
    QVector<QString> alpnProtocols;       // ALPN protocols (h2, http/1.1)
    bool useGREASE;                       // Use GREASE values
    
    QString toJA3() const;
    QString toJA4() const;
};

class TLSFingerprintManager : public QObject
{
    Q_OBJECT
    
public:
    explicit TLSFingerprintManager(QObject* parent = nullptr);
    
    // Get predefined profiles
    TLSFingerprintProfile getProfile(const QString& name) const;
    QStringList getProfileNames() const;
    
    // Apply TLS fingerprint (generates environment variables and config)
    bool applyFingerprint(const TLSFingerprintProfile& profile);
    
    // Generate JA3/JA4 fingerprints
    QString generateJA3(const TLSFingerprintProfile& profile) const;
    QString generateJA4(const TLSFingerprintProfile& profile) const;
    
    // Create custom profile
    TLSFingerprintProfile createCustomProfile(
        const QString& name,
        const QVector<uint16_t>& cipherSuites,
        const QVector<uint16_t>& extensions,
        const QVector<uint16_t>& supportedGroups
    );
    
signals:
    void fingerprintApplied(const QString& ja3, const QString& ja4);
    void fingerprintError(const QString& error);
    
private:
    void initializeProfiles();
    QString vectorToString(const QVector<uint16_t>& vec, const QString& separator = "-") const;
    
    QMap<QString, TLSFingerprintProfile> m_profiles;
};

#endif // TLSFINGERPRINTMANAGER_H
