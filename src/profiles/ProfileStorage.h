#ifndef PROFILESTORAGE_H
#define PROFILESTORAGE_H

#include <QString>
#include <QList>
#include "Profile.h"

class QSqlDatabase;

class ProfileStorage
{
public:
    explicit ProfileStorage(const QString& dataPath);
    ~ProfileStorage();
    
    bool save(const Profile& profile);
    Profile load(const QString& id) const;
    QList<Profile> loadAll() const;
    bool remove(const QString& id);
    bool exists(const QString& id) const;
    
private:
    bool initializeDatabase();
    bool createTables();
    
    QString m_dataPath;
    QString m_dbPath;
    QSqlDatabase* m_db;
};

#endif // PROFILESTORAGE_H
