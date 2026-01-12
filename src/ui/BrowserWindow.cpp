#include "BrowserWindow.h"
#include "core/Application.h"
#include "fingerprint/FingerprintManager.h"
#include "proxy/ProxyManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QPushButton>
#include <QLabel>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QNetworkProxy>

BrowserWindow::BrowserWindow(const Profile& profile, QWidget *parent)
    : QMainWindow(parent)
    , m_profile(profile)
    , m_webProfile(nullptr)
    , m_webView(nullptr)
    , m_addressBar(nullptr)
    , m_progressBar(nullptr)
{
    // Set timezone environment variable BEFORE creating web engine
    if (!m_profile.fingerprint().timezone.isEmpty()) {
        qputenv("TZ", m_profile.fingerprint().timezone.toUtf8());
        
        // Calculate timezone offset for LD_PRELOAD hook
        // America/Los_Angeles = UTC-8 = -28800 seconds
        QMap<QString, int> timezoneOffsets;
        timezoneOffsets["America/New_York"] = -18000;      // UTC-5
        timezoneOffsets["America/Chicago"] = -21600;       // UTC-6
        timezoneOffsets["America/Denver"] = -25200;        // UTC-7
        timezoneOffsets["America/Los_Angeles"] = -28800;   // UTC-8
        timezoneOffsets["America/Anchorage"] = -32400;     // UTC-9
        timezoneOffsets["Pacific/Honolulu"] = -36000;      // UTC-10
        timezoneOffsets["Europe/London"] = 0;              // UTC+0
        timezoneOffsets["Europe/Paris"] = 3600;            // UTC+1
        timezoneOffsets["Europe/Berlin"] = 3600;           // UTC+1
        timezoneOffsets["Europe/Moscow"] = 10800;          // UTC+3
        timezoneOffsets["Asia/Dubai"] = 14400;             // UTC+4
        timezoneOffsets["Asia/Shanghai"] = 28800;          // UTC+8
        timezoneOffsets["Asia/Tokyo"] = 32400;             // UTC+9
        
        int offset = timezoneOffsets.value(m_profile.fingerprint().timezone, 0);
        qputenv("ANTIDETECT_TZ_OFFSET", QString::number(offset).toUtf8());
        
        qDebug() << "Set TZ environment variable to:" << m_profile.fingerprint().timezone;
        qDebug() << "Set ANTIDETECT_TZ_OFFSET to:" << offset << "seconds";
    }
    
    setupUi();
    setupWebEngine();
    applyFingerprint();
    applyProxy();
    
    setWindowTitle(QString("Antidetect Browser - %1").arg(m_profile.name()));
    resize(1280, 800);
    
    // Navigate to home page
    navigate("https://www.google.com");
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
    
    // Create web view
    m_webView = new QWebEngineView(this);
    mainLayout->addWidget(m_webView);
    
    setCentralWidget(centralWidget);
}

void BrowserWindow::createToolbar()
{
    QToolBar* toolbar = addToolBar("Navigation");
    toolbar->setMovable(false);
    
    // Back button
    QAction* backAction = toolbar->addAction("←");
    backAction->setToolTip("Back");
    connect(backAction, &QAction::triggered, this, &BrowserWindow::onBackClicked);
    
    // Forward button
    QAction* forwardAction = toolbar->addAction("→");
    forwardAction->setToolTip("Forward");
    connect(forwardAction, &QAction::triggered, this, &BrowserWindow::onForwardClicked);
    
    // Reload button
    QAction* reloadAction = toolbar->addAction("⟳");
    reloadAction->setToolTip("Reload");
    connect(reloadAction, &QAction::triggered, this, &BrowserWindow::onReloadClicked);
    
    // Home button
    QAction* homeAction = toolbar->addAction("⌂");
    homeAction->setToolTip("Home");
    connect(homeAction, &QAction::triggered, this, &BrowserWindow::onHomeClicked);
    
    toolbar->addSeparator();
    
    // Check IP button
    QAction* checkIPAction = toolbar->addAction("Check IP");
    checkIPAction->setToolTip("Check your IP address");
    connect(checkIPAction, &QAction::triggered, this, [this]() {
        navigate("https://api.ipify.org?format=json");
    });
    
    toolbar->addSeparator();
    
    // Profile info
    QLabel* profileLabel = new QLabel(QString("Profile: %1").arg(m_profile.name()), this);
    toolbar->addWidget(profileLabel);
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
    
    // Create page with profile
    QWebEnginePage* page = new QWebEnginePage(m_webProfile, m_webView);
    m_webView->setPage(page);
    
    // Configure page settings
    QWebEngineSettings* settings = page->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);
    settings->setAttribute(QWebEngineSettings::AllowGeolocationOnInsecureOrigins, false);
    
    // Connect signals
    connect(m_webView, &QWebEngineView::urlChanged, this, &BrowserWindow::onUrlChanged);
    connect(m_webView, &QWebEngineView::loadProgress, this, &BrowserWindow::onLoadProgress);
    connect(m_webView, &QWebEngineView::loadFinished, this, &BrowserWindow::onLoadFinished);
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
        
        // Set application-wide proxy (affects all network requests)
        // Note: Qt WebEngine uses Chromium's network stack which respects QNetworkProxy
        QNetworkProxy::setApplicationProxy(proxy);
        
        qDebug() << "Proxy configured:" << proxyConfig.toString();
        qDebug() << "Proxy type:" << proxy.type();
        qDebug() << "Proxy host:" << proxy.hostName() << ":" << proxy.port();
        
        if (!proxy.user().isEmpty()) {
            qDebug() << "Proxy auth: user =" << proxy.user();
        }
        
        // Show proxy status in window title
        setWindowTitle(QString("Antidetect Browser - %1 [Proxy: %2:%3]")
            .arg(m_profile.name())
            .arg(proxyConfig.host)
            .arg(proxyConfig.port));
    } else {
        // No proxy - use direct connection
        QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
        qDebug() << "No proxy configured, using direct connection";
    }
}

void BrowserWindow::navigate(const QString& url)
{
    QString finalUrl = url;
    
    // Add http:// if no scheme
    if (!finalUrl.contains("://")) {
        finalUrl = "http://" + finalUrl;
    }
    
    m_webView->load(QUrl(finalUrl));
}

void BrowserWindow::onUrlChanged(const QUrl& url)
{
    m_addressBar->setText(url.toString());
}

void BrowserWindow::onLoadProgress(int progress)
{
    if (progress < 100) {
        m_progressBar->setVisible(true);
        m_progressBar->setValue(progress);
    } else {
        m_progressBar->setVisible(false);
    }
}

void BrowserWindow::onLoadFinished(bool success)
{
    m_progressBar->setVisible(false);
    
    if (!success) {
        qWarning() << "Failed to load page";
    }
}

void BrowserWindow::onAddressBarReturn()
{
    QString url = m_addressBar->text();
    navigate(url);
}

void BrowserWindow::onBackClicked()
{
    m_webView->back();
}

void BrowserWindow::onForwardClicked()
{
    m_webView->forward();
}

void BrowserWindow::onReloadClicked()
{
    m_webView->reload();
}

void BrowserWindow::onHomeClicked()
{
    navigate("https://www.google.com");
}

void BrowserWindow::closeEvent(QCloseEvent* event)
{
    emit windowClosed(m_profile.id());
    event->accept();
}
