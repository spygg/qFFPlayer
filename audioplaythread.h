#ifndef AUDIOPLAYTHREAD_H
#define AUDIOPLAYTHREAD_H

#include <QThread>
#include <QAudioOutput>
#include <QIODevice>

class AudioPlayThread : public QThread
{
public:
    AudioPlayThread(QObject *parent = nullptr);

public slots:
    void updateAudioData(QByteArray audio);

private:
    QAudioOutput *m_audioOutput;
    QIODevice *m_audioOutDevice;
//    PcmStream *m_pcmStream;
    char *m_buf;
    int m_index;

private:
    void run();
    void initializeAudio(const QAudioDeviceInfo &deviceInfo);
};

#endif // AUDIOPLAYTHREAD_H
