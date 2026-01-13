#include "BrowserWindow.h"
#include "core/Application.h"
#include "core/CookieManager.h"
#include "fingerprint/FingerprintManager.h"
#include "proxy/ProxyManager.h"
#include "stealth/UndetectableStealth.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineCookieStore>
#include <QCloseEvent>
#include <QMessageBox>
#include <QNetworkProxy>
#include <QTimer>
#include <QEventLoop>

BrowserWindow::BrowserWindow(const Profile& profile, QWidget *parent)
    : QMainWindow(parent)
    , m_profile(profile)
    , m_webProfile(nullptr)
    , m_tabWidget(nullptr)
    , m_addressBar(nullptr)
    , m_progressBar(nullptr)
    , m_toolbar(nullptr)
{
    // Set timezone environment variable BEFORE creating web engine
    if (!m_profile.fingerprint().timezone.isEmpty()) {
        qputenv("TZ", m_profile.fingerprint().timezone.toUtf8());
        
        // Calculate timezone offset for LD_PRELOAD hook
        QMap<QString, int> timezoneOffsets;
        timezoneOffsets["America/New_York"] = -18000;
        timezoneOffsets["America/Chicago"] = -21600;
        timezoneOffsets["America/Denver"] = -25200;
        timezoneOffsets["America/Los_Angeles"] = -28800;
        timezoneOffsets["America/Anchorage"] = -32400;
        timezoneOffsets["Pacific/Honolulu"] = -36000;
        timezoneOffsets["Europe/London"] = 0;
        timezoneOffsets["Europe/Paris"] = 3600;
        timezoneOffsets["Europe/Berlin"] = 3600;
        timezoneOffsets["Europe/Moscow"] = 10800;
        timezoneOffsets["Asia/Dubai"] = 14400;
        timezoneOffsets["Asia/Shanghai"] = 28800;
        timezoneOffsets["Asia/Tokyo"] = 32400;
        
        int offset = timezoneOffsets.value(m_profile.fingerprint().timezone, 0);
        qputenv("ANTIDETECT_TZ_OFFSET", QString::number(offset).toUtf8());
    }
    
    setupUi();
    setupWebEngine();
    applyFingerprint();
    applyProxy();
    
    setWindowTitle(QString("SECTA Anti Detect - %1").arg(m_profile.name()));
    resize(1280, 800);
    
    // Create first tab
    newTab("https://www.google.com");
}

BrowserWindow::~BrowserWindow()
{
    if (m_webProfile) {
        m_webProfile->deleteLater();
    }
}

void BrowserWindow::setupUi()
{
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create toolbar
    createToolbar();
    
    // Create address bar
    QHBoxLayout* addressLayout = new QHBoxLayout();
    addressLayout->setContentsMargins(5, 5, 5, 5);
    
    m_addressBar = new QLineEdit(this);
    m_addressBar->setPlaceholderText("Enter URL...");
    connect(m_addressBar, &QLineEdit::returnPressed, this, &BrowserWindow::onAddressBarReturn);
    addressLayout->addWidget(m_addressBar);
    
    mainLayout->addLayout(addressLayout);
    
    // Create progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setMaximumHeight(3);
    m_progressBar->setTextVisible(false);
    m_progressBar->setVisible(false);
    mainLayout->addWidget(m_progressBar);
    
    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &BrowserWindow::onTabChanged);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &BrowserWindow::onCloseTabClicked);
    mainLayout->addWidget(m_tabWidget);
    
    setCentralWidget(centralWidget);
}

void BrowserWindow::createToolbar()
{
    m_toolbar = addToolBar("Navigation");
    m_toolbar->setMovable(false);
    
    // Back button
    QAction* backAction = m_toolbar->addAction("←");
    backAction->setToolTip("Back");
    connect(backAction, &QAction::triggered, this, &BrowserWindow::onBackClicked);
    
    // Forward button
    QAction* forwardAction = m_toolbar->addAction("→");
    forwardAction->setToolTip("Forward");
    connect(forwardAction, &QAction::triggered, this, &BrowserWindow::onForwardClicked);
    
    // Reload button
    QAction* reloadAction = m_toolbar->addAction("⟳");
    reloadAction->setToolTip("Reload");
    connect(reloadAction, &QAction::triggered, this, &BrowserWindow::onReloadClicked);
    
    // Home button
    QAction* homeAction = m_toolbar->addAction("⌂");
    homeAction->setToolTip("Home");
    connect(homeAction, &QAction::triggered, this, &BrowserWindow::onHomeClicked);
    
    m_toolbar->addSeparator();
    
    // New tab button
    QAction* newTabAction = m_toolbar->addAction("+ New Tab");
    newTabAction->setToolTip("Open new tab");
    connect(newTabAction, &QAction::triggered, this, &BrowserWindow::onNewTabClicked);
    
    m_toolbar->addSeparator();
    
    // Check IP button
    QAction* checkIPAction = m_toolbar->addAction("Check IP");
    checkIPAction->setToolTip("Check your IP address");
    connect(checkIPAction, &QAction::triggered, this, [this]() {
        navigate("https://api.ipify.org?format=json");
    });
    
    m_toolbar->addSeparator();
    
    // Profile info
    QLabel* profileLabel = new QLabel(QString("Profile: %1").arg(m_profile.name()), this);
    m_toolbar->addWidget(profileLabel);
}

void BrowserWindow::setupWebEngine()
{
    // Create isolated profile for this browser window
    QString profileName = QString("profile_%1").arg(m_profile.id());
    m_webProfile = new QWebEngineProfile(profileName, this);
    
    // Configure profile settings
    m_webProfile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
    m_webProfile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
    
    // Set storage paths
    QString dataDir = Application::instance().dataDirectory();
    m_webProfile->setPersistentStoragePath(dataDir + "/sessions/" + m_profile.id());
    m_webProfile->setCachePath(dataDir + "/cache/" + m_profile.id());
    
    // Setup cookie monitoring - track cookies as they are added
    QWebEngineCookieStore* cookieStore = m_webProfile->cookieStore();
    if (cookieStore) {
        connect(cookieStore, &QWebEngineCookieStore::cookieAdded, this, 
            [this](const QNetworkCookie& cookie) mutable {
                // Store cookie in our collection
                m_cookies.append(cookie);
                qDebug() << "Cookie added:" << cookie.name() << "from" << cookie.domain();
            });
        
        connect(cookieStore, &QWebEngineCookieStore::cookieRemoved, this,
            [this](const QNetworkCookie& cookie) mutable {
                // Remove cookie from our collection
                m_cookies.removeAll(cookie);
                qDebug() << "Cookie removed:" << cookie.name();
            });
    }
}

QWebEngineView* BrowserWindow::createWebView()
{
    QWebEngineView* webView = new QWebEngineView(this);
    
    // Create page with shared profile
    QWebEnginePage* page = new QWebEnginePage(m_webProfile, webView);
    webView->setPage(page);
    
    // Apply undetectable stealth mode
    UndetectableStealth::applyStealthMode(page);
    UndetectableStealth::applyStealthProfile(m_webProfile);
    
    // Configure page settings
    QWebEngineSettings* settings = page->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);
    settings->setAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins, false);
    
    // Connect signals
    connect(webView, &QWebEngineView::urlChanged, this, &BrowserWindow::onUrlChanged);
    connect(webView, &QWebEngineView::loadProgress, this, &BrowserWindow::onLoadProgress);
    connect(webView, &QWebEngineView::loadFinished, this, &BrowserWindow::onLoadFinished);
    connect(webView, &QWebEngineView::titleChanged, this, [this, webView](const QString& title) {
        int index = m_tabWidget->indexOf(webView);
        if (index >= 0) {
            QString tabTitle = title.isEmpty() ? "New Tab" : title;
            if (tabTitle.length() > 30) {
                tabTitle = tabTitle.left(27) + "...";
            }
            m_tabWidget->setTabText(index, tabTitle);
        }
    });
    
    return webView;
}

QWebEngineView* BrowserWindow::currentWebView()
{
    return qobject_cast<QWebEngineView*>(m_tabWidget->currentWidget());
}

void BrowserWindow::applyFingerprint()
{
    FingerprintManager* fpManager = Application::instance().fingerprintManager();
    
    if (fpManager && m_webProfile) {
        fpManager->applyFingerprint(m_webProfile, m_profile.fingerprint());
    }
}

void BrowserWindow::applyProxy()
{
    ProxyConfig proxyConfig = m_profile.proxy();
    
    if (proxyConfig.type != ProxyConfig::None) {
        ProxyManager* proxyManager = Application::instance().proxyManager();
        QNetworkProxy proxy = proxyManager->createProxy(proxyConfig);
        
        QNetworkProxy::setApplicationProxy(proxy);
        
        setWindowTitle(QString("SECTA Anti Detect - %1 [Proxy: %2:%3]")
            .arg(m_profile.name())
            .arg(proxyConfig.host)
            .arg(proxyConfig.port));
    } else {
        QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
    }
}

void BrowserWindow::navigate(const QString& url)
{
    QWebEngineView* webView = currentWebView();
    if (!webView) return;
    
    QString finalUrl = url;
    
    // Add http:// if no scheme
    if (!finalUrl.contains("://")) {
        finalUrl = "http://" + finalUrl;
    }
    
    webView->load(QUrl(finalUrl));
}

void BrowserWindow::newTab(const QString& url)
{
    QWebEngineView* webView = createWebView();
    
    QString tabTitle = "New Tab";
    int index = m_tabWidget->addTab(webView, tabTitle);
    m_tabWidget->setCurrentIndex(index);
    
    if (!url.isEmpty()) {
        QString finalUrl = url;
        if (!finalUrl.contains("://")) {
            finalUrl = "http://" + finalUrl;
        }
        webView->load(QUrl(finalUrl));
    }
}

void BrowserWindow::onUrlChanged(const QUrl& url)
{
    QWebEngineView* webView = currentWebView();
    if (webView == sender()) {
        m_addressBar->setText(url.toString());
    }
}

void BrowserWindow::onLoadProgress(int progress)
{
    QWebEngineView* webView = currentWebView();
    if (webView == sender()) {
        if (progress < 100) {
            m_progressBar->setVisible(true);
            m_progressBar->setValue(progress);
        } else {
            m_progressBar->setVisible(false);
        }
    }
}

void BrowserWindow::onLoadFinished(bool success)
{
    QWebEngineView* webView = currentWebView();
    if (webView == sender()) {
        m_progressBar->setVisible(false);
    }
}

void BrowserWindow::onAddressBarReturn()
{
    QString url = m_addressBar->text();
    navigate(url);
}

void BrowserWindow::onBackClicked()
{
    QWebEngineView* webView = currentWebView();
    if (webView) {
        webView->back();
    }
}

void BrowserWindow::onForwardClicked()
{
    QWebEngineView* webView = currentWebView();
    if (webView) {
        webView->forward();
    }
}

void BrowserWindow::onReloadClicked()
{
    QWebEngineView* webView = currentWebView();
    if (webView) {
        webView->reload();
    }
}

void BrowserWindow::onHomeClicked()
{
    navigate("https://www.google.com");
}

void BrowserWindow::onNewTabClicked()
{
    newTab("https://www.google.com");
}

void BrowserWindow::onCloseTabClicked(int index)
{
    if (m_tabWidget->count() > 1) {
        QWidget* widget = m_tabWidget->widget(index);
        m_tabWidget->removeTab(index);
        widget->deleteLater();
    } else {
        // Last tab - close window
        close();
    }
}

void BrowserWindow::onTabChanged(int index)
{
    QWebEngineView* webView = currentWebView();
    if (webView) {
        m_addressBar->setText(webView->url().toString());
    }
}

void BrowserWindow::closeEvent(QCloseEvent* event)
{
    // Save cookies before closing
    saveCookies();
    
    emit windowClosed(m_profile.id());
    event->accept();
}

void BrowserWindow::saveCookies()
{
    qDebug() << "BrowserWindow::saveCookies - Saving cookies for profile:" << m_profile.id();
    qDebug() << "BrowserWindow::saveCookies - Total cookies collected:" << m_cookies.size();
    
    // Save to file using CookieManager
    if (!m_cookies.isEmpty()) {
        CookieManager* cm = Application::instance().cookieManager();
        if (cm) {
            bool success = cm->saveCookiesToProfile(m_profile.id(), m_cookies);
            if (success) {
                qDebug() << "BrowserWindow::saveCookies - Successfully saved" << m_cookies.size() << "cookies to file";
            } else {
                qWarning() << "BrowserWindow::saveCookies - Failed to save cookies to file";
            }
        }
    } else {
        qWarning() << "BrowserWindow::saveCookies - No cookies to save";
    }
}
