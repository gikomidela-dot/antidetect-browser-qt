#ifndef AMNEZIAWGPARSER_H
#define AMNEZIAWGPARSER_H

#include <QString>
#include <QMap>

class AmneziaWGParser
{
public:
    static bool parse(const QString& configPath, QString& name, QMap<QString, QString>& parameters);
    static bool parseFromText(const QString& configText, QString& name, QMap<QString, QString>& parameters);
    
private:
    static void parseSection(const QString& section, const QString& content, QMap<QString, QString>& parameters);
};

#endif // AMNEZIAWGPARSER_H
