#include "MainWindow.h"
#include "Dashboard.h"
#include "ProfileEditor.h"
#include "VPNImportDialog.h"
#include "core/Application.h"
#include "core/BrowserWindowManager.h"
#include "profiles/ProfileManager.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QIcon>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(nullptr)
    , m_dashboard(nullptr)
    , m_profileEditor(nullptr)
{
    setupUi();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    
    setWindowTitle("Antidetect Browser");
    resize(1200, 800);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);
    
    // Create dashboard
    m_dashboard = new Dashboard(this);
    m_stackedWidget->addWidget(m_dashboard);
    
    // Create profile editor
    m_profileEditor = new ProfileEditor(this);
    m_stackedWidget->addWidget(m_profileEditor);
    
    // Connect signals
    connect(m_dashboard, &Dashboard::createProfileRequested, 
            this, [this]() { showProfileEditor(); });
    connect(m_dashboard, &Dashboard::editProfileRequested,
            this, &MainWindow::showProfileEditor);
    connect(m_dashboard, &Dashboard::launchProfileRequested,
            this, [this](const QString& profileId) {
                ProfileManager* pm = Application::instance().profileManager();
                Profile profile = pm->getProfile(profileId);
                
                if (profile.isValid()) {
                    BrowserWindowManager* bwm = Application::instance().browserWindowManager();
                    bwm->launchProfile(profile);
                    statusBar()->showMessage(QString("Launched browser for profile: %1").arg(profile.name()), 3000);
                } else {
                    QMessageBox::warning(this, "Error", "Failed to load profile");
                }
            });
    connect(m_profileEditor, &ProfileEditor::profileSaved,
            this, &MainWindow::onProfileCreated);
    connect(m_profileEditor, &ProfileEditor::cancelled,
            this, &MainWindow::showDashboard);
    
    // Show dashboard by default
    showDashboard();
}

void MainWindow::setupMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    // File menu
    QMenu* fileMenu = menuBar->addMenu("&File");
    
    QAction* newProfileAction = fileMenu->addAction("&New Profile");
    newProfileAction->setShortcut(QKeySequence::New);
    connect(newProfileAction, &QAction::triggered, this, [this]() { showProfileEditor(); });
    
    fileMenu->addSeparator();
    
    QAction* importAction = fileMenu->addAction("&Import Profiles...");
    QAction* exportAction = fileMenu->addAction("&Export Profiles...");
    
    fileMenu->addSeparator();
    
    QAction* importVPNAction = fileMenu->addAction("Import &VPN Configuration...");
    importVPNAction->setIcon(QIcon::fromTheme("network-vpn"));
    connect(importVPNAction, &QAction::triggered, this, [this]() {
        VPNImportDialog dialog(this);
        dialog.exec();
        statusBar()->showMessage("VPN configurations updated", 3000);
    });
    
    fileMenu->addSeparator();
    
    QAction* exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    // View menu
    QMenu* viewMenu = menuBar->addMenu("&View");
    viewMenu->addAction("&Dashboard", this, &MainWindow::showDashboard);
    
    // Help menu
    QMenu* helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&About");
    helpMenu->addAction("&Documentation");
}

void MainWindow::setupToolBar()
{
    QToolBar* toolBar = addToolBar("Main Toolbar");
    toolBar->setMovable(false);
    
    QAction* dashboardAction = toolBar->addAction("Dashboard");
    connect(dashboardAction, &QAction::triggered, this, &MainWindow::showDashboard);
    
    toolBar->addSeparator();
    
    QAction* newProfileAction = toolBar->addAction("New Profile");
    connect(newProfileAction, &QAction::triggered, this, [this]() { showProfileEditor(); });
    
    toolBar->addSeparator();
    
    QAction* importVPNAction = toolBar->addAction("Import VPN");
    importVPNAction->setToolTip("Import VPN Configuration");
    connect(importVPNAction, &QAction::triggered, this, [this]() {
        VPNImportDialog dialog(this);
        dialog.exec();
        statusBar()->showMessage("VPN configurations updated", 3000);
    });
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready");
}

void MainWindow::showDashboard()
{
    m_stackedWidget->setCurrentWidget(m_dashboard);
    m_dashboard->refresh();
}

void MainWindow::showProfileEditor(const QString& profileId)
{
    if (!profileId.isEmpty()) {
        m_profileEditor->loadProfile(profileId);
    } else {
        m_profileEditor->clear();
    }
    
    m_stackedWidget->setCurrentWidget(m_profileEditor);
}

void MainWindow::onProfileCreated()
{
    statusBar()->showMessage("Profile saved successfully", 3000);
    showDashboard();
}

void MainWindow::onProfileUpdated()
{
    statusBar()->showMessage("Profile updated successfully", 3000);
    showDashboard();
}
