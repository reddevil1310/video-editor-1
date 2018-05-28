#ifndef ACTIONS_H
#define ACTIONS_H

#include <QObject>
#include <QTime>
#include <QString>
#include <QVector>

#include "media.h"

class Actions : public QObject
{
public:
    static QString ffmpeg;
    static QString sox;
    static qint64 count;
    enum enumActions {NONE, DELETE_ZONE, DELETE_BEGIN, DELETE_END, MUT, TRIM, SPLIT, FUSION, NOISE};

    Actions();
    static QString getCommandOnVideo(Actions::enumActions action, QString nameVideo, QTime start, QTime end=QTime());
    static QString fusionVideos(QString finalName, QStringList nameOfVideos);
    static QString createProfileNoise(QString videoName, QTime start, QTime end);
    static QString getCommandApplyFilterNoise(QString videoName);
    static bool removeAllFileDir(const QString &dirName);
    static bool removeFile(QStringList nameOfVideos);
    static bool copyFile(QString src, QString dest);
    static bool renameFile(QString src, QString newname);
    bool executeCommand(QString command);
    ~Actions();

};

#endif // ACTIONS_H
