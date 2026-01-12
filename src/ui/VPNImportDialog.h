#ifndef VPNIMPORTDIALOG_H
#define VPNIMPORTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include "vpn/VPNManager.h"

class VPNImportDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit VPNImportDialog(QWidget *parent = nullptr);
    
    QList<VPNConfig> getImportedConfigs() const { return m_importedConfigs; }
    
private slots:
    void onImportFromFile();
    void onImportFromUrl();
    void onImportFromText();
    void onConfigImported(const QString& name);
    void onImportError(const QString& error);
    
private:
    void setupUi();
    void updateConfigList();
    
    QListWidget* m_configList;
    QLineEdit* m_urlEdit;
    QTextEdit* m_textEdit;
    QComboBox* m_typeCombo;
    QPushButton* m_fileButton;
    QPushButton* m_urlButton;
    QPushButton* m_textButton;
    QPushButton* m_closeButton;
    
    QList<VPNConfig> m_importedConfigs;
};

#endif // VPNIMPORTDIALOG_H
