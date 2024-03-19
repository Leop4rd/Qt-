#ifndef GOMOKUVOICE_H
#define GOMOKUVOICE_H

#include <QObject>
#include <QSound>
#include <QMediaPlayer>
#include <QMediaPlaylist>
class GomokuVoice : public QObject
{
    Q_OBJECT
public:
    explicit GomokuVoice(QObject *parent = nullptr);
    ~GomokuVoice();
    QSound *m_win =new QSound(":/voice/resource/music/WinSound.wav",this);
    QSound *m_dropPiese =new QSound(":/voice/resource/music/dropPiese.wav",this);
    QSound *m_bgm=nullptr;
    //QSound *m_bgm=new QSound(":/voice/resource/music/bgmHWS.WAV",this);
    QMediaPlayer *m_mediaBgm;
    QMediaPlaylist * m_mediaList;
    void playWinVoice();
    void playDropPieseVoice();
    void playBgm();
    void bgmInit();
    void stopBgm();
    void startBgm();
    void bgmReload(QMediaPlaylist *mediaList);
    void setBgmVolume(int value);


signals:
};

#endif // GOMOKUVOICE_H
