#include "Encryption.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <QDebug>

QByteArray Encryption::encrypt(const QByteArray& plaintext, const QByteArray& key, const QByteArray& iv)
{
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        qWarning() << "Failed to create cipher context";
        return QByteArray();
    }
    
    // Initialize encryption
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                           reinterpret_cast<const unsigned char*>(key.data()),
                           reinterpret_cast<const unsigned char*>(iv.data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    
    // Allocate output buffer
    QByteArray ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_gcm()), 0);
    int len = 0;
    int ciphertext_len = 0;
    
    // Encrypt
    if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(ciphertext.data()), &len,
                          reinterpret_cast<const unsigned char*>(plaintext.data()),
                          plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    ciphertext_len = len;
    
    // Finalize
    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(ciphertext.data()) + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    ciphertext_len += len;
    
    // Get tag
    QByteArray tag(16, 0);
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16,
                            reinterpret_cast<unsigned char*>(tag.data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    
    EVP_CIPHER_CTX_free(ctx);
    
    ciphertext.resize(ciphertext_len);
    return ciphertext + tag;
}

QByteArray Encryption::decrypt(const QByteArray& ciphertext, const QByteArray& key, const QByteArray& iv)
{
    if (ciphertext.size() < 16) {
        return QByteArray();
    }
    
    // Split ciphertext and tag
    int ciphertext_len = ciphertext.size() - 16;
    QByteArray ct = ciphertext.left(ciphertext_len);
    QByteArray tag = ciphertext.right(16);
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return QByteArray();
    }
    
    // Initialize decryption
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                           reinterpret_cast<const unsigned char*>(key.data()),
                           reinterpret_cast<const unsigned char*>(iv.data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    
    // Allocate output buffer
    QByteArray plaintext(ct.size(), 0);
    int len = 0;
    int plaintext_len = 0;
    
    // Decrypt
    if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(plaintext.data()), &len,
                          reinterpret_cast<const unsigned char*>(ct.data()),
                          ct.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    plaintext_len = len;
    
    // Set tag
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16,
                            reinterpret_cast<unsigned char*>(tag.data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    
    // Finalize
    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(plaintext.data()) + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    plaintext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    
    plaintext.resize(plaintext_len);
    return plaintext;
}

QByteArray Encryption::generateKey(int length)
{
    QByteArray key(length, 0);
    if (RAND_bytes(reinterpret_cast<unsigned char*>(key.data()), length) != 1) {
        qWarning() << "Failed to generate random key";
        return QByteArray();
    }
    return key;
}

QByteArray Encryption::generateIV(int length)
{
    QByteArray iv(length, 0);
    if (RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), length) != 1) {
        qWarning() << "Failed to generate random IV";
        return QByteArray();
    }
    return iv;
}

QString Encryption::encryptString(const QString& plaintext, const QByteArray& key)
{
    QByteArray iv = generateIV();
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
