#include "gomokuvoice.h"
#include <QDebug>

GomokuVoice::GomokuVoice(QObject *parent) : QObject(parent)
{
    m_mediaBgm = new QMediaPlayer;
    m_mediaList = new QMediaPlaylist;

    bgmInit();

}

GomokuVoice::~GomokuVoice()
{
    qDebug() << "delete GomkuVoice";
    if(m_win!= nullptr)
        delete m_win;
    if(m_dropPiese != nullptr)
        delete m_dropPiese;
    if(m_bgm != nullptr)
        delete m_bgm;
    if(m_mediaBgm!=nullptr)
        delete m_mediaBgm;
    if(m_mediaList!=nullptr)
        delete m_mediaList;
}

void GomokuVoice::playWinVoice()
{
    if(m_win != nullptr)
        this->m_win->play();
}

void GomokuVoice::playDropPieseVoice()
{
    if(m_dropPiese != nullptr)
        this->m_dropPiese->play();
}

void GomokuVoice::playBgm()
{
    if(m_bgm != nullptr)
        this->m_bgm->play();
}

void GomokuVoice::bgmInit()
{
    bgmReload(m_mediaList);
    m_mediaBgm->setPlaylist(m_mediaList);
    m_mediaBgm->setVolume(50);
    //m_mediaBgm->play();
}

void GomokuVoice::stopBgm()
{
    if(m_mediaBgm!=nullptr){
        m_mediaBgm->stop();

    }
}

void GomokuVoice::startBgm()
{
    if(m_mediaBgm!=nullptr){
        bgmReload(m_mediaList);
        m_mediaBgm->setPlaylist(m_mediaList);

        m_mediaBgm->play();
    }
}

void GomokuVoice::bgmReload(QMediaPlaylist *mediaList)
{
    if(mediaList == nullptr) return;
    mediaList->clear();
    mediaList->addMedia(QUrl("qrc:/voice/resource/music/bgmHWS.mp3"));
    mediaList->addMedia(QUrl("qrc:/voice/resource/music/ZDZY_Jay.mp3"));

    m_mediaList->setPlaybackMode(QMediaPlaylist::Loop);
}

void GomokuVoice::setBgmVolume(int value)
{
    if(m_mediaBgm!=nullptr){
        m_mediaBgm->setVolume(value);
    }
}
