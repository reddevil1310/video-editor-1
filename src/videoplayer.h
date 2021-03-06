#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "media.h"
#include <QWidget>
#include <QMediaContent>
#include <QMediaPlaylist>

class QMediaPlayer;
class QVideoWidget;
class QVBoxLayout;
class PlayerControl;
class MediaFileInfo;

/**
 * @brief The VideoPlayer class
 * A graphic class to connect the mediaPlayer with playerControl
 */
class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~VideoPlayer();
    PlayerControl *getPlayerControl();
    QMediaPlayer *getMediaPlayer();
    void setPlayMode(QMediaPlaylist::PlaybackMode pbm);

public slots:
    void addMediaToPlaylist(Media& media);
    void removeMediaToPlaylist(int i);
    void moveMediaInPlaylist(int a, int b);
    void renameMedia(int i, Media& media);
    void setCurrentMedia(int i, Media& media);
    void emitCurrentMediaChanged(int i);
    void timeAsked();
    void playerReady();

signals:
    void currentMediaChanged(int i);
    void sendDuration(qint64 duration);

private:
    bool isReady;
    bool durationIsNeeded;
    int lastIndex;
    QVBoxLayout *vLayout;

    QMediaPlayer *mediaPlayer;
    QMediaPlaylist *mediaPlaylist;
    QVideoWidget *videoWidget;
    PlayerControl *playerControl;

};

#endif // VIDEOPLAYER_H
