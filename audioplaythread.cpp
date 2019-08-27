#include "audioplaythread.h"
#include <QDebug>
#include <QSemaphore>


const int BufferSize = 44100 * 10000;
int iCurrentRead = 0;
int iCurrentWrite = 0;
char buffer[BufferSize];

QSemaphore freeBytes(BufferSize);
QSemaphore usedBytes;

AudioPlayThread::AudioPlayThread(QObject *parent):
    QThread (parent),
    m_audioOutput(nullptr),
    m_audioOutDevice(nullptr),
    m_buf(nullptr),
    m_index(0)
{
    initializeAudio(QAudioDeviceInfo::defaultOutputDevice());

}

void AudioPlayThread::updateAudioData(QByteArray audio)
{
    int out_buffer_size = audio.size();
    if(freeBytes.tryAcquire(out_buffer_size)){

        memcpy(buffer + iCurrentWrite, audio.constData(), out_buffer_size);
        iCurrentWrite += out_buffer_size;

        iCurrentWrite = iCurrentWrite % BufferSize;
        usedBytes.release(out_buffer_size);
    }

}

void AudioPlayThread::run()
{
    while (1) {

#if 0
        QByteArray buffer(32768, 0);
        int chunks = m_audioOutput->bytesFree() / m_audioOutput->periodSize();
        while (chunks) {
           const qint64 len = m_generator->read(buffer.data(), m_audioOutput->periodSize());
           if (len)
               io->write(buffer.data(), len);
           if (len != m_audioOutput->periodSize())
               break;
           --chunks;
        }
#endif

        int chunks = m_audioOutput->bytesFree() / m_audioOutput->periodSize();
        while (chunks) {
            int len = m_audioOutput->periodSize();
            if(usedBytes.tryAcquire(len)){




                m_audioOutDevice->write(buffer + iCurrentRead, len);

                iCurrentRead += len;

                iCurrentRead = iCurrentRead % BufferSize;
                freeBytes.release();
            }

           --chunks;
        }

    }
}

void AudioPlayThread::initializeAudio(const QAudioDeviceInfo &deviceInfo)
{
    if(m_audioOutput){
        m_audioOutput->stop();
        delete  m_audioOutput;
        m_audioOutput = nullptr;
    }

    if(m_audioOutDevice){
        m_audioOutDevice->close();
        delete  m_audioOutDevice;
        m_audioOutDevice = nullptr;
    }


    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    if (!deviceInfo.isFormatSupported(format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        format = deviceInfo.nearestFormat(format);
    }

    m_audioOutput = new QAudioOutput(deviceInfo, format);

    m_audioOutDevice = m_audioOutput->start();

//    m_pcmStream = new PcmStream();
//    m_pcmStream->start();

    int bf = m_audioOutput->bytesFree();
    if(!m_buf){
        m_buf = new char[bf * 2000];
    }

    memset(m_buf, 0, bf * 2000);
    m_index = 0;
}


