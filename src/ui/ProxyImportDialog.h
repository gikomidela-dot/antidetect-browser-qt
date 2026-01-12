#ifndef PROXYIMPORTDIALOG_H
#define PROXYIMPORTDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QCheckBox>
#include <QMap>

class ProxyImportDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ProxyImportDialog(QWidget *parent = nullptr);
    
    struct ImportResult {
        QStringList proxyList;
        QStringList selectedUserAgents;
        bool createMultipleProfiles;
    };
    
    ImportResult getResult() const { return m_result; }
    
private slots:
    void onImport();
    void onCancel();
    void onSelectAll();
    void onDeselectAll();
    void onProxyTextChanged();
    
private:
    void setupUi();
    QString parseProxyLine(const QString& line);
    
    QTextEdit* m_proxyTextEdit;
    QMap<QString, QCheckBox*> m_userAgentCheckboxes;
    ImportResult m_result;
};

#endif // PROXYIMPORTDIALOG_H
