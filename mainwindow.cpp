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

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName();

    if(fileName.isEmpty())
        return;

    char szFileName[255] = {0};
    memcpy(szFileName, fileName.toUtf8().constData(), fileName.length());

//    initializeAudio(QAudioDeviceInfo::defaultOutputDevice());

//    startWorkInAThread(szFileName);

//    m_audioStream->start();
//    out = m_audioOutput->start();

//    int bf = m_audioOutput->bytesFree();
//    if(!m_buf){
//        m_buf = new char[bf * 2];
//    }

//    memset(m_buf, 0, bf * 2);
//    m_index = 0;

}


void MainWindow::playAudioTimeout()
{
//    QByteArray buffer(32768, 0);

//    int chunks = m_audioOutput->bytesFree() / m_audioOutput->periodSize();
//    while (chunks) {
//       const qint64 len = m_audioStream->read(buffer.data(), m_audioOutput->periodSize());
//       if (len)
//           out->write(buffer.data(), len);
//       if (len != m_audioOutput->periodSize())
//           break;
//       --chunks;
//    }
}
void MainWindow::updateAudioData(char *audioBuffer, int out_buffer_size)
{
//    int vv = m_audioOutput->bytesFree();
//    qDebug() << out_buffer_size << vv;
//    memcpy(m_buf + m_index, audioBuffer, out_buffer_size);
//    m_index += out_buffer_size;

//    if(m_index >= vv){
//        out->write(m_buf, m_index);
//        m_index = 0;
//    }
//    m_audioStream->write(audioBuffer, out_buffer_size);
}
