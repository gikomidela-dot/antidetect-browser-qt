#ifndef VPNMANAGER_H
#define VPNMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>

struct VPNConfig {
    QString name;
    QString type; // openvpn, shadowsocks, wireguard
    QString configPath;
    QMap<QString, QString> parameters;
};

class VPNManager : public QObject
{
    Q_OBJECT
    
public:
    explicit VPNManager(QObject* parent = nullptr);
    
    bool importConfig(const QString& filePath);
    bool importFromUrl(const QString& url);
    VPNConfig parseConfig(const QString& filePath, const QString& type);
    VPNConfig parseFromUrl(const QString& url);
    
    QList<VPNConfig> getConfigs() const { return m_configs.values(); }
    VPNConfig getConfig(const QString& name) const { return m_configs.value(name); }
    bool removeConfig(const QString& name);
    
signals:
    void configImported(const QString& name);
    void importError(const QString& error);
    
private:
    QMap<QString, VPNConfig> m_configs;
};

#endif // VPNMANAGER_H
