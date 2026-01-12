#ifndef APIHANDLER_H
#define APIHANDLER_H

#include <QString>
#include <QByteArray>

class ApiHandler
{
public:
    ApiHandler();
    
    QByteArray handleRequest(const QString& method, const QString& path, const QByteArray& body);
    
private:
    QByteArray handleProfiles(const QString& method, const QString& path, const QByteArray& body);
    QByteArray handleProfile(const QString& method, const QString& id, const QByteArray& body);
    
    QByteArray jsonResponse(const QString& json);
    QByteArray errorResponse(const QString& message);
};

#endif // APIHANDLER_H
