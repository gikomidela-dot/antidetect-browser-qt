#ifndef BROWSERWINDOWMANAGER_H
#define BROWSERWINDOWMANAGER_H

#include <QObject>
#include <QMap>
#include "profiles/Profile.h"

class BrowserWindow;

class BrowserWindowManager : public QObject
{
    Q_OBJECT
    
public:
    explicit BrowserWindowManager(QObject* parent = nullptr);
    ~BrowserWindowManager();
    
    // Launch browser with profile
    BrowserWindow* launchProfile(const Profile& profile);
    
    // Get active window for profile
    BrowserWindow* getWindow(const QString& profileId) const;
    
    // Check if profile is running
    bool isProfileRunning(const QString& profileId) const;
    
    // Close browser window
    void closeWindow(const QString& profileId);
    
    // Get all active windows
    QList<BrowserWindow*> getActiveWindows() const;
    
signals:
    void windowOpened(const QString& profileId);
    void windowClosed(const QString& profileId);
    
private slots:
    void onWindowClosed(const QString& profileId);
    
private:
    QMap<QString, BrowserWindow*> m_windows;
};

#endif // BROWSERWINDOWMANAGER_H
