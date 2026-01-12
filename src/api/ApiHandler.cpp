#include "ApiHandler.h"
#include "core/Application.h"
#include "profiles/ProfileManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ApiHandler::ApiHandler()
{
}

QByteArray ApiHandler::handleRequest(const QString& method, const QString& path, const QByteArray& body)
{
    if (path.startsWith("/api/profiles")) {
        if (path == "/api/profiles") {
            return handleProfiles(method, path, body);
        } else {
            // Extract profile ID
            QString id = path.mid(14); // Remove "/api/profiles/"
            return handleProfile(method, id, body);
        }
    }
    
    return errorResponse("Not found");
}

QByteArray ApiHandler::handleProfiles(const QString& method, const QString& path, const QByteArray& body)
{
    ProfileManager* pm = Application::instance().profileManager();
    
    if (method == "GET") {
        // Get all profiles
        QList<Profile> profiles = pm->getAllProfiles();
        QJsonArray array;
        
        for (const Profile& profile : profiles) {
            array.append(profile.toJson());
        }
        
        QJsonDocument doc(array);
        return jsonResponse(QString::fromUtf8(doc.toJson()));
    }
    else if (method == "POST") {
        // Create new profile
        QJsonDocument doc = QJsonDocument::fromJson(body);
        Profile profile = Profile::fromJson(doc.object());
        
        if (pm->createProfile(profile)) {
            QJsonDocument responseDoc(profile.toJson());
            return jsonResponse(QString::fromUtf8(responseDoc.toJson()));
        }
        
        return errorResponse("Failed to create profile");
    }
    
    return errorResponse("Method not allowed");
}

QByteArray ApiHandler::handleProfile(const QString& method, const QString& id, const QByteArray& body)
{
    ProfileManager* pm = Application::instance().profileManager();
    
    if (method == "GET") {
        // Get specific profile
        Profile profile = pm->getProfile(id);
        
        if (profile.isValid()) {
            QJsonDocument doc(profile.toJson());
            return jsonResponse(QString::fromUtf8(doc.toJson()));
        }
        
        return errorResponse("Profile not found");
    }
    else if (method == "PUT") {
        // Update profile
        QJsonDocument doc = QJsonDocument::fromJson(body);
        Profile profile = Profile::fromJson(doc.object());
        profile.setId(id);
        
        if (pm->updateProfile(profile)) {
            QJsonDocument responseDoc(profile.toJson());
            return jsonResponse(QString::fromUtf8(responseDoc.toJson()));
        }
        
        return errorResponse("Failed to update profile");
    }
    else if (method == "DELETE") {
        // Delete profile
        if (pm->deleteProfile(id)) {
            return jsonResponse("{\"success\": true}");
        }
        
        return errorResponse("Failed to delete profile");
    }
    
    return errorResponse("Method not allowed");
}

QByteArray ApiHandler::jsonResponse(const QString& json)
{
    return json.toUtf8();
}

QByteArray ApiHandler::errorResponse(const QString& message)
{
    QJsonObject obj;
    obj["error"] = message;
    QJsonDocument doc(obj);
    return doc.toJson();
}
