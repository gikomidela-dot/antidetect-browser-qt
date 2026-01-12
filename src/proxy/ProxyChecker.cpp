#include "ProxyChecker.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QElapsedTimer>
#include <QUrl>

ProxyChecker::ProxyChecker(QObject* parent)
    : QObject(parent)
{
}

void ProxyChecker::checkProxy(const ProxyConfig& config)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    
    // Configure proxy
    QNetworkProxy proxy;
    if (config.type == ProxyConfig::HTTP || config.type == ProxyConfig::HTTPS) {
        proxy.setType(QNetworkProxy::HttpProxy);
    } else if (config.type == ProxyConfig::SOCKS5) {
        proxy.setType(QNetworkProxy::Socks5Proxy);
    }
    
    proxy.setHostName(config.host);
    proxy.setPort(config.port);
    proxy.setUser(config.username);
    proxy.setPassword(config.password);
    
    manager->setProxy(proxy);
    
    // Make request
    QNetworkRequest request(QUrl("http://httpbin.org/ip"));
    request.setTransferTimeout(10000); // 10 seconds timeout
    
    QNetworkReply* reply = manager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, &ProxyChecker::onFinished);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &ProxyChecker::onError);
}

void ProxyChecker::onFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    if (reply->error() == QNetworkReply::NoError) {
        emit proxyChecked(true, 0);
    } else {
        emit proxyChecked(false, 0);
    }
    
    reply->deleteLater();
}

void ProxyChecker::onError()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    emit checkError(reply->errorString());
    reply->deleteLater();
}
