#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startWorkInAThread(char *szFileName)
{
    AVThread *workerThread = new AVThread(this);

    connect(workerThread, &AVThread::updateVideoPic, ui->label, &ImageDispaly::slotGetOneFrame);
    connect(workerThread, &AVThread::updateAudioData, this, &MainWindow::updateAudioData);
    connect(workerThread, &AVThread::finished, workerThread, &QObject::deleteLater);

    workerThread->openAV(szFileName);
}

void MainWindow::initializeAudio(const QAudioDeviceInfo &deviceInfo)
{
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

    m_audioStream.reset(new PcmStream());
    m_audioOutput.reset(new QAudioOutput(deviceInfo, format));
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName();

    if(fileName.isEmpty())
        return;

    char szFileName[255] = {0};
    memcpy(szFileName, fileName.toUtf8().constData(), fileName.length());

    initializeAudio(QAudioDeviceInfo::defaultOutputDevice());

    startWorkInAThread(szFileName);

    m_audioStream->start();
    m_audioOutput->start(m_audioStream.data());
//    out = m_audioOutput->start();
}

void MainWindow::updateAudioData(char *audioBuffer, int out_buffer_size)
{
//    out->write(audioBuffer, out_buffer_size);
    m_audioStream->setData(audioBuffer, out_buffer_size);
}
