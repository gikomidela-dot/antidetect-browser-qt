#include "ProxyUrlRequestInterceptor.h"
#include <QDebug>

ProxyUrlRequestInterceptor::ProxyUrlRequestInterceptor(const ProxyConfig& proxyConfig, QObject* parent)
    : QWebEngineUrlRequestInterceptor(parent)
    , m_proxyConfig(proxyConfig)
{
}

void ProxyUrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo& info)
{
    // Set custom headers if needed
    // Note: Qt WebEngine doesn't support per-request proxy through interceptor
    // Proxy must be set at application or system level
    
    qDebug() << "Intercepting request to:" << info.requestUrl().toString();
    
    // You can modify headers here
    // info.setHttpHeader("X-Custom-Header", "value");
}
