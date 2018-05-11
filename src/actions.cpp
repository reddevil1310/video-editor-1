#include "actions.h"
#include <QFile>
#include <QTextStream>
#include <QMediaContent>

Actions::Actions()
{

}

Actions::~Actions()
{
    terminate();
}

QString Actions::getCommandOnVideo(Actions::enumActions action, QString name, QTime start, QTime end)
{
    QString str;
    QString videoName("preview/"+name);
    QString nameStart;
    QString nameEnd;
    QString nameVideos;
    switch(action) {
        case Actions::enumActions::DELETE_ZONE:
            if (end.isNull()) {
                return "";
            }
            // Récupération du début
            nameStart += "preview/start_"+name;
            str += "ffmpeg -i "+videoName;
            str += " -ss 00:00:00";
            str += " -to "+start.toString();
            str += " -c copy "+nameStart+";";
            // Récupération de la fin
            nameEnd += "preview/end_"+name;
            str += "ffmpeg -i "+videoName;
            str += " -ss "+end.toString();
            str += " -c copy "+nameEnd+";";
            // Concaténation des deux parties
            nameVideos += nameStart+"|"+nameEnd;
            str += fusionVideos(videoName,nameVideos.split("|"));
            // Suppression des vidéos temporaires
            str += "DELETE:"+nameStart+"|"+nameEnd;
            break;
        case Actions::enumActions::DELETE_BEGIN:
            str += "ffmpeg -i "+videoName;
            str += " -ss "+start.toString();
            str += " -c copy "+videoName+";";
            break;
        case Actions::enumActions::DELETE_END:
            str += "ffmpeg -i "+videoName;
            str += " -ss 00:00:00";
            str += " -to "+start.toString();
            str += " -c copy "+videoName+";";
            break;
        case Actions::enumActions::MUT:
            break;
        case Actions::enumActions::SPLIT:
            break;
        default: 
            return "";
    }
    return str;
}

QString Actions::fusionVideos(QString finalName, QStringList nameOfVideos)
{
    QString str("ffmpeg -i \"concat:");
    int i = 0;
    for (QString name : nameOfVideos) {
        if (i != 0) {
            str += '|';
        }
        str += "preview/"+name;
        ++i;
    }
    str += " -c copy preview/"+finalName+";";

    return str;
}

bool Actions::removeFile(QStringList nameOfVideos)
{
    bool success = true;
    for (QString name : nameOfVideos) {        
        if (!QFile::remove(name)) {
            success = false;
        }
    }
    return success;
}

void Actions::executeCommand(QString command)
{
    // Découpage de la commande si présence du mot DELETE
    QStringList nameVideosDelete;
    if (int index = command.indexOf("DELETE:")) {
        QStringList nameVideos = command.mid(index+6).split("|");
        command = command.left(index);
    }
    // Lancement de la commande
    start(command);
    // Suppression des fichiers si besoin
    if (!nameVideosDelete.isEmpty()) {
        removeFile(nameVideosDelete);
    }
}
