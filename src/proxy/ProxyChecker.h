#ifndef PROXYCHECKER_H
#define PROXYCHECKER_H

#include <QObject>
#include "profiles/Profile.h"

class ProxyChecker : public QObject
{
    Q_OBJECT
    
public:
    explicit ProxyChecker(QObject* parent = nullptr);
    
    void checkProxy(const ProxyConfig& config);
    
signals:
    void proxyChecked(bool success, int latency);
    void checkError(const QString& error);
    
private slots:
    void onFinished();
    void onError();
};

#endif // PROXYCHECKER_H
