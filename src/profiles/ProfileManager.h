#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <memory>
#include "Profile.h"

class ProfileStorage;

class ProfileManager : public QObject
{
    Q_OBJECT
    
public:
    explicit ProfileManager(const QString& dataPath, QObject* parent = nullptr);
    ~ProfileManager();
    
    // CRUD operations
    bool createProfile(const Profile& profile);
    Profile getProfile(const QString& id) const;
    QList<Profile> getAllProfiles() const;
    bool updateProfile(const Profile& profile);
    bool deleteProfile(const QString& id);
    
    // Bulk operations
    bool deleteProfiles(const QStringList& ids);
    QList<Profile> importProfiles(const QString& filePath, const QString& format);
    bool exportProfiles(const QStringList& ids, const QString& filePath, const QString& format);
    
    // Search and filter
    QList<Profile> searchProfiles(const QString& query) const;
    QList<Profile> filterByTags(const QStringList& tags) const;
    
    // Templates
    Profile createFromTemplate(const QString& templateName);
    QStringList getTemplateNames() const;
    
signals:
    void profileCreated(const QString& id);
    void profileUpdated(const QString& id);
    void profileDeleted(const QString& id);
    void profilesImported(int count);
    void profilesExported(int count);
    
private:
    void initializeTemplates();
    Profile generateRandomFingerprint(const Profile& base);
    
    std::unique_ptr<ProfileStorage> m_storage;
    QMap<QString, Profile> m_templates;
};

#endif // PROFILEMANAGER_H
