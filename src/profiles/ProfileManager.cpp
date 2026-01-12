#include "ProfileManager.h"
#include "ProfileStorage.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTextStream>
#include <QRandomGenerator>
#include <QDebug>

ProfileManager::ProfileManager(const QString& dataPath, QObject* parent)
    : QObject(parent)
    , m_storage(std::make_unique<ProfileStorage>(dataPath))
{
    initializeTemplates();
}

ProfileManager::~ProfileManager() = default;

bool ProfileManager::createProfile(const Profile& profile)
{
    if (!profile.isValid()) {
        qWarning() << "Invalid profile";
        return false;
    }
    
    if (m_storage->save(profile)) {
        emit profileCreated(profile.id());
        return true;
    }
    return false;
}

Profile ProfileManager::getProfile(const QString& id) const
{
    return m_storage->load(id);
}

QList<Profile> ProfileManager::getAllProfiles() const
{
    return m_storage->loadAll();
}

bool ProfileManager::updateProfile(const Profile& profile)
{
    if (!profile.isValid()) {
        return false;
    }
    
    Profile updated = profile;
    updated.setUpdatedAt(QDateTime::currentDateTime());
    
    if (m_storage->save(updated)) {
        emit profileUpdated(profile.id());
        return true;
    }
    return false;
}

bool ProfileManager::deleteProfile(const QString& id)
{
    if (m_storage->remove(id)) {
        emit profileDeleted(id);
        return true;
    }
    return false;
}

bool ProfileManager::deleteProfiles(const QStringList& ids)
{
    int deleted = 0;
    for (const QString& id : ids) {
        if (deleteProfile(id)) {
            deleted++;
        }
    }
    return deleted > 0;
}

QList<Profile> ProfileManager::importProfiles(const QString& filePath, const QString& format)
{
    QList<Profile> imported;
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << filePath;
        return imported;
    }
    
    if (format.toLower() == "json") {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray array = doc.array();
        
        for (const auto& value : array) {
            Profile profile = Profile::fromJson(value.toObject());
            if (createProfile(profile)) {
                imported.append(profile);
            }
        }
    } else if (format.toLower() == "csv") {
        QTextStream in(&file);
        in.readLine(); // Skip header
        
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(',');
            
            if (fields.size() >= 2) {
                Profile profile;
                profile.setName(fields[0]);
                profile.setNotes(fields.size() > 1 ? fields[1] : "");
                
                if (createProfile(profile)) {
                    imported.append(profile);
                }
            }
        }
    }
    
    file.close();
    
    if (!imported.isEmpty()) {
        emit profilesImported(imported.size());
    }
    
    return imported;
}

bool ProfileManager::exportProfiles(const QStringList& ids, const QString& filePath, const QString& format)
{
    QList<Profile> profiles;
    for (const QString& id : ids) {
        Profile profile = getProfile(id);
        if (profile.isValid()) {
            profiles.append(profile);
        }
    }
    
    if (profiles.isEmpty()) {
        return false;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot create file:" << filePath;
        return false;
    }
    
    if (format.toLower() == "json") {
        QJsonArray array;
        for (const Profile& profile : profiles) {
            array.append(profile.toJson());
        }
        
        QJsonDocument doc(array);
        file.write(doc.toJson(QJsonDocument::Indented));
    } else if (format.toLower() == "csv") {
        QTextStream out(&file);
        out << "Name,Notes,Tags,Created,Updated\n";
        
        for (const Profile& profile : profiles) {
            out << profile.name() << ","
                << profile.notes() << ","
                << profile.tags().join(";") << ","
                << profile.createdAt().toString(Qt::ISODate) << ","
                << profile.updatedAt().toString(Qt::ISODate) << "\n";
        }
    }
    
    file.close();
    emit profilesExported(profiles.size());
    return true;
}

QList<Profile> ProfileManager::searchProfiles(const QString& query) const
{
    QList<Profile> results;
    QList<Profile> all = getAllProfiles();
    
    QString lowerQuery = query.toLower();
    
    for (const Profile& profile : all) {
        if (profile.name().toLower().contains(lowerQuery) ||
            profile.notes().toLower().contains(lowerQuery)) {
            results.append(profile);
        }
    }
    
    return results;
}

QList<Profile> ProfileManager::filterByTags(const QStringList& tags) const
{
    QList<Profile> results;
    QList<Profile> all = getAllProfiles();
    
    for (const Profile& profile : all) {
        for (const QString& tag : tags) {
            if (profile.tags().contains(tag)) {
                results.append(profile);
                break;
            }
        }
    }
    
    return results;
}

Profile ProfileManager::createFromTemplate(const QString& templateName)
{
    if (!m_templates.contains(templateName)) {
        return Profile();
    }
    
    Profile base = m_templates[templateName];
    return generateRandomFingerprint(base);
}

QStringList ProfileManager::getTemplateNames() const
{
    return m_templates.keys();
}

void ProfileManager::initializeTemplates()
{
    // Windows Chrome template
    Profile winChrome;
    winChrome.setName("Windows Chrome");
    FingerprintConfig fp;
    fp.userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36";
    fp.platform = "Win32";
    fp.vendor = "Google Inc.";
    fp.renderer = "Google Inc. (NVIDIA)";
    fp.hardwareConcurrency = 8;
    fp.deviceMemory = 8;
    fp.screenWidth = 1920;
    fp.screenHeight = 1080;
    fp.colorDepth = 24;
    fp.timezone = "America/New_York";
    fp.locale = "en-US";
    fp.webglVendor = "Google Inc. (NVIDIA)";
    fp.webglRenderer = "ANGLE (NVIDIA, NVIDIA GeForce RTX 3060 Direct3D11 vs_5_0 ps_5_0)";
    fp.canvasNoise = true;
    fp.audioNoise = true;
    fp.webrtcProtection = true;
    fp.tlsProfile = "Chrome 120 Windows";
    winChrome.setFingerprint(fp);
    m_templates["Windows Chrome"] = winChrome;
    
    // macOS Safari template
    Profile macSafari;
    macSafari.setName("macOS Safari");
    FingerprintConfig fp2;
    fp2.userAgent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Safari/605.1.15";
    fp2.platform = "MacIntel";
    fp2.vendor = "Apple Computer, Inc.";
    fp2.renderer = "Apple Inc.";
    fp2.hardwareConcurrency = 8;
    fp2.deviceMemory = 8;
    fp2.screenWidth = 1920;
    fp2.screenHeight = 1080;
    fp2.colorDepth = 24;
    fp2.timezone = "America/Los_Angeles";
    fp2.locale = "en-US";
    fp2.webglVendor = "Apple Inc.";
    fp2.webglRenderer = "Apple M1";
    fp2.canvasNoise = true;
    fp2.audioNoise = true;
    fp2.webrtcProtection = true;
    fp2.tlsProfile = "Safari 17 macOS";
    macSafari.setFingerprint(fp2);
    m_templates["macOS Safari"] = macSafari;
    
    // Linux Firefox template
    Profile linuxFirefox;
    linuxFirefox.setName("Linux Firefox");
    FingerprintConfig fp3;
    fp3.userAgent = "Mozilla/5.0 (X11; Linux x86_64; rv:120.0) Gecko/20100101 Firefox/120.0";
    fp3.platform = "Linux x86_64";
    fp3.vendor = "";
    fp3.renderer = "Mesa";
    fp3.hardwareConcurrency = 4;
    fp3.deviceMemory = 8;
    fp3.screenWidth = 1920;
    fp3.screenHeight = 1080;
    fp3.colorDepth = 24;
    fp3.timezone = "Europe/London";
    fp3.locale = "en-GB";
    fp3.webglVendor = "Mesa";
    fp3.webglRenderer = "Mesa Intel(R) UHD Graphics 620 (KBL GT2)";
    fp3.canvasNoise = true;
    fp3.audioNoise = true;
    fp3.webrtcProtection = true;
    fp3.tlsProfile = "Firefox 121 Windows";
    linuxFirefox.setFingerprint(fp3);
    m_templates["Linux Firefox"] = linuxFirefox;
    
    // Android Chrome template
    Profile androidChrome;
    androidChrome.setName("Android Chrome");
    FingerprintConfig fp4;
    fp4.userAgent = "Mozilla/5.0 (Linux; Android 13; SM-G998B) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Mobile Safari/537.36";
    fp4.platform = "Linux armv8l";
    fp4.vendor = "Google Inc.";
    fp4.renderer = "Qualcomm";
    fp4.hardwareConcurrency = 8;
    fp4.deviceMemory = 8;
    fp4.screenWidth = 1080;
    fp4.screenHeight = 2400;
    fp4.colorDepth = 24;
    fp4.timezone = "America/New_York";
    fp4.locale = "en-US";
    fp4.webglVendor = "Qualcomm";
    fp4.webglRenderer = "Adreno (TM) 660";
    fp4.canvasNoise = true;
    fp4.audioNoise = true;
    fp4.webrtcProtection = true;
    fp4.tlsProfile = "Chrome 120 Windows";
    androidChrome.setFingerprint(fp4);
    m_templates["Android Chrome"] = androidChrome;
    
    // iOS Safari template
    Profile iosSafari;
    iosSafari.setName("iOS Safari");
    FingerprintConfig fp5;
    fp5.userAgent = "Mozilla/5.0 (iPhone; CPU iPhone OS 17_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Mobile/15E148 Safari/604.1";
    fp5.platform = "iPhone";
    fp5.vendor = "Apple Computer, Inc.";
    fp5.renderer = "Apple Inc.";
    fp5.hardwareConcurrency = 6;
    fp5.deviceMemory = 6;
    fp5.screenWidth = 1179;
    fp5.screenHeight = 2556;
    fp5.colorDepth = 24;
    fp5.timezone = "America/Los_Angeles";
    fp5.locale = "en-US";
    fp5.webglVendor = "Apple Inc.";
    fp5.webglRenderer = "Apple A17 Pro GPU";
    fp5.canvasNoise = true;
    fp5.audioNoise = true;
    fp5.webrtcProtection = true;
    fp5.tlsProfile = "Safari 17 macOS";
    iosSafari.setFingerprint(fp5);
    m_templates["iOS Safari"] = iosSafari;
}

Profile ProfileManager::generateRandomFingerprint(const Profile& base)
{
    Profile profile = base;
    FingerprintConfig fp = base.fingerprint();
    
    // Randomize some parameters
    QRandomGenerator* rng = QRandomGenerator::global();
    
    fp.hardwareConcurrency = rng->bounded(4, 17); // 4-16 cores
    fp.deviceMemory = (1 << rng->bounded(2, 5)); // 4, 8, 16 GB
    
    // Common screen resolutions
    QList<QPair<int, int>> resolutions = {
        {1920, 1080}, {1366, 768}, {1440, 900}, {1536, 864},
        {2560, 1440}, {3840, 2160}
    };
    auto res = resolutions[rng->bounded(resolutions.size())];
    fp.screenWidth = res.first;
    fp.screenHeight = res.second;
    
    profile.setFingerprint(fp);
    return profile;
}
