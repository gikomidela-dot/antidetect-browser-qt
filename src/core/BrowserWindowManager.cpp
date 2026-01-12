#include "BrowserWindowManager.h"
#include "ui/BrowserWindow.h"
#include <QDebug>

BrowserWindowManager::BrowserWindowManager(QObject* parent)
    : QObject(parent)
{
}

BrowserWindowManager::~BrowserWindowManager()
{
    // Close all windows
    for (BrowserWindow* window : m_windows.values()) {
        window->close();
        window->deleteLater();
    }
    m_windows.clear();
}

BrowserWindow* BrowserWindowManager::launchProfile(const Profile& profile)
{
    QString profileId = profile.id();
    
    // Check if already running
    if (m_windows.contains(profileId)) {
        BrowserWindow* existingWindow = m_windows[profileId];
        existingWindow->show();
        existingWindow->raise();
        existingWindow->activateWindow();
        qDebug() << "Profile already running, bringing to front:" << profile.name();
        return existingWindow;
    }
    
    // Create new browser window
    BrowserWindow* window = new BrowserWindow(profile);
    
    // Connect signals
    connect(window, &BrowserWindow::windowClosed, 
            this, &BrowserWindowManager::onWindowClosed);
    
    // Store window
    m_windows[profileId] = window;
    
    // Show window
    window->show();
    
    emit windowOpened(profileId);
    qDebug() << "Launched browser for profile:" << profile.name();
    
    return window;
}

BrowserWindow* BrowserWindowManager::getWindow(const QString& profileId) const
{
    return m_windows.value(profileId, nullptr);
}

bool BrowserWindowManager::isProfileRunning(const QString& profileId) const
{
    return m_windows.contains(profileId);
}

void BrowserWindowManager::closeWindow(const QString& profileId)
{
    if (m_windows.contains(profileId)) {
        BrowserWindow* window = m_windows[profileId];
        window->close();
        // Window will be removed in onWindowClosed slot
    }
}

QList<BrowserWindow*> BrowserWindowManager::getActiveWindows() const
{
    return m_windows.values();
}

void BrowserWindowManager::onWindowClosed(const QString& profileId)
{
    if (m_windows.contains(profileId)) {
        BrowserWindow* window = m_windows[profileId];
        m_windows.remove(profileId);
        window->deleteLater();
        
        emit windowClosed(profileId);
        qDebug() << "Browser window closed for profile:" << profileId;
    }
}
