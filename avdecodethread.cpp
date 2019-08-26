#include "avdecodethread.h"
#include <QDebug>

#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

AVDecodeThread::AVDecodeThread(QObject *parent):QThread (parent)
{
    memset(m_szFilename, 0, sizeof(m_szFilename));
    initAV();
}

AVDecodeThread::~AVDecodeThread()
{
    avformat_network_deinit();
    qDebug() << "thread quit";
}

void AVDecodeThread::initAV()
{
    avformat_network_init();
}

void AVDecodeThread::openAV(char *szFileName)
{
    strcpy(m_szFilename, szFileName);
    start();
}

void AVDecodeThread::startWorkInAThread()
{
    AudioPlayThread *workerThread = new AudioPlayThread(this);

    connect(this, &AVDecodeThread::updateAudioData, workerThread, &AudioPlayThread::updateAudioData);
    connect(workerThread, &AVDecodeThread::finished, workerThread, &QObject::deleteLater);
}

void AVDecodeThread::run()
{
    AVFormatContext *avFormatContext = nullptr;
    AVPacket packet;
    AVFrame	*pFrame = nullptr;
    int audioStream= -1;
    int videoStream = -1;
    int err = -1;

    avFormatContext = avformat_alloc_context();
    if (!avFormatContext) {
        av_log(nullptr, AV_LOG_FATAL, "Could not allocate context.\n");
    }

    if(avformat_open_input(&avFormatContext, m_szFilename, nullptr, nullptr) != 0){

    }
    // Retrieve stream information
    if(avformat_find_stream_info(avFormatContext, nullptr)<0){
        qDebug() << "Couldn't find stream information";
        return;
    }

    for(unsigned int i=0; i < avFormatContext->nb_streams; i++){
        if(avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audioStream = static_cast<int>(i);
        }
        else if(avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStream = static_cast<int>(i);
        }
    }

    if(audioStream == -1 && videoStream == -1){
        qDebug() << "Didn't find a audio stream";
        return;
    }

    pFrame = av_frame_alloc();

    //音频
    AVCodecContext	*pCodecCtxAudio = nullptr;
    AVCodec			*pCodecAudio;
    uint8_t			*audioBuffer = nullptr;
    struct SwrContext *au_convert_ctx = nullptr;
    int out_buffer_size = 0;
    if(audioStream != -1)
    {
        pCodecCtxAudio = avcodec_alloc_context3(nullptr);
        if (!pCodecCtxAudio)
            return;

        //音频编码器设置
        err = avcodec_parameters_to_context(pCodecCtxAudio, avFormatContext->streams[audioStream]->codecpar);
        if(err < 0){
            return;
        }

        // Find the decoder for the audio stream
        pCodecAudio = avcodec_find_decoder(pCodecCtxAudio->codec_id);
        if(pCodecAudio == nullptr){
            qDebug() << "Codec not found";
            return;
        }

        // Open codec
        if(avcodec_open2(pCodecCtxAudio, pCodecAudio, nullptr)<0){
            qDebug() << "Could not open codec";
            return;
        }


        //Out Audio Param
        uint64_t out_channel_layout=AV_CH_LAYOUT_STEREO;
        //nb_samples: AAC-1024 MP3-1152
        int out_nb_samples = pCodecCtxAudio->frame_size;
        AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
        int out_sample_rate = 44100;
        int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
        //Out Buffer Size
        out_buffer_size = av_samples_get_buffer_size(nullptr,
                                                       out_channels,
                                                       out_nb_samples,
                                                       out_sample_fmt,
                                                       1);


        //FIX:Some Codec's Context Information is missing
        int64_t in_channel_layout = av_get_default_channel_layout(pCodecCtxAudio->channels);

        audioBuffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
        au_convert_ctx = swr_alloc();
        au_convert_ctx = swr_alloc_set_opts(au_convert_ctx,
                                          out_channel_layout,
                                          out_sample_fmt,
                                          out_sample_rate,
                                          in_channel_layout,
                                          pCodecCtxAudio->sample_fmt,
                                          pCodecCtxAudio->sample_rate,
                                          0,
                                          nullptr);
        swr_init(au_convert_ctx);
    }

    //视频
    AVCodecContext *pVideoCodecContext = nullptr;
    AVCodec			*pCodecVideo;
    AVCodecParameters *pVideoChannelCodecPara = nullptr;
    int iWindowWidth = 0;
    int iWindowHeight = 0;
    struct SwsContext *img_convert_ctx = nullptr;
    struct SwsContext *img_convert_ctx_rgb = nullptr;
    AVFrame *pFrameYUV = nullptr;
    AVFrame * pFrameRGB= nullptr;

    unsigned char *rgbBuffer;
    unsigned char *yuvBuffer;
    if(videoStream != -1)
    {
        //开始视频
        pVideoChannelCodecPara = avFormatContext->streams[videoStream]->codecpar;
        pVideoCodecContext =  avcodec_alloc_context3(nullptr);
        if (!pVideoCodecContext){
            qDebug() <<  "avcodec_alloc_context3";
            return;
        }

        err = avcodec_parameters_to_context(pVideoCodecContext, pVideoChannelCodecPara);
        if (err < 0){
            qDebug() << "avcodec_parameters_to_context";
            return;
        }

        pCodecVideo = avcodec_find_decoder(pVideoChannelCodecPara->codec_id);
        if(pCodecVideo == nullptr){
            qDebug() <<  "avcodec_find_decoder";
            return;
        }

        qDebug() << "编解码器名:" << pCodecVideo->long_name;

        err = avcodec_open2(pVideoCodecContext, pCodecVideo, nullptr);
        if(err){
            qDebug() << "avcodec_open2";
            return;
        }

        iWindowWidth = pVideoCodecContext->width;
        iWindowHeight = pVideoCodecContext->height;

        img_convert_ctx = sws_getContext(pVideoCodecContext->width,
                                         pVideoCodecContext->height,
                                         pVideoCodecContext->pix_fmt,
                                         pVideoCodecContext->width,
                                         pVideoCodecContext->height,
                                         AV_PIX_FMT_YUV420P,
                                         SWS_BICUBIC,
                                         nullptr,
                                         nullptr,
                                         nullptr);

        img_convert_ctx_rgb = sws_getContext(pVideoCodecContext->width,
                                             pVideoCodecContext->height,
                                             AV_PIX_FMT_YUV420P,
                                             pVideoCodecContext->width,
                                             pVideoCodecContext->height,
                                             AV_PIX_FMT_RGB32,
                                             SWS_BICUBIC,
                                             nullptr,
                                             nullptr,
                                             nullptr);


        pFrameYUV = av_frame_alloc();
        pFrameRGB = av_frame_alloc();


        //计算一个标准画面的大小
        yuvBuffer = static_cast<unsigned char *>(av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                                                        pVideoCodecContext->width,
                                                                        pVideoCodecContext->height,
                                                                        1)));

        rgbBuffer = static_cast<unsigned char *>(av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32,
                                                                        pVideoCodecContext->width,
                                                                        pVideoCodecContext->height,
                                                                        1
                                                                        )));

        av_image_fill_arrays(pFrameYUV->data,
                             pFrameYUV->linesize,
                             yuvBuffer,
                             AV_PIX_FMT_YUV420P,
                             pVideoCodecContext->width,
                             pVideoCodecContext->height,
                             1
                             );

        av_image_fill_arrays(pFrameRGB->data,
                             pFrameRGB->linesize,
                             rgbBuffer,
                             AV_PIX_FMT_RGB32,
                             pVideoCodecContext->width,
                             pVideoCodecContext->height,
                             1
                             );
        qDebug() << "视频宽度:" << iWindowWidth << "高度:" << iWindowHeight;
    }

    //前戏完毕
    qDebug() << "--------------- File Information ----------------";
    // Dump valid information onto standard error
    av_dump_format(avFormatContext, 0, m_szFilename, false);
    qDebug() << "-------------------------------------------------";

    while(av_read_frame(avFormatContext, &packet) >= 0){
        if(packet.stream_index == audioStream){
//            msleep(10);
            int got_frame = 0;
            err = avcodec_decode_audio4( pCodecCtxAudio,
                                         pFrame,
                                         &got_frame,
                                         &packet);
            if (err < 0) {
                qDebug() << "Error in decoding audio frame";
                return;
            }

            if (got_frame > 0){
                swr_convert(au_convert_ctx,
                            &audioBuffer,
                            MAX_AUDIO_FRAME_SIZE,
                            (const uint8_t **)pFrame->data,
                            pFrame->nb_samples);

                updateAudioData((char*)audioBuffer, out_buffer_size);

//                 m_audioStream->setData((char*)audioBuffer, out_buffer_size);
            }
            else{
                qDebug() << "no audio data";

            }
        }
        else if(packet.stream_index == videoStream){

            err = avcodec_send_packet(pVideoCodecContext, &packet);
            if(err != 0){
                if(AVERROR(EAGAIN) == err)
                    continue;
                qDebug() << "发送视频帧失败!"<<  err;
            }

            msleep(40);
            //解码
            while(avcodec_receive_frame(pVideoCodecContext, pFrame) == 0){

                sws_scale(img_convert_ctx,
                          pFrame->data,
                          pFrame->linesize,
                          0,
                          pVideoCodecContext->height,
                          pFrameYUV->data,
                          pFrameYUV->linesize
                          );

                if(img_convert_ctx_rgb){
                    sws_scale(img_convert_ctx_rgb,
                              pFrameYUV->data,
                              pFrameYUV->linesize,
                              0,
                              pVideoCodecContext->height,
                              pFrameRGB->data,
                              pFrameRGB->linesize);

                    //构造QImage，用于主页面显示
                    QImage image((uchar *)pFrameRGB->data[0], iWindowWidth, iWindowHeight,QImage::Format_ARGB32);
//                    QImage imagef = image.copy(); //把图像复制一份 传递给界面显

                    emit updateVideoPic(image);
                }

            }

        }

    }

    //    av_free(out_buffer);
    //    avcodec_close(pCodecCtx);
    avformat_close_input(&avFormatContext);

    qDebug() << "end of play";
}
