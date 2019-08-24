#include "imagedispaly.h"
#include <QPainter>

ImageDispaly::ImageDispaly(QWidget *parent):QLabel (parent)
{

}

void ImageDispaly::slotGetOneFrame(QImage img)
{
    m_image = img;
    update(); //调用update将执行 paintEvent函数
}

void ImageDispaly::paintEvent(QPaintEvent *)
{

    if (m_image.size().width() <= 0)
        return;

    QPainter painter(this);

    ///将图像按比例缩放成和窗口一样大小
    QImage img = m_image.scaled(this->size(), Qt::IgnoreAspectRatio);
    int x = this->width() - img.width();
    int y = this->height() - img.height();
    painter.drawImage(QPoint(x, y), img); //画出图像
}
