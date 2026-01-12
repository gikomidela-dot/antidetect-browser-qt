#ifndef PASSWORDHASH_H
#define PASSWORDHASH_H

#include <QString>
#include <QByteArray>

class PasswordHash
{
public:
    // Argon2id hashing
    static QString hash(const QString& password);
    static bool verify(const QString& password, const QString& hash);
    
    // Derive key from password for encryption
    static QByteArray deriveKey(const QString& password, const QByteArray& salt, int keyLength = 32);
    static QByteArray generateSalt(int length = 16);
    
private:
    PasswordHash() = delete;
};

#endif // PASSWORDHASH_H
