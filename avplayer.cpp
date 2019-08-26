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
    AVDecodeThread *workerThread = new AVDecodeThread(this);
    AudioPlayThread *audioThread = new AudioPlayThread(this);

    connect(workerThread, &AVDecodeThread::updateVideoPic, m_displyer, &ImageDispaly::slotGetOneFrame);
    connect(workerThread, &AVDecodeThread::updateAudioData, audioThread, &AudioPlayThread::updateAudioData);

    connect(workerThread, &AVDecodeThread::finished, workerThread, &QObject::deleteLater);
    connect(audioThread, &AudioPlayThread::finished, audioThread, &QObject::deleteLater);

    workerThread->openAV(szFileName);
    audioThread->start();
}


