//#include "mainwindow.h"
#include <QApplication>
#include <QDialog>
#include "avplayer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
    AVPlayer w;
    w.show();
    w.open("/home/spygg/Downloads/test.mp3");
//    w.open("/home/spygg/Downloads/download.mp4");


    return a.exec();
}
