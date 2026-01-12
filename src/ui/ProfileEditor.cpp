#include "ProfileEditor.h"
#include "core/Application.h"
#include "profiles/ProfileManager.h"
#include "vpn/VPNManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QClipboard>
#include <QApplication>

ProfileEditor::ProfileEditor(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void ProfileEditor::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Basic info
    QGroupBox* basicGroup = new QGroupBox("Basic Information", this);
    QFormLayout* basicLayout = new QFormLayout(basicGroup);
    
    m_nameEdit = new QLineEdit(this);
    basicLayout->addRow("Name:", m_nameEdit);
    
    m_notesEdit = new QTextEdit(this);
    m_notesEdit->setMaximumHeight(60);
    basicLayout->addRow("Notes:", m_notesEdit);
    
    m_tagsEdit = new QLineEdit(this);
    basicLayout->addRow("Tags:", m_tagsEdit);
    
    mainLayout->addWidget(basicGroup);
    
    // Fingerprint
    QGroupBox* fpGroup = new QGroupBox("Fingerprint", this);
    QFormLayout* fpLayout = new QFormLayout(fpGroup);
    
    m_templateCombo = new QComboBox(this);
    m_templateCombo->addItem("-- Select Template --");
    m_templateCombo->addItems(Application::instance().profileManager()->getTemplateNames());
    connect(m_templateCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ProfileEditor::onTemplateSelected);
    fpLayout->addRow("Template:", m_templateCombo);
    
    m_userAgentEdit = new QLineEdit(this);
    fpLayout->addRow("User Agent:", m_userAgentEdit);
    
    m_platformEdit = new QLineEdit(this);
    fpLayout->addRow("Platform:", m_platformEdit);
    
    m_hardwareConcurrencySpin = new QSpinBox(this);
    m_hardwareConcurrencySpin->setRange(1, 32);
    fpLayout->addRow("CPU Cores:", m_hardwareConcurrencySpin);
    
    m_deviceMemorySpin = new QSpinBox(this);
    m_deviceMemorySpin->setRange(1, 64);
    fpLayout->addRow("RAM (GB):", m_deviceMemorySpin);
    
    m_screenWidthSpin = new QSpinBox(this);
    m_screenWidthSpin->setRange(800, 7680);
    fpLayout->addRow("Screen Width:", m_screenWidthSpin);
    
    m_screenHeightSpin = new QSpinBox(this);
    m_screenHeightSpin->setRange(600, 4320);
    fpLayout->addRow("Screen Height:", m_screenHeightSpin);
    
    m_timezoneEdit = new QLineEdit(this);
    fpLayout->addRow("Timezone:", m_timezoneEdit);
    
    m_localeEdit = new QLineEdit(this);
    fpLayout->addRow("Locale:", m_localeEdit);
    
    m_webglVendorEdit = new QLineEdit(this);
    fpLayout->addRow("WebGL Vendor:", m_webglVendorEdit);
    
    m_webglRendererEdit = new QLineEdit(this);
    fpLayout->addRow("WebGL Renderer:", m_webglRendererEdit);
    
    m_canvasNoiseCheck = new QCheckBox("Enable Canvas Noise", this);
    m_canvasNoiseCheck->setChecked(true);
    fpLayout->addRow("", m_canvasNoiseCheck);
    
    m_audioNoiseCheck = new QCheckBox("Enable Audio Noise", this);
    m_audioNoiseCheck->setChecked(true);
    fpLayout->addRow("", m_audioNoiseCheck);
    
    m_webrtcProtectionCheck = new QCheckBox("Enable WebRTC Protection", this);
    m_webrtcProtectionCheck->setChecked(true);
    fpLayout->addRow("", m_webrtcProtectionCheck);
    
    mainLayout->addWidget(fpGroup);
    
    // Proxy
    QGroupBox* proxyGroup = new QGroupBox("Proxy", this);
    QFormLayout* proxyLayout = new QFormLayout(proxyGroup);
    
    m_proxyTypeCombo = new QComboBox(this);
    m_proxyTypeCombo->addItems({"None", "HTTP", "HTTPS", "SOCKS4", "SOCKS5"});
    proxyLayout->addRow("Type:", m_proxyTypeCombo);
    
    m_proxyHostEdit = new QLineEdit(this);
    proxyLayout->addRow("Host:", m_proxyHostEdit);
    
    m_proxyPortSpin = new QSpinBox(this);
    m_proxyPortSpin->setRange(1, 65535);
    proxyLayout->addRow("Port:", m_proxyPortSpin);
    
    m_proxyUsernameEdit = new QLineEdit(this);
    proxyLayout->addRow("Username:", m_proxyUsernameEdit);
    
    m_proxyPasswordEdit = new QLineEdit(this);
    m_proxyPasswordEdit->setEchoMode(QLineEdit::Password);
    proxyLayout->addRow("Password:", m_proxyPasswordEdit);
    
    // Parse proxy button
    QPushButton* parseProxyBtn = new QPushButton("Parse Proxy String", this);
    parseProxyBtn->setToolTip("Parse proxy in format: host:port:username:password");
    connect(parseProxyBtn, &QPushButton::clicked, this, &ProfileEditor::onParseProxy);
    proxyLayout->addRow("", parseProxyBtn);
    
    mainLayout->addWidget(proxyGroup);
    
    // VPN
    QGroupBox* vpnGroup = new QGroupBox("VPN Configuration", this);
    QFormLayout* vpnLayout = new QFormLayout(vpnGroup);
    
    m_vpnCombo = new QComboBox(this);
    m_vpnCombo->addItem("-- No VPN --");
    
    // Load VPN configs from VPNManager
    VPNManager* vpnManager = Application::instance().vpnManager();
    QList<VPNConfig> vpnConfigs = vpnManager->getConfigs();
    for (const VPNConfig& config : vpnConfigs) {
        QString displayText = QString("%1 (%2) - %3:%4")
            .arg(config.name)
            .arg(config.type)
            .arg(config.parameters.value("server", "N/A"))
            .arg(config.parameters.value("port", "N/A"));
        m_vpnCombo->addItem(displayText, config.name);
    }
    
    vpnLayout->addRow("VPN Config:", m_vpnCombo);
    
    mainLayout->addWidget(vpnGroup);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton* saveBtn = new QPushButton("Save", this);
    connect(saveBtn, &QPushButton::clicked, this, &ProfileEditor::onSave);
    buttonLayout->addWidget(saveBtn);
    
    QPushButton* cancelBtn = new QPushButton("Cancel", this);
    connect(cancelBtn, &QPushButton::clicked, this, &ProfileEditor::onCancel);
    buttonLayout->addWidget(cancelBtn);
    
    mainLayout->addLayout(buttonLayout);
}

void ProfileEditor::loadProfile(const QString& profileId)
{
    m_currentProfileId = profileId;
    Profile profile = Application::instance().profileManager()->getProfile(profileId);
    setProfileToForm(profile);
}

void ProfileEditor::clear()
{
    m_currentProfileId.clear();
    m_nameEdit->clear();
    m_notesEdit->clear();
    m_tagsEdit->clear();
    m_templateCombo->setCurrentIndex(0);
}

void ProfileEditor::onSave()
{
    Profile profile = getProfileFromForm();
    
    if (m_currentProfileId.isEmpty()) {
        if (Application::instance().profileManager()->createProfile(profile)) {
            emit profileSaved(profile.id());
        }
    } else {
        profile.setId(m_currentProfileId);
        if (Application::instance().profileManager()->updateProfile(profile)) {
            emit profileSaved(profile.id());
        }
    }
}

void ProfileEditor::onCancel()
{
    emit cancelled();
}

void ProfileEditor::onTemplateSelected(int index)
{
    if (index <= 0) return;
    
    QString templateName = m_templateCombo->currentText();
    Profile profile = Application::instance().profileManager()->createFromTemplate(templateName);
    setProfileToForm(profile);
}

Profile ProfileEditor::getProfileFromForm()
{
    Profile profile;
    profile.setName(m_nameEdit->text());
    profile.setNotes(m_notesEdit->toPlainText());
    profile.setTags(m_tagsEdit->text().split(",", Qt::SkipEmptyParts));
    
    FingerprintConfig fp;
    fp.userAgent = m_userAgentEdit->text();
    fp.platform = m_platformEdit->text();
    fp.hardwareConcurrency = m_hardwareConcurrencySpin->value();
    fp.deviceMemory = m_deviceMemorySpin->value();
    fp.screenWidth = m_screenWidthSpin->value();
    fp.screenHeight = m_screenHeightSpin->value();
    fp.timezone = m_timezoneEdit->text();
    fp.locale = m_localeEdit->text();
    fp.webglVendor = m_webglVendorEdit->text();
    fp.webglRenderer = m_webglRendererEdit->text();
    fp.canvasNoise = m_canvasNoiseCheck->isChecked();
    fp.audioNoise = m_audioNoiseCheck->isChecked();
    fp.webrtcProtection = m_webrtcProtectionCheck->isChecked();
    profile.setFingerprint(fp);
    
    ProxyConfig proxy;
    proxy.type = static_cast<ProxyConfig::Type>(m_proxyTypeCombo->currentIndex());
    proxy.host = m_proxyHostEdit->text();
    proxy.port = m_proxyPortSpin->value();
    proxy.username = m_proxyUsernameEdit->text();
    proxy.password = m_proxyPasswordEdit->text();
    profile.setProxy(proxy);
    
    // VPN
    if (m_vpnCombo->currentIndex() > 0) {
        profile.setVpnConfig(m_vpnCombo->currentData().toString());
    }
    
    return profile;
}

void ProfileEditor::setProfileToForm(const Profile& profile)
{
    m_nameEdit->setText(profile.name());
    m_notesEdit->setPlainText(profile.notes());
    m_tagsEdit->setText(profile.tags().join(","));
    
    FingerprintConfig fp = profile.fingerprint();
    m_userAgentEdit->setText(fp.userAgent);
    m_platformEdit->setText(fp.platform);
    m_hardwareConcurrencySpin->setValue(fp.hardwareConcurrency);
    m_deviceMemorySpin->setValue(fp.deviceMemory);
    m_screenWidthSpin->setValue(fp.screenWidth);
    m_screenHeightSpin->setValue(fp.screenHeight);
    m_timezoneEdit->setText(fp.timezone);
    m_localeEdit->setText(fp.locale);
    m_webglVendorEdit->setText(fp.webglVendor);
    m_webglRendererEdit->setText(fp.webglRenderer);
    m_canvasNoiseCheck->setChecked(fp.canvasNoise);
    m_audioNoiseCheck->setChecked(fp.audioNoise);
    m_webrtcProtectionCheck->setChecked(fp.webrtcProtection);
    
    ProxyConfig proxy = profile.proxy();
    m_proxyTypeCombo->setCurrentIndex(static_cast<int>(proxy.type));
    m_proxyHostEdit->setText(proxy.host);
    m_proxyPortSpin->setValue(proxy.port);
    m_proxyUsernameEdit->setText(proxy.username);
    m_proxyPasswordEdit->setText(proxy.password);
    
    // VPN
    QString vpnConfig = profile.vpnConfig();
    if (!vpnConfig.isEmpty()) {
        int index = m_vpnCombo->findData(vpnConfig);
        if (index >= 0) {
            m_vpnCombo->setCurrentIndex(index);
        }
    }
}

void ProfileEditor::onParseProxy()
{
    // Get proxy string from clipboard or show input dialog
    QString proxyString = QApplication::clipboard()->text().trimmed();
    
    if (proxyString.isEmpty()) {
        bool ok;
        proxyString = QInputDialog::getText(this, "Parse Proxy",
            "Enter proxy string (host:port:username:password):",
            QLineEdit::Normal, "", &ok);
        
        if (!ok || proxyString.isEmpty()) {
            return;
        }
    }
    
    // Parse proxy string: host:port:username:password
    QStringList parts = proxyString.split(":");
    
    if (parts.size() < 2) {
        QMessageBox::warning(this, "Parse Error", 
            "Invalid proxy format. Expected: host:port:username:password");
        return;
    }
    
    // Set host
    m_proxyHostEdit->setText(parts[0].trimmed());
    
    // Set port
    bool portOk;
    int port = parts[1].trimmed().toInt(&portOk);
    if (portOk && port > 0 && port <= 65535) {
        m_proxyPortSpin->setValue(port);
    }
    
    // Set username (if provided)
    if (parts.size() >= 3) {
        m_proxyUsernameEdit->setText(parts[2].trimmed());
    }
    
    // Set password (if provided)
    if (parts.size() >= 4) {
        m_proxyPasswordEdit->setText(parts[3].trimmed());
    }
    
    // Auto-detect proxy type (default to HTTP)
    if (m_proxyTypeCombo->currentIndex() == 0) { // If "None" selected
        m_proxyTypeCombo->setCurrentIndex(1); // Set to HTTP
    }
    
    QMessageBox::information(this, "Success", 
        QString("Proxy parsed successfully!\nHost: %1\nPort: %2")
        .arg(parts[0].trimmed())
        .arg(port));
}
