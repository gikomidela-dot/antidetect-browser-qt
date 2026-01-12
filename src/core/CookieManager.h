#ifndef COOKIEMANAGER_H
#define COOKIEMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QNetworkCookie>
#include <QWebEngineCookieStore>

class CookieManager : public QObject
{
    Q_OBJECT
    
public:
    explicit CookieManager(QObject* parent = nullptr);
    
    // Export cookies from profile
    bool exportCookies(const QString& profileId, const QString& filePath, const QString& format = "json");
    
    // Export cookies from multiple profiles
    bool exportCookiesMultiple(const QStringList& profileIds, const QString& filePath, const QString& format = "json");
    
    // Import cookies to profile
    bool importCookies(const QString& profileId, const QString& filePath, const QString& format = "json");
    
    // Get cookies from profile storage
    QList<QNetworkCookie> getCookiesFromProfile(const QString& profileId);
    
    // Save cookies to profile storage
    bool saveCookiesToProfile(const QString& profileId, const QList<QNetworkCookie>& cookies);
    
signals:
    void cookiesExported(const QString& filePath);
    void cookiesImported(int count);
    void exportError(const QString& error);
    
private:
    QString cookiesToJson(const QList<QNetworkCookie>& cookies);
    QString cookiesToNetscape(const QList<QNetworkCookie>& cookies);
    QList<QNetworkCookie> cookiesFromJson(const QString& json);
    QList<QNetworkCookie> cookiesFromNetscape(const QString& content);
    
    QString getCookieStoragePath(const QString& profileId);
};

#endif // COOKIEMANAGER_H
