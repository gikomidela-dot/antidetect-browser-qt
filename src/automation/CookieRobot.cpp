#include "CookieRobot.h"
#include <QWebEngineView>
#include <QTimer>

CookieRobot::CookieRobot(QObject* parent)
    : QObject(parent)
    , m_profile(nullptr)
    , m_currentIndex(0)
{
}

void CookieRobot::warmupProfile(QWebEngineProfile* profile, const QStringList& sites)
{
    m_profile = profile;
    m_sites = sites;
    m_currentIndex = 0;
    
    if (m_sites.isEmpty()) {
        emit warmupCompleted();
        return;
    }
    
    emit warmupStarted(m_sites.size());
    visitNextSite();
}

void CookieRobot::visitNextSite()
{
    if (m_currentIndex >= m_sites.size()) {
        emit warmupCompleted();
        return;
    }
    
    QString url = m_sites[m_currentIndex];
    
    QWebEngineView* view = new QWebEngineView();
    view->setPage(new QWebEnginePage(m_profile, view));
    
    connect(view, &QWebEngineView::loadFinished, this, [this, view, url](bool success) {
        if (success) {
            emit siteVisited(url);
        }
        
        // Wait a bit before visiting next site
        QTimer::singleShot(2000, this, [this, view]() {
            view->deleteLater();
            m_currentIndex++;
            visitNextSite();
        });
    });
    
    view->load(QUrl(url));
}
