#ifndef WEATHERTOOL_H
#define WEATHERTOOL_H
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <map>
#include <QFile>
#include <QCoreApplication>
#include <stdexcept>

class WeatherTool
{
private:
    std::map<QString,QString> m_mapCity2Code;
public:
    WeatherTool();
    QString operator[](const QString& city);
};

#endif // WEATHERTOOL_H
