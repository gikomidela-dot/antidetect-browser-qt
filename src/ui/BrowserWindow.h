#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QLineEdit>
#include <QProgressBar>
#include "profiles/Profile.h"

class BrowserWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit BrowserWindow(const Profile& profile, QWidget *parent = nullptr);
    ~BrowserWindow();
    
    void navigate(const QString& url);
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
    
private:
    void setupUi();
    void setupWebEngine();
    void applyFingerprint();
    void applyProxy();
    void createToolbar();
    
    Profile m_profile;
    QWebEngineProfile* m_webProfile;
    QWebEngineView* m_webView;
    QLineEdit* m_addressBar;
    QProgressBar* m_progressBar;
};

#endif // BROWSERWINDOW_H
