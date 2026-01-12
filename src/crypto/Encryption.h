#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QString>
#include <QByteArray>

class Encryption
{
public:
    // AES-256-GCM encryption
    static QByteArray encrypt(const QByteArray& plaintext, const QByteArray& key, const QByteArray& iv);
    static QByteArray decrypt(const QByteArray& ciphertext, const QByteArray& key, const QByteArray& iv);
    
    // Generate random key and IV
    static QByteArray generateKey(int length = 32); // 256 bits
    static QByteArray generateIV(int length = 12);  // 96 bits for GCM
    
    // Convenience methods for QString
    static QString encryptString(const QString& plaintext, const QByteArray& key);
    static QString decryptString(const QString& ciphertext, const QByteArray& key);
    
private:
    Encryption() = delete;
};

#endif // ENCRYPTION_H
