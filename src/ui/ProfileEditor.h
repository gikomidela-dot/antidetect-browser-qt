#ifndef PROFILEEDITOR_H
#define PROFILEEDITOR_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include "profiles/Profile.h"

class ProfileEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit ProfileEditor(QWidget *parent = nullptr);
    
    void loadProfile(const QString& profileId);
    void clear();
    
signals:
    void profileSaved(const QString& profileId);
    void cancelled();
    
private slots:
    void onSave();
    void onCancel();
    void onTemplateSelected(int index);
    void onParseProxy();
    
private:
    void setupUi();
    Profile getProfileFromForm();
    void setProfileToForm(const Profile& profile);
    
    QString m_currentProfileId;
    
    // Basic info
    QLineEdit* m_nameEdit;
    QTextEdit* m_notesEdit;
    QLineEdit* m_tagsEdit;
    
    // Fingerprint
    QComboBox* m_templateCombo;
    QLineEdit* m_userAgentEdit;
    QLineEdit* m_platformEdit;
    QSpinBox* m_hardwareConcurrencySpin;
    QSpinBox* m_deviceMemorySpin;
    QSpinBox* m_screenWidthSpin;
    QSpinBox* m_screenHeightSpin;
    QLineEdit* m_timezoneEdit;
    QLineEdit* m_localeEdit;
    QLineEdit* m_webglVendorEdit;
    QLineEdit* m_webglRendererEdit;
    QCheckBox* m_canvasNoiseCheck;
    QCheckBox* m_audioNoiseCheck;
    QCheckBox* m_webrtcProtectionCheck;
    
    // Proxy
    QComboBox* m_proxyTypeCombo;
    QLineEdit* m_proxyHostEdit;
    QSpinBox* m_proxyPortSpin;
    QLineEdit* m_proxyUsernameEdit;
    QLineEdit* m_proxyPasswordEdit;
    
    // VPN
    QComboBox* m_vpnCombo;
};

#endif // PROFILEEDITOR_H
