#ifndef PCMSTREAM_H
#define PCMSTREAM_H

#include <QObject>
#include <QIODevice>

class PcmStream : public QIODevice
{
public:
    PcmStream();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 setData(const char *data, qint64 len);
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;

private:
    qint64 m_pos;
    QByteArray m_buffer;
};

#endif // PCMSTREAM_H
