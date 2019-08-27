#ifndef AVPLAYER_H
#define AVPLAYER_H

#include <QWidget>
#include "avdemuxthread.h"
#include "audioplaythread.h"
#include "imagedispaly.h"

class AVPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit AVPlayer(QWidget *parent = nullptr);
    void open(char *szFileName);

signals:

public slots:

private:


private:
    ImageDispaly *m_displyer;
};

#endif // AVPLAYER_H
