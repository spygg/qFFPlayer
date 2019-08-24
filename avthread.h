#ifndef AVTHREAD_H
#define AVTHREAD_H

#include "pcmstream.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include "libavutil/avstring.h"
#include "libavutil/eval.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"
#ifdef __cplusplus
};
#endif

#include <QThread>
#include <QImage>
#include <QAudioOutput>

class AVThread : public QThread
{
    Q_OBJECT

public:
    AVThread(QObject *parent = nullptr);
    ~AVThread();

public:
    void initAV();
    void openAV(char *szFileName);

signals:
    void updateVideoPic(QImage img);
    void updateAudioData(char*audioBuffer, int out_buffer_size);
private:
    void run();
    void initializeAudio(const QAudioDeviceInfo &deviceInfo);

private:
    char m_szFilename[255];
};

#endif // AVTHREAD_H
