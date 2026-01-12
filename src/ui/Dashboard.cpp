#include "Dashboard.h"
#include "core/Application.h"
#include "core/BrowserWindowManager.h"
#include "core/CookieManager.h"
#include "profiles/ProfileManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>

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
    
    m_editButton = new QPushButton("Edit", this);
    connect(m_editButton, &QPushButton::clicked, this, &Dashboard::onEditProfile);
    buttonLayout->addWidget(m_editButton);
    
    m_deleteButton = new QPushButton("Delete", this);
    connect(m_deleteButton, &QPushButton::clicked, this, &Dashboard::onDeleteProfile);
    buttonLayout->addWidget(m_deleteButton);
    
    m_launchButton = new QPushButton("Launch", this);
    connect(m_launchButton, &QPushButton::clicked, this, &Dashboard::onLaunchProfile);
    buttonLayout->addWidget(m_launchButton);
    
    buttonLayout->addSeparator();
    
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
    
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Cookies",
        profileName + "_cookies.json",
        "JSON Files (*.json);;Netscape Format (*.txt);;All Files (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QString format = fileName.endsWith(".txt") ? "netscape" : "json";
    
    CookieManager* cm = Application::instance().cookieManager();
    if (cm->exportCookies(profileId, fileName, format)) {
        QMessageBox::information(this, "Success", "Cookies exported successfully!");
    } else {
        QMessageBox::warning(this, "Error", "Failed to export cookies.");
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
