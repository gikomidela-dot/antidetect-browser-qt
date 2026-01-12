#ifndef PROXYURLREQUESTINTERCEPTOR_H
#define PROXYURLREQUESTINTERCEPTOR_H

#include <QWebEngineUrlRequestInterceptor>
#include "profiles/Profile.h"

class ProxyUrlRequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT
    
public:
    explicit ProxyUrlRequestInterceptor(const ProxyConfig& proxyConfig, QObject* parent = nullptr);
    
    void interceptRequest(QWebEngineUrlRequestInfo& info) override;
    
private:
    ProxyConfig m_proxyConfig;
};

#endif // PROXYURLREQUESTINTERCEPTOR_H
