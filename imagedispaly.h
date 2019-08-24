#ifndef IMAGEDISPALY_H
#define IMAGEDISPALY_H

#include <QLabel>
#include <QImage>

class ImageDispaly : public QLabel
{
public:
    ImageDispaly(QWidget *parent=nullptr);

public slots:
    void slotGetOneFrame(QImage img);

protected:
    void paintEvent(QPaintEvent *);

private:
    QImage m_image;
};

#endif // IMAGEDISPALY_H
