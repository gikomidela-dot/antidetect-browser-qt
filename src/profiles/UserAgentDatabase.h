#ifndef USERAGENTDATABASE_H
#define USERAGENTDATABASE_H

#include <QString>
#include <QMap>
#include <QList>
#include "Profile.h"

struct UserAgentProfile {
    QString name;
    QString category; // Windows, macOS, Linux, Android, iOS
    QString userAgent;
    QString platform;
    QString vendor;
    QString webglVendor;
    QString webglRenderer;
    int screenWidth;
    int screenHeight;
    int hardwareConcurrency;
    int deviceMemory;
    QString timezone;
    QString locale;
    
    FingerprintConfig toFingerprintConfig() const;
};

class UserAgentDatabase
{
public:
    UserAgentDatabase();
    
    QStringList getCategories() const;
    QList<UserAgentProfile> getProfilesByCategory(const QString& category) const;
    UserAgentProfile getProfile(const QString& name) const;
    
private:
    void initializeDatabase();
    
    QMap<QString, QList<UserAgentProfile>> m_database;
};

#endif // USERAGENTDATABASE_H
