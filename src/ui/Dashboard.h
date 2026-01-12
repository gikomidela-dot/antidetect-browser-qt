#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>

class Dashboard : public QWidget
{
    Q_OBJECT
    
public:
    explicit Dashboard(QWidget *parent = nullptr);
    
    void refresh();
    
signals:
    void createProfileRequested();
    void editProfileRequested(const QString& profileId);
    void deleteProfileRequested(const QString& profileId);
    void launchProfileRequested(const QString& profileId);
    
private slots:
    void onCreateProfile();
    void onEditProfile();
    void onDeleteProfile();
    void onLaunchProfile();
    void onSearchTextChanged(const QString& text);
    void onExportCookies();
    void onImportCookies();
    void onMassImportProxy();
    
private:
    void setupUi();
    void loadProfiles();
    void filterProfiles(const QString& query);
    
    QTableWidget* m_profileTable;
    QPushButton* m_createButton;
    QPushButton* m_editButton;
    QPushButton* m_deleteButton;
    QPushButton* m_launchButton;
    QPushButton* m_exportCookiesButton;
    QPushButton* m_importCookiesButton;
    QLineEdit* m_searchEdit;
};

#endif // DASHBOARD_H
