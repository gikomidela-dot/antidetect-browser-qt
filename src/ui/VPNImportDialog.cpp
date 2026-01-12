#include "VPNImportDialog.h"
#include "core/Application.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>

VPNImportDialog::VPNImportDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    setWindowTitle("Import VPN Configuration");
    resize(700, 500);
    
    // Connect to VPN manager signals
    VPNManager* vpnManager = Application::instance().vpnManager();
    connect(vpnManager, &VPNManager::configImported, this, &VPNImportDialog::onConfigImported);
    connect(vpnManager, &VPNManager::importError, this, &VPNImportDialog::onImportError);
}

void VPNImportDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Import from file section
    QGroupBox* fileGroup = new QGroupBox("Import from File", this);
    QVBoxLayout* fileLayout = new QVBoxLayout(fileGroup);
    
    QLabel* fileLabel = new QLabel("Supported formats: .ovpn (OpenVPN), .conf (WireGuard), .json (Shadowsocks/V2Ray)", this);
    fileLabel->setWordWrap(true);
    fileLayout->addWidget(fileLabel);
    
    m_fileButton = new QPushButton("Select File...", this);
    connect(m_fileButton, &QPushButton::clicked, this, &VPNImportDialog::onImportFromFile);
    fileLayout->addWidget(m_fileButton);
    
    mainLayout->addWidget(fileGroup);
    
    // Import from URL section
    QGroupBox* urlGroup = new QGroupBox("Import from URL", this);
    QVBoxLayout* urlLayout = new QVBoxLayout(urlGroup);
    
    QLabel* urlLabel = new QLabel("Supported: ss:// (Shadowsocks), vmess:// (V2Ray VMess), vless:// (V2Ray VLESS)", this);
    urlLabel->setWordWrap(true);
    urlLayout->addWidget(urlLabel);
    
    QHBoxLayout* urlInputLayout = new QHBoxLayout();
    m_urlEdit = new QLineEdit(this);
    m_urlEdit->setPlaceholderText("Paste VPN URL here (e.g., ss://...)");
    urlInputLayout->addWidget(m_urlEdit);
    
    m_urlButton = new QPushButton("Import", this);
    connect(m_urlButton, &QPushButton::clicked, this, &VPNImportDialog::onImportFromUrl);
    urlInputLayout->addWidget(m_urlButton);
    
    urlLayout->addLayout(urlInputLayout);
    mainLayout->addWidget(urlGroup);
    
    // Import from text section
    QGroupBox* textGroup = new QGroupBox("Import from Text", this);
    QVBoxLayout* textLayout = new QVBoxLayout(textGroup);
    
    QHBoxLayout* typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel("Config Type:", this));
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItems({"OpenVPN", "WireGuard", "Shadowsocks", "V2Ray"});
    typeLayout->addWidget(m_typeCombo);
    typeLayout->addStretch();
    textLayout->addLayout(typeLayout);
    
    m_textEdit = new QTextEdit(this);
    m_textEdit->setPlaceholderText("Paste configuration text here...");
    m_textEdit->setMaximumHeight(150);
    textLayout->addWidget(m_textEdit);
    
    m_textButton = new QPushButton("Import from Text", this);
    connect(m_textButton, &QPushButton::clicked, this, &VPNImportDialog::onImportFromText);
    textLayout->addWidget(m_textButton);
    
    mainLayout->addWidget(textGroup);
    
    // Imported configs list
    QGroupBox* listGroup = new QGroupBox("Imported Configurations", this);
    QVBoxLayout* listLayout = new QVBoxLayout(listGroup);
    
    m_configList = new QListWidget(this);
    listLayout->addWidget(m_configList);
    
    mainLayout->addWidget(listGroup);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_closeButton = new QPushButton("Close", this);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Load existing configs
    updateConfigList();
}

void VPNImportDialog::onImportFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select VPN Configuration File",
        QString(),
        "VPN Configs (*.ovpn *.conf *.json);;OpenVPN (*.ovpn);;WireGuard (*.conf);;JSON (*.json);;All Files (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    VPNManager* vpnManager = Application::instance().vpnManager();
    if (vpnManager->importConfig(fileName)) {
        QMessageBox::information(this, "Success", "VPN configuration imported successfully!");
    }
}

void VPNImportDialog::onImportFromUrl()
{
    QString url = m_urlEdit->text().trimmed();
    
    if (url.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a VPN URL");
        return;
    }
    
    VPNManager* vpnManager = Application::instance().vpnManager();
    if (vpnManager->importFromUrl(url)) {
        QMessageBox::information(this, "Success", "VPN configuration imported from URL!");
        m_urlEdit->clear();
    }
}

void VPNImportDialog::onImportFromText()
{
    QString text = m_textEdit->toPlainText().trimmed();
    
    if (text.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please paste configuration text");
        return;
    }
    
    QString type = m_typeCombo->currentText().toLower();
    
    // Save to temporary file
    QString tempPath = QDir::temp().filePath("vpn_config_temp");
    
    if (type == "openvpn") {
        tempPath += ".ovpn";
    } else if (type == "wireguard") {
        tempPath += ".conf";
    } else {
        tempPath += ".json";
    }
    
    QFile file(tempPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Failed to create temporary file");
        return;
    }
    
    file.write(text.toUtf8());
    file.close();
    
    // Import from temp file
    VPNManager* vpnManager = Application::instance().vpnManager();
    if (vpnManager->importConfig(tempPath)) {
        QMessageBox::information(this, "Success", "VPN configuration imported from text!");
        m_textEdit->clear();
    }
    
    // Clean up temp file
    QFile::remove(tempPath);
}

void VPNImportDialog::onConfigImported(const QString& name)
{
    updateConfigList();
    
    VPNManager* vpnManager = Application::instance().vpnManager();
    VPNConfig config = vpnManager->getConfig(name);
    m_importedConfigs.append(config);
}

void VPNImportDialog::onImportError(const QString& error)
{
    QMessageBox::critical(this, "Import Error", error);
}

void VPNImportDialog::updateConfigList()
{
    m_configList->clear();
    
    VPNManager* vpnManager = Application::instance().vpnManager();
    QList<VPNConfig> configs = vpnManager->getConfigs();
    
    for (const VPNConfig& config : configs) {
        QString displayText = QString("%1 (%2) - %3:%4")
            .arg(config.name)
            .arg(config.type)
            .arg(config.parameters.value("server", "N/A"))
            .arg(config.parameters.value("port", "N/A"));
        
        m_configList->addItem(displayText);
    }
}
