#include "avplayer.h"
#include <QDebug>

AVPlayer::AVPlayer(QWidget *parent) :
    QWidget(parent)

{
    resize(800, 600);
    m_displyer = new ImageDispaly(this);
    m_displyer->resize(800, 600);
    m_displyer->show();
}


void AVPlayer::open(char *szFileName)
{
    AVDemuxThread *workerThread = new AVDemuxThread(this);
    AudioPlayThread *audioThread = new AudioPlayThread(this);

    connect(workerThread, &AVDemuxThread::updateVideoPic, m_displyer, &ImageDispaly::slotGetOneFrame);
    connect(workerThread, &AVDemuxThread::updateAudioData, audioThread, &AudioPlayThread::updateAudioData);

    connect(workerThread, &AVDemuxThread::finished, workerThread, &QObject::deleteLater);
    connect(audioThread, &AudioPlayThread::finished, audioThread, &QObject::deleteLater);

    workerThread->openAV(szFileName);
    audioThread->start();
}


