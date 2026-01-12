#include "PasswordHash.h"
#include <QCryptographicHash>
#include <QRandomGenerator>

QByteArray PasswordHash::hash(const QString& password, const QByteArray& salt, int iterations)
{
    QByteArray result = password.toUtf8() + salt;
    
    // Simple iterative hashing (PBKDF2-like)
    for (int i = 0; i < iterations; ++i) {
        result = QCryptographicHash::hash(result, QCryptographicHash::Sha256);
    }
    
    return result;
}

QByteArray PasswordHash::generateSalt(int length)
{
    QByteArray salt;
    salt.resize(length);
    
    for (int i = 0; i < length; ++i) {
        salt[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }
    
    return salt;
}

bool PasswordHash::verify(const QString& password, const QByteArray& hash, const QByteArray& salt, int iterations)
{
    QByteArray computed = PasswordHash::hash(password, salt, iterations);
    return computed == hash;
}
