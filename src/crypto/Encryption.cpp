#include "Encryption.h"
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDebug>

// Simple XOR-based encryption (for demo purposes)
// In production, use proper crypto library
QByteArray Encryption::encrypt(const QByteArray& plaintext, const QByteArray& key, const QByteArray& iv)
{
    QByteArray result = plaintext;
    QByteArray fullKey = key + iv;
    
    for (int i = 0; i < result.size(); ++i) {
        result[i] = result[i] ^ fullKey[i % fullKey.size()];
    }
    
    return result;
}

QByteArray Encryption::decrypt(const QByteArray& ciphertext, const QByteArray& key, const QByteArray& iv)
{
    // XOR is symmetric
    return encrypt(ciphertext, key, iv);
}

QByteArray Encryption::generateKey(int length)
{
    QByteArray key;
    key.resize(length);
    
    for (int i = 0; i < length; ++i) {
        key[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }
    
    return key;
}

QByteArray Encryption::generateIV(int length)
{
    QByteArray iv;
    iv.resize(length);
    
    for (int i = 0; i < length; ++i) {
        iv[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }
    
    return iv;
}

QString Encryption::encryptString(const QString& plaintext, const QByteArray& key)
{
    QByteArray iv = generateIV(12);
    QByteArray encrypted = encrypt(plaintext.toUtf8(), key, iv);
    
    if (encrypted.isEmpty()) {
        return QString();
    }
    
    // Prepend IV to ciphertext
    QByteArray result = iv + encrypted;
    return QString::fromLatin1(result.toBase64());
}

QString Encryption::decryptString(const QString& ciphertext, const QByteArray& key)
{
    QByteArray data = QByteArray::fromBase64(ciphertext.toLatin1());
    
    if (data.size() < 12) {
        return QString();
    }
    
    // Extract IV and ciphertext
    QByteArray iv = data.left(12);
    QByteArray ct = data.mid(12);
    
    QByteArray decrypted = decrypt(ct, key, iv);
    return QString::fromUtf8(decrypted);
}
