#ifndef RESTAPISERVER_H
#define RESTAPISERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class RestApiServer : public QObject
{
    Q_OBJECT
    
public:
    explicit RestApiServer(QObject* parent = nullptr);
    ~RestApiServer();
    
    bool start(int port = 8080);
    void stop();
    
    bool isRunning() const { return m_server && m_server->isListening(); }
    int port() const;
    
signals:
    void started(int port);
    void stopped();
    void requestReceived(const QString& method, const QString& path);
    
private slots:
    void onNewConnection();
    void onReadyRead();
    
private:
    void handleRequest(QTcpSocket* socket, const QString& method, 
                      const QString& path, const QByteArray& body);
    void sendResponse(QTcpSocket* socket, int statusCode, 
                     const QString& contentType, const QByteArray& body);
    
    QTcpServer* m_server;
};

#endif // RESTAPISERVER_H
