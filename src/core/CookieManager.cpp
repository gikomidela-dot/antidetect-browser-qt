#include "CookieManager.h"
#include "Application.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QDebug>

CookieManager::CookieManager(QObject* parent)
    : QObject(parent)
{
}

bool CookieManager::exportCookies(const QString& profileId, const QString& filePath, const QString& format)
{
    qDebug() << "CookieManager::exportCookies - ProfileId:" << profileId << "FilePath:" << filePath << "Format:" << format;
    
    QList<QNetworkCookie> cookies = getCookiesFromProfile(profileId);
    
    qDebug() << "CookieManager::exportCookies - Found" << cookies.size() << "cookies";
    
    if (cookies.isEmpty()) {
        QString errorMsg = QString("No cookies found for profile: %1\n\nCookie storage path: %2")
            .arg(profileId)
            .arg(getCookieStoragePath(profileId));
        qWarning() << errorMsg;
        emit exportError(errorMsg);
        return false;
    }
    
    // Ensure directory exists
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        qDebug() << "Creating directory:" << dir.absolutePath();
        dir.mkpath(".");
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString errorMsg = QString("Cannot create file: %1\nError: %2")
            .arg(filePath)
            .arg(file.errorString());
        qWarning() << errorMsg;
        emit exportError(errorMsg);
        return false;
    }
    
    QString content;
    if (format.toLower() == "json") {
        content = cookiesToJson(cookies);
    } else if (format.toLower() == "netscape" || format.toLower() == "txt") {
        content = cookiesToNetscape(cookies);
    } else {
        emit exportError("Unsupported format: " + format);
        return false;
    }
    
    file.write(content.toUtf8());
    file.close();
    
    emit cookiesExported(filePath);
    return true;
}

bool CookieManager::exportCookiesMultiple(const QStringList& profileIds, const QString& filePath, const QString& format)
{
    QList<QNetworkCookie> allCookies;
    
    for (const QString& profileId : profileIds) {
        QList<QNetworkCookie> cookies = getCookiesFromProfile(profileId);
        allCookies.append(cookies);
    }
    
    if (allCookies.isEmpty()) {
        emit exportError("No cookies found in selected profiles");
        return false;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit exportError("Cannot create file: " + filePath);
        return false;
    }
    
    QString content;
    if (format.toLower() == "json") {
        content = cookiesToJson(allCookies);
    } else if (format.toLower() == "netscape" || format.toLower() == "txt") {
        content = cookiesToNetscape(allCookies);
    } else {
        emit exportError("Unsupported format: " + format);
        return false;
    }
    
    file.write(content.toUtf8());
    file.close();
    
    emit cookiesExported(filePath);
    return true;
}

bool CookieManager::importCookies(const QString& profileId, const QString& filePath, const QString& format)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit exportError("Cannot open file: " + filePath);
        return false;
    }
    
    QString content = QString::fromUtf8(file.readAll());
    file.close();
    
    QList<QNetworkCookie> cookies;
    if (format.toLower() == "json") {
        cookies = cookiesFromJson(content);
    } else if (format.toLower() == "netscape" || format.toLower() == "txt") {
        cookies = cookiesFromNetscape(content);
    } else {
        emit exportError("Unsupported format: " + format);
        return false;
    }
    
    if (cookies.isEmpty()) {
        emit exportError("No valid cookies found in file");
        return false;
    }
    
    bool success = saveCookiesToProfile(profileId, cookies);
    if (success) {
        emit cookiesImported(cookies.size());
    }
    
    return success;
}

QList<QNetworkCookie> CookieManager::getCookiesFromProfile(const QString& profileId)
{
    QList<QNetworkCookie> cookies;
    
    // Read cookies from profile's cookie storage
    QString cookiePath = getCookieStoragePath(profileId);
    QFile file(cookiePath);
    
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QString content = QString::fromUtf8(file.readAll());
        file.close();
        cookies = cookiesFromJson(content);
    }
    
    return cookies;
}

bool CookieManager::saveCookiesToProfile(const QString& profileId, const QList<QNetworkCookie>& cookies)
{
    QString cookiePath = getCookieStoragePath(profileId);
    
    // Create directory if needed
    QFileInfo fileInfo(cookiePath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QFile file(cookiePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QString content = cookiesToJson(cookies);
    file.write(content.toUtf8());
    file.close();
    
    return true;
}

QString CookieManager::cookiesToJson(const QList<QNetworkCookie>& cookies)
{
    QJsonArray array;
    
    for (const QNetworkCookie& cookie : cookies) {
        QJsonObject obj;
        obj["name"] = QString::fromUtf8(cookie.name());
        obj["value"] = QString::fromUtf8(cookie.value());
        obj["domain"] = cookie.domain();
        obj["path"] = cookie.path();
        obj["expires"] = cookie.expirationDate().toString(Qt::ISODate);
        obj["secure"] = cookie.isSecure();
        obj["httpOnly"] = cookie.isHttpOnly();
        obj["sameSite"] = cookie.sameSitePolicy() == QNetworkCookie::SameSite::Strict ? "Strict" :
                          cookie.sameSitePolicy() == QNetworkCookie::SameSite::Lax ? "Lax" : "None";
        
        array.append(obj);
    }
    
    QJsonDocument doc(array);
    return doc.toJson(QJsonDocument::Indented);
}

QString CookieManager::cookiesToNetscape(const QList<QNetworkCookie>& cookies)
{
    QString result;
    result += "# Netscape HTTP Cookie File\n";
    result += "# This is a generated file! Do not edit.\n\n";
    
    for (const QNetworkCookie& cookie : cookies) {
        QString domain = cookie.domain();
        QString flag = domain.startsWith(".") ? "TRUE" : "FALSE";
        QString path = cookie.path();
        QString secure = cookie.isSecure() ? "TRUE" : "FALSE";
        QString expiration = QString::number(cookie.expirationDate().toSecsSinceEpoch());
        QString name = QString::fromUtf8(cookie.name());
        QString value = QString::fromUtf8(cookie.value());
        
        result += QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\n")
            .arg(domain)
            .arg(flag)
            .arg(path)
            .arg(secure)
            .arg(expiration)
            .arg(name)
            .arg(value);
    }
    
    return result;
}

QList<QNetworkCookie> CookieManager::cookiesFromJson(const QString& json)
{
    QList<QNetworkCookie> cookies;
    
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (!doc.isArray()) {
        return cookies;
    }
    
    QJsonArray array = doc.array();
    for (const QJsonValue& value : array) {
        if (!value.isObject()) continue;
        
        QJsonObject obj = value.toObject();
        
        QNetworkCookie cookie;
        cookie.setName(obj["name"].toString().toUtf8());
        cookie.setValue(obj["value"].toString().toUtf8());
        cookie.setDomain(obj["domain"].toString());
        cookie.setPath(obj["path"].toString());
        
        QString expires = obj["expires"].toString();
        if (!expires.isEmpty()) {
            cookie.setExpirationDate(QDateTime::fromString(expires, Qt::ISODate));
        }
        
        cookie.setSecure(obj["secure"].toBool());
        cookie.setHttpOnly(obj["httpOnly"].toBool());
        
        QString sameSite = obj["sameSite"].toString();
        if (sameSite == "Strict") {
            cookie.setSameSitePolicy(QNetworkCookie::SameSite::Strict);
        } else if (sameSite == "Lax") {
            cookie.setSameSitePolicy(QNetworkCookie::SameSite::Lax);
        } else {
            cookie.setSameSitePolicy(QNetworkCookie::SameSite::None);
        }
        
        cookies.append(cookie);
    }
    
    return cookies;
}

QList<QNetworkCookie> CookieManager::cookiesFromNetscape(const QString& content)
{
    QList<QNetworkCookie> cookies;
    
    QStringList lines = content.split('\n');
    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        
        // Skip comments and empty lines
        if (trimmed.isEmpty() || trimmed.startsWith('#')) {
            continue;
        }
        
        QStringList parts = trimmed.split('\t');
        if (parts.size() < 7) {
            continue;
        }
        
        QNetworkCookie cookie;
        cookie.setDomain(parts[0]);
        cookie.setPath(parts[2]);
        cookie.setSecure(parts[3] == "TRUE");
        
        qint64 expiration = parts[4].toLongLong();
        if (expiration > 0) {
            cookie.setExpirationDate(QDateTime::fromSecsSinceEpoch(expiration));
        }
        
        cookie.setName(parts[5].toUtf8());
        cookie.setValue(parts[6].toUtf8());
        
        cookies.append(cookie);
    }
    
    return cookies;
}

QString CookieManager::getCookieStoragePath(const QString& profileId)
{
    QString dataDir = Application::instance().dataDirectory();
    QString path = QDir(dataDir).filePath("cookies");
    QDir cookieDir(path);
    if (!cookieDir.exists()) {
        cookieDir.mkpath(".");
    }
    return QDir(path).filePath(profileId + ".json");
}
