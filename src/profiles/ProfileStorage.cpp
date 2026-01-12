#include "ProfileStorage.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QDir>
#include <QDebug>

ProfileStorage::ProfileStorage(const QString& dataPath)
    : m_dataPath(dataPath)
    , m_dbPath(dataPath + "/profiles.db")
    , m_db(nullptr)
{
    QDir dir;
    if (!dir.exists(m_dataPath)) {
        dir.mkpath(m_dataPath);
    }
    
    initializeDatabase();
}

ProfileStorage::~ProfileStorage()
{
    if (m_db) {
        m_db->close();
        delete m_db;
    }
}

bool ProfileStorage::initializeDatabase()
{
    m_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "profiles"));
    m_db->setDatabaseName(m_dbPath);
    
    if (!m_db->open()) {
        qCritical() << "Cannot open database:" << m_db->lastError().text();
        return false;
    }
    
    return createTables();
}

bool ProfileStorage::createTables()
{
    QSqlQuery query(*m_db);
    
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS profiles (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            notes TEXT,
            tags TEXT,
            fingerprint TEXT,
            proxy TEXT,
            created_at TEXT,
            updated_at TEXT
        )
    )";
    
    if (!query.exec(sql)) {
        qCritical() << "Cannot create table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool ProfileStorage::save(const Profile& profile)
{
    if (!profile.isValid()) {
        return false;
    }
    
    QSqlQuery query(*m_db);
    
    QString sql = R"(
        INSERT OR REPLACE INTO profiles 
        (id, name, notes, tags, fingerprint, proxy, created_at, updated_at)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )";
    
    query.prepare(sql);
    query.addBindValue(profile.id());
    query.addBindValue(profile.name());
    query.addBindValue(profile.notes());
    query.addBindValue(profile.tags().join(","));
    
    QJsonDocument fpDoc(profile.fingerprint().toJson());
    query.addBindValue(QString::fromUtf8(fpDoc.toJson(QJsonDocument::Compact)));
    
    QJsonDocument proxyDoc(profile.proxy().toJson());
    query.addBindValue(QString::fromUtf8(proxyDoc.toJson(QJsonDocument::Compact)));
    
    query.addBindValue(profile.createdAt().toString(Qt::ISODate));
    query.addBindValue(profile.updatedAt().toString(Qt::ISODate));
    
    if (!query.exec()) {
        qWarning() << "Cannot save profile:" << query.lastError().text();
        return false;
    }
    
    return true;
}

Profile ProfileStorage::load(const QString& id) const
{
    QSqlQuery query(*m_db);
    query.prepare("SELECT * FROM profiles WHERE id = ?");
    query.addBindValue(id);
    
    if (!query.exec() || !query.next()) {
        return Profile();
    }
    
    Profile profile;
    profile.setId(query.value("id").toString());
    profile.setName(query.value("name").toString());
    profile.setNotes(query.value("notes").toString());
    
    QString tagsStr = query.value("tags").toString();
    if (!tagsStr.isEmpty()) {
        profile.setTags(tagsStr.split(","));
    }
    
    QString fpJson = query.value("fingerprint").toString();
    QJsonDocument fpDoc = QJsonDocument::fromJson(fpJson.toUtf8());
    profile.setFingerprint(FingerprintConfig::fromJson(fpDoc.object()));
    
    QString proxyJson = query.value("proxy").toString();
    QJsonDocument proxyDoc = QJsonDocument::fromJson(proxyJson.toUtf8());
    profile.setProxy(ProxyConfig::fromJson(proxyDoc.object()));
    
    profile.setUpdatedAt(QDateTime::fromString(query.value("updated_at").toString(), Qt::ISODate));
    
    return profile;
}

QList<Profile> ProfileStorage::loadAll() const
{
    QList<Profile> profiles;
    
    QSqlQuery query(*m_db);
    if (!query.exec("SELECT id FROM profiles")) {
        qWarning() << "Cannot load profiles:" << query.lastError().text();
        return profiles;
    }
    
    while (query.next()) {
        QString id = query.value(0).toString();
        Profile profile = load(id);
        if (profile.isValid()) {
            profiles.append(profile);
        }
    }
    
    return profiles;
}

bool ProfileStorage::remove(const QString& id)
{
    QSqlQuery query(*m_db);
    query.prepare("DELETE FROM profiles WHERE id = ?");
    query.addBindValue(id);
    
    if (!query.exec()) {
        qWarning() << "Cannot delete profile:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}

bool ProfileStorage::exists(const QString& id) const
{
    QSqlQuery query(*m_db);
    query.prepare("SELECT COUNT(*) FROM profiles WHERE id = ?");
    query.addBindValue(id);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    
    return false;
}
