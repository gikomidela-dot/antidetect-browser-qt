#include "PasswordHash.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <QCryptographicHash>
#include <QDebug>

QString PasswordHash::hash(const QString& password)
{
    // Generate salt
    QByteArray salt = generateSalt();
    
    // Derive key using PBKDF2 (simpler alternative to Argon2)
    QByteArray key = deriveKey(password, salt, 32);
    
    // Combine salt and hash
    QByteArray result = salt + key;
    return QString::fromLatin1(result.toBase64());
}

bool PasswordHash::verify(const QString& password, const QString& hash)
{
    QByteArray data = QByteArray::fromBase64(hash.toLatin1());
    
    if (data.size() < 48) { // 16 bytes salt + 32 bytes key
        return false;
    }
    
    QByteArray salt = data.left(16);
    QByteArray storedKey = data.mid(16);
    
    QByteArray derivedKey = deriveKey(password, salt, 32);
    
    return derivedKey == storedKey;
}

QByteArray PasswordHash::deriveKey(const QString& password, const QByteArray& salt, int keyLength)
{
    QByteArray key(keyLength, 0);
    
    // Use PBKDF2 with SHA-256
    if (PKCS5_PBKDF2_HMAC(password.toUtf8().constData(),
                          password.toUtf8().size(),
                          reinterpret_cast<const unsigned char*>(salt.constData()),
                          salt.size(),
                          100000, // iterations
                          EVP_sha256(),
                          keyLength,
                          reinterpret_cast<unsigned char*>(key.data())) != 1) {
        qWarning() << "Failed to derive key";
        return QByteArray();
    }
    
    return key;
}

QByteArray PasswordHash::generateSalt(int length)
{
    QByteArray salt(length, 0);
    if (RAND_bytes(reinterpret_cast<unsigned char*>(salt.data()), length) != 1) {
        qWarning() << "Failed to generate salt";
        return QByteArray();
    }
    return salt;
}
