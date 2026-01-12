#include "PasswordHash.h"
#include <QCryptographicHash>
#include <QRandomGenerator>

QString PasswordHash::hash(const QString& password)
{
    // Generate random salt
    QByteArray salt = generateSalt();
    
    // Hash password with salt (simple PBKDF2-like)
    QByteArray result = password.toUtf8() + salt;
    
    for (int i = 0; i < 10000; ++i) {
        result = QCryptographicHash::hash(result, QCryptographicHash::Sha256);
    }
    
    // Return salt + hash in base64
    return QString::fromLatin1((salt + result).toBase64());
}

bool PasswordHash::verify(const QString& password, const QString& hash)
{
    QByteArray data = QByteArray::fromBase64(hash.toLatin1());
    
    if (data.size() < 16) {
        return false;
    }
    
    // Extract salt
    QByteArray salt = data.left(16);
    QByteArray storedHash = data.mid(16);
    
    // Hash password with extracted salt
    QByteArray result = password.toUtf8() + salt;
    
    for (int i = 0; i < 10000; ++i) {
        result = QCryptographicHash::hash(result, QCryptographicHash::Sha256);
    }
    
    return result == storedHash;
}

QByteArray PasswordHash::deriveKey(const QString& password, const QByteArray& salt, int keyLength)
{
    QByteArray result = password.toUtf8() + salt;
    
    // Simple key derivation (PBKDF2-like)
    for (int i = 0; i < 10000; ++i) {
        result = QCryptographicHash::hash(result, QCryptographicHash::Sha256);
    }
    
    // Truncate or extend to desired length
    if (result.size() > keyLength) {
        return result.left(keyLength);
    } else if (result.size() < keyLength) {
        // Extend by repeating
        while (result.size() < keyLength) {
            result += QCryptographicHash::hash(result, QCryptographicHash::Sha256);
        }
        return result.left(keyLength);
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
