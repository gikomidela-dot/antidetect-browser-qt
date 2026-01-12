#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

class Dashboard;
class ProfileEditor;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void showDashboard();
    void showProfileEditor(const QString& profileId = QString());
    void onProfileCreated();
    void onProfileUpdated();
    
private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    
    QStackedWidget* m_stackedWidget;
    Dashboard* m_dashboard;
    ProfileEditor* m_profileEditor;
};

#endif // MAINWINDOW_H
