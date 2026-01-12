#include "ProxyImportDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

ProxyImportDialog::ProxyImportDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    setWindowTitle("Mass Proxy Import");
    resize(600, 500);
}

void ProxyImportDialog::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Proxy list input
    QGroupBox* proxyGroup = new QGroupBox("Proxy List", this);
    QVBoxLayout* proxyLayout = new QVBoxLayout(proxyGroup);
    
    QLabel* proxyLabel = new QLabel("Enter proxy list (one per line):\nFormat: host:port:username:password", this);
    proxyLayout->addWidget(proxyLabel);
    
    m_proxyTextEdit = new QTextEdit(this);
    m_proxyTextEdit->setPlaceholderText("103.47.53.144:8442:user:pass\n104.48.54.145:8443:user2:pass2\n...");
    proxyLayout->addWidget(m_proxyTextEdit);
    
    mainLayout->addWidget(proxyGroup);
    
    // User Agent selection
    QGroupBox* uaGroup = new QGroupBox("Select User Agents", this);
    QVBoxLayout* uaLayout = new QVBoxLayout(uaGroup);
    
    QLabel* uaLabel = new QLabel("Select which User Agents to use for profiles:", this);
    uaLayout->addWidget(uaLabel);
    
    // Selection buttons
    QHBoxLayout* selectLayout = new QHBoxLayout();
    QPushButton* selectAllBtn = new QPushButton("Select All", this);
    connect(selectAllBtn, &QPushButton::clicked, this, &ProxyImportDialog::onSelectAll);
    selectLayout->addWidget(selectAllBtn);
    
    QPushButton* deselectAllBtn = new QPushButton("Deselect All", this);
    connect(deselectAllBtn, &QPushButton::clicked, this, &ProxyImportDialog::onDeselectAll);
    selectLayout->addWidget(deselectAllBtn);
    selectLayout->addStretch();
    
    uaLayout->addLayout(selectLayout);
    
    // User Agent checkboxes
    QStringList userAgents = {
        "Windows 10 - Chrome 120",
        "Windows 11 - Chrome 121",
        "macOS Sonoma - Safari 17",
        "macOS Ventura - Chrome 120",
        "Linux Ubuntu - Firefox 121",
        "Android 14 - Chrome Mobile",
        "iOS 17 - Safari Mobile"
    };
    
    for (const QString& ua : userAgents) {
        QCheckBox* checkbox = new QCheckBox(ua, this);
        checkbox->setChecked(true); // Default: all selected
        m_userAgentCheckboxes[ua] = checkbox;
        uaLayout->addWidget(checkbox);
    }
    
    mainLayout->addWidget(uaGroup);
    
    // Info label
    QLabel* infoLabel = new QLabel(
        "<b>Note:</b> Will create one profile per proxy × selected User Agents.\n"
        "Example: 10 proxies × 3 User Agents = 30 profiles",
        this
    );
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { background-color: #ffffcc; padding: 10px; border-radius: 5px; }");
    mainLayout->addWidget(infoLabel);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton* importBtn = new QPushButton("Import && Create Profiles", this);
    importBtn->setDefault(true);
    connect(importBtn, &QPushButton::clicked, this, &ProxyImportDialog::onImport);
    buttonLayout->addWidget(importBtn);
    
    QPushButton* cancelBtn = new QPushButton("Cancel", this);
    connect(cancelBtn, &QPushButton::clicked, this, &ProxyImportDialog::onCancel);
    buttonLayout->addWidget(cancelBtn);
    
    mainLayout->addLayout(buttonLayout);
}

void ProxyImportDialog::onImport()
{
    // Get proxy list
    QString proxyText = m_proxyTextEdit->toPlainText().trimmed();
    if (proxyText.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter at least one proxy!");
        return;
    }
    
    m_result.proxyList = proxyText.split('\n', Qt::SkipEmptyParts);
    
    // Get selected User Agents
    m_result.selectedUserAgents.clear();
    for (auto it = m_userAgentCheckboxes.begin(); it != m_userAgentCheckboxes.end(); ++it) {
        if (it.value()->isChecked()) {
            m_result.selectedUserAgents.append(it.key());
        }
    }
    
    if (m_result.selectedUserAgents.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select at least one User Agent!");
        return;
    }
    
    // Calculate total profiles
    int totalProfiles = m_result.proxyList.size() * m_result.selectedUserAgents.size();
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Import",
        QString("This will create %1 profiles:\n\n"
                "• %2 proxies\n"
                "• %3 User Agents\n"
                "• %4 total profiles\n\n"
                "Continue?")
            .arg(totalProfiles)
            .arg(m_result.proxyList.size())
            .arg(m_result.selectedUserAgents.size())
            .arg(totalProfiles),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        m_result.createMultipleProfiles = true;
        accept();
    }
}

void ProxyImportDialog::onCancel()
{
    reject();
}

void ProxyImportDialog::onSelectAll()
{
    for (QCheckBox* checkbox : m_userAgentCheckboxes) {
        checkbox->setChecked(true);
    }
}

void ProxyImportDialog::onDeselectAll()
{
    for (QCheckBox* checkbox : m_userAgentCheckboxes) {
        checkbox->setChecked(false);
    }
}
