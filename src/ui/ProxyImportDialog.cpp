#include "ProxyImportDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextCursor>

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
    connect(m_proxyTextEdit, &QTextEdit::textChanged, this, &ProxyImportDialog::onProxyTextChanged);
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
    
    // User Agent checkboxes with real data
    QStringList userAgents = {
        "Windows 10 - Chrome 120 (1920x1080)",
        "Windows 10 - Chrome 121 (1366x768)",
        "Windows 11 - Chrome 120 (2560x1440)",
        "Windows 11 - Edge 120 (1920x1080)",
        "Windows 11 - Firefox 121 (1920x1080)",
        "macOS Sonoma - Safari 17 (1920x1080)",
        "macOS Sonoma - Chrome 120 (2560x1440)",
        "macOS Ventura - Safari 16 (1440x900)",
        "macOS Ventura - Chrome 119 (1920x1080)",
        "Linux Ubuntu - Firefox 121 (1920x1080)",
        "Linux Ubuntu - Chrome 120 (1366x768)",
        "Android 14 - Chrome Mobile (412x915)",
        "Android 13 - Chrome Mobile (360x800)",
        "iOS 17 - Safari Mobile (390x844)",
        "iOS 16 - Safari Mobile (375x667)"
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

QString ProxyImportDialog::parseProxyLine(const QString& line)
{
    QString trimmed = line.trimmed();
    if (trimmed.isEmpty()) return QString();
    
    // Format 1: host:port:username:password (standard)
    // Example: 103.47.53.144:8442:user:pass
    if (trimmed.count(':') == 3) {
        return trimmed;
    }
    
    // Format 2: host:port@username:password
    // Example: 103.47.53.144:8442@user:pass
    if (trimmed.contains('@')) {
        QStringList parts = trimmed.split('@');
        if (parts.size() == 2) {
            QString hostPort = parts[0];
            QString userPass = parts[1];
            return hostPort + ":" + userPass;
        }
    }
    
    // Format 3: username:password@host:port
    // Example: user:pass@103.47.53.144:8442
    QRegularExpression re1("^([^:]+):([^@]+)@([^:]+):(\\d+)$");
    QRegularExpressionMatch match1 = re1.match(trimmed);
    if (match1.hasMatch()) {
        QString username = match1.captured(1);
        QString password = match1.captured(2);
        QString host = match1.captured(3);
        QString port = match1.captured(4);
        return QString("%1:%2:%3:%4").arg(host).arg(port).arg(username).arg(password);
    }
    
    // Format 4: host:port (no auth)
    // Example: 103.47.53.144:8442
    if (trimmed.count(':') == 1) {
        QStringList parts = trimmed.split(':');
        if (parts.size() == 2) {
            bool ok;
            parts[1].toInt(&ok);
            if (ok) {
                return trimmed + "::"; // Add empty username and password
            }
        }
    }
    
    // Format 5: http://host:port or http://username:password@host:port
    // Example: http://103.47.53.144:8442 or http://user:pass@103.47.53.144:8442
    if (trimmed.startsWith("http://") || trimmed.startsWith("https://") || 
        trimmed.startsWith("socks4://") || trimmed.startsWith("socks5://")) {
        
        QString cleaned = trimmed;
        cleaned.remove(QRegularExpression("^(https?|socks[45])://"));
        
        // Check if has auth
        if (cleaned.contains('@')) {
            QStringList parts = cleaned.split('@');
            if (parts.size() == 2) {
                QString userPass = parts[0];
                QString hostPort = parts[1];
                return hostPort + ":" + userPass;
            }
        } else {
            // No auth
            return cleaned + "::";
        }
    }
    
    // Format 6: IP:PORT|USERNAME:PASSWORD (pipe separator)
    // Example: 103.47.53.144:8442|user:pass
    if (trimmed.contains('|')) {
        QStringList parts = trimmed.split('|');
        if (parts.size() == 2) {
            return parts[0] + ":" + parts[1];
        }
    }
    
    // Format 7: IP PORT USERNAME PASSWORD (space separated)
    // Example: 103.47.53.144 8442 user pass
    QStringList spaceParts = trimmed.split(QRegularExpression("\\s+"));
    if (spaceParts.size() == 4) {
        bool ok;
        spaceParts[1].toInt(&ok);
        if (ok) {
            return QString("%1:%2:%3:%4")
                .arg(spaceParts[0])
                .arg(spaceParts[1])
                .arg(spaceParts[2])
                .arg(spaceParts[3]);
        }
    } else if (spaceParts.size() == 2) {
        // IP PORT (no auth)
        bool ok;
        spaceParts[1].toInt(&ok);
        if (ok) {
            return QString("%1:%2::").arg(spaceParts[0]).arg(spaceParts[1]);
        }
    }
    
    // Format 8: IP,PORT,USERNAME,PASSWORD (comma separated)
    // Example: 103.47.53.144,8442,user,pass
    if (trimmed.contains(',')) {
        QStringList commaParts = trimmed.split(',');
        if (commaParts.size() == 4) {
            return QString("%1:%2:%3:%4")
                .arg(commaParts[0].trimmed())
                .arg(commaParts[1].trimmed())
                .arg(commaParts[2].trimmed())
                .arg(commaParts[3].trimmed());
        } else if (commaParts.size() == 2) {
            return QString("%1:%2::").arg(commaParts[0].trimmed()).arg(commaParts[1].trimmed());
        }
    }
    
    // Format 9: IP;PORT;USERNAME;PASSWORD (semicolon separated)
    // Example: 103.47.53.144;8442;user;pass
    if (trimmed.contains(';')) {
        QStringList semiParts = trimmed.split(';');
        if (semiParts.size() == 4) {
            return QString("%1:%2:%3:%4")
                .arg(semiParts[0].trimmed())
                .arg(semiParts[1].trimmed())
                .arg(semiParts[2].trimmed())
                .arg(semiParts[3].trimmed());
        } else if (semiParts.size() == 2) {
            return QString("%1:%2::").arg(semiParts[0].trimmed()).arg(semiParts[1].trimmed());
        }
    }
    
    // If nothing matched, return original
    return trimmed;
}

void ProxyImportDialog::onProxyTextChanged()
{
    // Auto-parse proxy format when text changes
    QString text = m_proxyTextEdit->toPlainText();
    
    if (text.isEmpty()) return;
    
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    QString formattedText;
    bool hasChanges = false;
    
    for (const QString& line : lines) {
        QString parsed = parseProxyLine(line);
        if (!parsed.isEmpty()) {
            formattedText += parsed + "\n";
            if (parsed != line.trimmed()) {
                hasChanges = true;
            }
        }
    }
    
    // Only update if formatting changed
    if (hasChanges && !formattedText.isEmpty()) {
        // Block signals to prevent recursion
        m_proxyTextEdit->blockSignals(true);
        
        // Save cursor position
        QTextCursor cursor = m_proxyTextEdit->textCursor();
        int position = cursor.position();
        
        m_proxyTextEdit->setPlainText(formattedText.trimmed());
        
        // Restore cursor position (approximately)
        cursor.setPosition(qMin(position, m_proxyTextEdit->toPlainText().length()));
        m_proxyTextEdit->setTextCursor(cursor);
        
        m_proxyTextEdit->blockSignals(false);
    }
}
