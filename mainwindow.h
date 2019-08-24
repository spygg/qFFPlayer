#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "avthread.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:

    void on_pushButton_clicked();
    void updateAudioData(char*audioBuffer, int out_buffer_size);
private:
    Ui::MainWindow *ui;

    QScopedPointer<PcmStream> m_audioStream;
    QScopedPointer<QAudioOutput> m_audioOutput;
    QIODevice *out;
private:
    void startWorkInAThread(char *szFileName);
    void initializeAudio(const QAudioDeviceInfo &deviceInfo);
};

#endif // MAINWINDOW_H
