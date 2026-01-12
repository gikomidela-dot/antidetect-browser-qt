#include "Dashboard.h"
#include "ProxyImportDialog.h"
#include "core/Application.h"
#include "core/BrowserWindowManager.h"
#include "core/CookieManager.h"
#include "profiles/ProfileManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QDir>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    loadProfiles();
}

void Dashboard::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Search bar
    QHBoxLayout* searchLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Search profiles...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &Dashboard::onSearchTextChanged);
    searchLayout->addWidget(m_searchEdit);
    mainLayout->addLayout(searchLayout);
    
    // Profile table
    m_profileTable = new QTableWidget(this);
    m_profileTable->setColumnCount(5);
    m_profileTable->setHorizontalHeaderLabels({"Name", "Tags", "Proxy", "Created", "Updated"});
    m_profileTable->horizontalHeader()->setStretchLastSection(true);
    m_profileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_profileTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_profileTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(m_profileTable);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_createButton = new QPushButton("Create Profile", this);
    connect(m_createButton, &QPushButton::clicked, this, &Dashboard::onCreateProfile);
    buttonLayout->addWidget(m_createButton);
    
    QPushButton* massImportBtn = new QPushButton("Mass Import Proxy", this);
    massImportBtn->setToolTip("Import multiple proxies and create profiles automatically");
    connect(massImportBtn, &QPushButton::clicked, this, &Dashboard::onMassImportProxy);
    buttonLayout->addWidget(massImportBtn);
    
    m_editButton = new QPushButton("Edit", this);
    connect(m_editButton, &QPushButton::clicked, this, &Dashboard::onEditProfile);
    buttonLayout->addWidget(m_editButton);
    
    m_deleteButton = new QPushButton("Delete", this);
    connect(m_deleteButton, &QPushButton::clicked, this, &Dashboard::onDeleteProfile);
    buttonLayout->addWidget(m_deleteButton);
    
    m_launchButton = new QPushButton("Launch", this);
    connect(m_launchButton, &QPushButton::clicked, this, &Dashboard::onLaunchProfile);
    buttonLayout->addWidget(m_launchButton);
    
    buttonLayout->addSpacing(20);
    
    m_exportCookiesButton = new QPushButton("Export Cookies", this);
    connect(m_exportCookiesButton, &QPushButton::clicked, this, &Dashboard::onExportCookies);
    buttonLayout->addWidget(m_exportCookiesButton);
    
    m_importCookiesButton = new QPushButton("Import Cookies", this);
    connect(m_importCookiesButton, &QPushButton::clicked, this, &Dashboard::onImportCookies);
    buttonLayout->addWidget(m_importCookiesButton);
    
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

void Dashboard::refresh()
{
    loadProfiles();
}

void Dashboard::loadProfiles()
{
    m_profileTable->setRowCount(0);
    
    ProfileManager* pm = Application::instance().profileManager();
    QList<Profile> profiles = pm->getAllProfiles();
    
    for (const Profile& profile : profiles) {
        int row = m_profileTable->rowCount();
        m_profileTable->insertRow(row);
        
        m_profileTable->setItem(row, 0, new QTableWidgetItem(profile.name()));
        m_profileTable->setItem(row, 1, new QTableWidgetItem(profile.tags().join(", ")));
        m_profileTable->setItem(row, 2, new QTableWidgetItem(profile.proxy().toString()));
        m_profileTable->setItem(row, 3, new QTableWidgetItem(profile.createdAt().toString("yyyy-MM-dd hh:mm")));
        m_profileTable->setItem(row, 4, new QTableWidgetItem(profile.updatedAt().toString("yyyy-MM-dd hh:mm")));
        
        // Store profile ID in first column
        m_profileTable->item(row, 0)->setData(Qt::UserRole, profile.id());
    }
}

void Dashboard::filterProfiles(const QString& query)
{
    for (int row = 0; row < m_profileTable->rowCount(); ++row) {
        bool match = false;
        
        for (int col = 0; col < m_profileTable->columnCount(); ++col) {
            QTableWidgetItem* item = m_profileTable->item(row, col);
            if (item && item->text().contains(query, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        
        m_profileTable->setRowHidden(row, !match);
    }
}

void Dashboard::onCreateProfile()
{
    emit createProfileRequested();
}

void Dashboard::onEditProfile()
{
    int row = m_profileTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a profile to edit.");
        return;
    }
    
    QString profileId = m_profileTable->item(row, 0)->data(Qt::UserRole).toString();
    emit editProfileRequested(profileId);
}

void Dashboard::onDeleteProfile()
{
    int row = m_profileTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a profile to delete.");
        return;
    }
    
    QString profileName = m_profileTable->item(row, 0)->text();
    QString profileId = m_profileTable->item(row, 0)->data(Qt::UserRole).toString();
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Are you sure you want to delete profile '%1'?").arg(profileName),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        ProfileManager* pm = Application::instance().profileManager();
        if (pm->deleteProfile(profileId)) {
            loadProfiles();
        }
    }
}

void Dashboard::onLaunchProfile()
{
    int row = m_profileTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a profile to launch.");
        return;
    }
    
    QString profileId = m_profileTable->item(row, 0)->data(Qt::UserRole).toString();
    emit launchProfileRequested(profileId);
}

void Dashboard::onSearchTextChanged(const QString& text)
{
    filterProfiles(text);
}

void Dashboard::onExportCookies()
{
    int row = m_profileTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a profile to export cookies.");
        return;
    }
    
    QString profileId = m_profileTable->item(row, 0)->data(Qt::UserRole).toString();
    QString profileName = m_profileTable->item(row, 0)->text();
    
    // Use Documents folder as default location
    QString defaultPath = QDir::toNativeSeparators(
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + 
        "/" + profileName + "_cookies.json"
    );
    
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Cookies",
        defaultPath,
        "JSON Files (*.json);;Netscape Format (*.txt);;All Files (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // Convert to native separators for display
    QString nativeFileName = QDir::toNativeSeparators(fileName);
    QString format = fileName.endsWith(".txt") ? "netscape" : "json";
    
    CookieManager* cm = Application::instance().cookieManager();
    if (cm->exportCookies(profileId, fileName, format)) {
        QMessageBox::information(this, "Success", 
            QString("Cookies exported successfully!\n\nFile: %1\nFormat: %2")
            .arg(nativeFileName)
            .arg(format));
    } else {
        QMessageBox::warning(this, "Error", 
            QString("Failed to export cookies.\n\nProfile: %1\nFile: %2\n\nNote: Make sure the profile has been launched at least once to have cookies.")
            .arg(profileName)
            .arg(nativeFileName));
    }
}

void Dashboard::onImportCookies()
{
    int row = m_profileTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a profile to import cookies.");
        return;
    }
    
    QString profileId = m_profileTable->item(row, 0)->data(Qt::UserRole).toString();
    
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Import Cookies",
        QString(),
        "JSON Files (*.json);;Netscape Format (*.txt);;All Files (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QString format = fileName.endsWith(".txt") ? "netscape" : "json";
    
    CookieManager* cm = Application::instance().cookieManager();
    if (cm->importCookies(profileId, fileName, format)) {
        QMessageBox::information(this, "Success", "Cookies imported successfully!");
    } else {
        QMessageBox::warning(this, "Error", "Failed to import cookies.");
    }
}

Profile Dashboard::createProfileFromUserAgent(const QString& uaString)
{
    Profile profile;
    ProfileManager* pm = Application::instance().profileManager();
    
    // Parse: "Windows 10 - Chrome 120 (1920x1080)"
    QRegularExpression re("^(.+?)\\s+-\\s+(.+?)\\s+\\((\\d+)x(\\d+)\\)$");
    QRegularExpressionMatch match = re.match(uaString);
    
    QString os, browser;
    int screenWidth = 1920, screenHeight = 1080;
    
    if (match.hasMatch()) {
        os = match.captured(1).trimmed();
        browser = match.captured(2).trimmed();
        screenWidth = match.captured(3).toInt();
        screenHeight = match.captured(4).toInt();
    }
    
    // Get base template
    QString templateName;
    if (os.contains("Windows")) {
        templateName = "Windows 10";
    } else if (os.contains("macOS")) {
        templateName = "macOS Sonoma";
    } else if (os.contains("Linux")) {
        templateName = "Linux Ubuntu";
    } else if (os.contains("Android")) {
        templateName = "Android 14";
    } else if (os.contains("iOS")) {
        templateName = "iOS 17";
    } else {
        templateName = "Windows 10";
    }
    
    profile = pm->createFromTemplate(templateName);
    
    // Update fingerprint with real User Agent
    FingerprintConfig fp = profile.fingerprint();
    
    // Set screen resolution
    fp.screenWidth = screenWidth;
    fp.screenHeight = screenHeight;
    
    // Generate real User Agent
    if (os.contains("Windows 10") && browser.contains("Chrome 120")) {
        fp.userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36";
        fp.platform = "Win32";
    } else if (os.contains("Windows 10") && browser.contains("Chrome 121")) {
        fp.userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36";
        fp.platform = "Win32";
    } else if (os.contains("Windows 11") && browser.contains("Chrome 120")) {
        fp.userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36";
        fp.platform = "Win32";
    } else if (os.contains("Windows 11") && browser.contains("Edge 120")) {
        fp.userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36 Edg/120.0.0.0";
        fp.platform = "Win32";
    } else if (os.contains("Windows 11") && browser.contains("Firefox 121")) {
        fp.userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:121.0) Gecko/20100101 Firefox/121.0";
        fp.platform = "Win32";
    } else if (os.contains("macOS Sonoma") && browser.contains("Safari 17")) {
        fp.userAgent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 14_1) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.1 Safari/605.1.15";
        fp.platform = "MacIntel";
    } else if (os.contains("macOS Sonoma") && browser.contains("Chrome 120")) {
        fp.userAgent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36";
        fp.platform = "MacIntel";
    } else if (os.contains("macOS Ventura") && browser.contains("Safari 16")) {
        fp.userAgent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 13_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.6 Safari/605.1.15";
        fp.platform = "MacIntel";
    } else if (os.contains("macOS Ventura") && browser.contains("Chrome 119")) {
        fp.userAgent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36";
        fp.platform = "MacIntel";
    } else if (os.contains("Linux") && browser.contains("Firefox 121")) {
        fp.userAgent = "Mozilla/5.0 (X11; Linux x86_64; rv:121.0) Gecko/20100101 Firefox/121.0";
        fp.platform = "Linux x86_64";
    } else if (os.contains("Linux") && browser.contains("Chrome 120")) {
        fp.userAgent = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36";
        fp.platform = "Linux x86_64";
    } else if (os.contains("Android 14")) {
        fp.userAgent = "Mozilla/5.0 (Linux; Android 14) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.6099.144 Mobile Safari/537.36";
        fp.platform = "Linux armv8l";
    } else if (os.contains("Android 13")) {
        fp.userAgent = "Mozilla/5.0 (Linux; Android 13) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.6099.144 Mobile Safari/537.36";
        fp.platform = "Linux armv8l";
    } else if (os.contains("iOS 17")) {
        fp.userAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 17_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.1 Mobile/15E148 Safari/604.1";
        fp.platform = "iPhone";
    } else if (os.contains("iOS 16")) {
        fp.userAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 16_6 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.6 Mobile/15E148 Safari/604.1";
        fp.platform = "iPhone";
    }
    
    profile.setFingerprint(fp);
    return profile;
}

void Dashboard::onMassImportProxy()
{
    ProxyImportDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        ProxyImportDialog::ImportResult result = dialog.getResult();
        
        int totalProfiles = result.proxyList.size() * result.selectedUserAgents.size();
        
        QProgressDialog progress("Creating profiles...", "Cancel", 0, totalProfiles, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        
        int created = 0;
        int counter = 1;
        
        for (const QString& proxyString : result.proxyList) {
            // Parse proxy: host:port:username:password
            QStringList parts = proxyString.split(":");
            if (parts.size() < 2) continue;
            
            QString host = parts[0].trimmed();
            int port = parts[1].trimmed().toInt();
            QString username = parts.size() >= 3 ? parts[2].trimmed() : "";
            QString password = parts.size() >= 4 ? parts[3].trimmed() : "";
            
            for (const QString& uaString : result.selectedUserAgents) {
                if (progress.wasCanceled()) {
                    break;
                }
                
                // Create profile with real User Agent
                Profile profile = createProfileFromUserAgent(uaString);
                
                // Set name
                profile.setName(QString("Profile_%1_%2").arg(counter).arg(uaString.left(20)));
                
                // Set proxy
                ProxyConfig proxy;
                proxy.type = ProxyConfig::HTTP;
                proxy.host = host;
                proxy.port = port;
                proxy.username = username;
                proxy.password = password;
                profile.setProxy(proxy);
                
                // Set tags
                profile.setTags(QStringList() << "mass-import" << host);
                
                // Create profile
                ProfileManager* pm = Application::instance().profileManager();
                if (pm->createProfile(profile)) {
                    created++;
                }
                
                counter++;
                progress.setValue(created);
            }
            
            if (progress.wasCanceled()) {
                break;
            }
        }
        
        progress.setValue(totalProfiles);
        
        // Refresh table
        loadProfiles();
        
        QMessageBox::information(this, "Success", 
            QString("Successfully created %1 profiles!").arg(created));
    }
}
