#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QLineEdit>
#include <QProgressBar>
#include <QTabWidget>
#include <QToolBar>
#include "profiles/Profile.h"

class BrowserWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit BrowserWindow(const Profile& profile, QWidget *parent = nullptr);
    ~BrowserWindow();
    
    void navigate(const QString& url);
    void newTab(const QString& url = "about:blank");
    Profile profile() const { return m_profile; }
    
signals:
    void windowClosed(const QString& profileId);
    
protected:
    void closeEvent(QCloseEvent* event) override;
    
private slots:
    void onUrlChanged(const QUrl& url);
    void onLoadProgress(int progress);
    void onLoadFinished(bool success);
    void onAddressBarReturn();
    void onBackClicked();
    void onForwardClicked();
    void onReloadClicked();
    void onHomeClicked();
    void onNewTabClicked();
    void onCloseTabClicked(int index);
    void onTabChanged(int index);
    
private:
    void setupUi();
    void setupWebEngine();
    void applyFingerprint();
    void applyProxy();
    void createToolbar();
    QWebEngineView* createWebView();
    QWebEngineView* currentWebView();
    void saveCookies();
    
    Profile m_profile;
    QWebEngineProfile* m_webProfile;
    QTabWidget* m_tabWidget;
    QLineEdit* m_addressBar;
    QProgressBar* m_progressBar;
    QToolBar* m_toolbar;
};

#endif // BROWSERWINDOW_H
