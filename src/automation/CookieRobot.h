#ifndef COOKIEROBOT_H
#define COOKIEROBOT_H

#include <QObject>
#include <QWebEngineProfile>

class CookieRobot : public QObject
{
    Q_OBJECT
    
public:
    explicit CookieRobot(QObject* parent = nullptr);
    
    // Warm up profile by visiting sites
    void warmupProfile(QWebEngineProfile* profile, const QStringList& sites);
    
signals:
    void warmupStarted(int totalSites);
    void siteVisited(const QString& url);
    void warmupCompleted();
    void warmupFailed(const QString& error);
    
private:
    void visitNextSite();
    
    QWebEngineProfile* m_profile;
    QStringList m_sites;
    int m_currentIndex;
};

#endif // COOKIEROBOT_H
