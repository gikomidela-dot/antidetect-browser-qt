#include <QApplication>
#include <QStyleFactory>
#include "core/Application.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    // Set Chromium flags BEFORE QApplication
    // This ensures timezone hook is loaded in Chromium renderer processes
    QString hookLib = QString("%1/../src/timezone_hook/libtimezone_hook.so").arg(QCoreApplication::applicationDirPath());
    qputenv("LD_PRELOAD", hookLib.toUtf8());
    
    QApplication app(argc, argv);
    
    // Set application info
    QApplication::setApplicationName("Antidetect Browser");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("AntidetectBrowser");
    
    // Set dark theme
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(darkPalette);
    
    // Initialize core application
    Application::instance().initialize();
    
    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
