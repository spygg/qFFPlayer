#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QMainWindow>
#include <QTimer>
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
    void playAudioTimeout();
private:
    Ui::MainWindow *ui;


};

#endif // MAINWINDOW_H
