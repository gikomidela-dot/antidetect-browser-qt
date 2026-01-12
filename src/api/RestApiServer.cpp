#include "RestApiServer.h"
#include "ApiHandler.h"
#include <QDebug>

RestApiServer::RestApiServer(QObject* parent)
    : QObject(parent)
    , m_server(nullptr)
{
}

RestApiServer::~RestApiServer()
{
    stop();
}

bool RestApiServer::start(int port)
{
    if (m_server && m_server->isListening()) {
        qWarning() << "Server already running";
        return false;
    }
    
    m_server = new QTcpServer(this);
    
    if (!m_server->listen(QHostAddress::LocalHost, port)) {
        qCritical() << "Failed to start server:" << m_server->errorString();
        delete m_server;
        m_server = nullptr;
        return false;
    }
    
    connect(m_server, &QTcpServer::newConnection, this, &RestApiServer::onNewConnection);
    
    qInfo() << "API Server started on port" << port;
    emit started(port);
    
    return true;
}

void RestApiServer::stop()
{
    if (m_server) {
        m_server->close();
        delete m_server;
        m_server = nullptr;
        emit stopped();
        qInfo() << "API Server stopped";
    }
}

int RestApiServer::port() const
{
    return m_server ? m_server->serverPort() : 0;
}

void RestApiServer::onNewConnection()
{
    QTcpSocket* socket = m_server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &RestApiServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void RestApiServer::onReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    QByteArray request = socket->readAll();
    QList<QByteArray> lines = request.split('\n');
    
    if (lines.isEmpty()) return;
    
    // Parse request line
    QList<QByteArray> requestLine = lines[0].split(' ');
    if (requestLine.size() < 3) return;
    
    QString method = QString::fromUtf8(requestLine[0]);
    QString path = QString::fromUtf8(requestLine[1]);
    
    // Find body (after empty line)
    QByteArray body;
    bool foundEmptyLine = false;
    for (const QByteArray& line : lines) {
        if (foundEmptyLine) {
            body += line;
        } else if (line.trimmed().isEmpty()) {
            foundEmptyLine = true;
        }
    }
    
    emit requestReceived(method, path);
    handleRequest(socket, method, path, body);
}

void RestApiServer::handleRequest(QTcpSocket* socket, const QString& method, 
                                  const QString& path, const QByteArray& body)
{
    ApiHandler handler;
    QByteArray response = handler.handleRequest(method, path, body);
    
    sendResponse(socket, 200, "application/json", response);
}

void RestApiServer::sendResponse(QTcpSocket* socket, int statusCode, 
                                 const QString& contentType, const QByteArray& body)
{
    QString statusText = (statusCode == 200) ? "OK" : "Error";
    
    QString response = QString("HTTP/1.1 %1 %2\r\n").arg(statusCode).arg(statusText);
    response += QString("Content-Type: %1\r\n").arg(contentType);
    response += QString("Content-Length: %1\r\n").arg(body.size());
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    
    socket->write(response.toUtf8());
    socket->write(body);
    socket->flush();
    socket->disconnectFromHost();
}
