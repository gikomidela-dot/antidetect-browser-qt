#ifndef PROFILE_H
#define PROFILE_H

#include <QString>
#include <QJsonObject>
#include <QDateTime>
#include <QMap>

struct FingerprintConfig {
    QString userAgent;
    QString platform;
    QString vendor;
    QString renderer;
    int hardwareConcurrency;
    int deviceMemory;
    int screenWidth;
    int screenHeight;
    int colorDepth;
    QString timezone;
    QString locale;
    QString webglVendor;
    QString webglRenderer;
    bool canvasNoise;
    bool audioNoise;
    bool webrtcProtection;
    QString tlsProfile;  // TLS fingerprint profile name
    
    QJsonObject toJson() const;
    static FingerprintConfig fromJson(const QJsonObject& json);
};

struct ProxyConfig {
    enum Type {
        None,
        HTTP,
        HTTPS,
        SOCKS4,
        SOCKS5
    };
    
    Type type;
    QString host;
    int port;
    QString username;
    QString password;
    
    QJsonObject toJson() const;
    static ProxyConfig fromJson(const QJsonObject& json);
    QString toString() const;
};

class Profile
{
public:
    Profile();
    Profile(const QString& id, const QString& name);
    
    QString id() const { return m_id; }
    void setId(const QString& id) { m_id = id; }
    
    QString name() const { return m_name; }
    void setName(const QString& name) { m_name = name; }
    
    QString notes() const { return m_notes; }
    void setNotes(const QString& notes) { m_notes = notes; }
    
    QStringList tags() const { return m_tags; }
    void setTags(const QStringList& tags) { m_tags = tags; }
    
    FingerprintConfig fingerprint() const { return m_fingerprint; }
    void setFingerprint(const FingerprintConfig& config) { m_fingerprint = config; }
    
    ProxyConfig proxy() const { return m_proxy; }
    void setProxy(const ProxyConfig& config) { m_proxy = config; }
    
    QString vpnConfig() const { return m_vpnConfig; }
    void setVpnConfig(const QString& config) { m_vpnConfig = config; }
    
    QDateTime createdAt() const { return m_createdAt; }
    QDateTime updatedAt() const { return m_updatedAt; }
    void setUpdatedAt(const QDateTime& dt) { m_updatedAt = dt; }
    
    QJsonObject toJson() const;
    static Profile fromJson(const QJsonObject& json);
    
    bool isValid() const;
    
private:
    QString m_id;
    QString m_name;
    QString m_notes;
    QStringList m_tags;
    FingerprintConfig m_fingerprint;
    ProxyConfig m_proxy;
    QString m_vpnConfig;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

#endif // PROFILE_H
