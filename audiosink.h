#ifndef AUDIOSINK_H
#define AUDIOSINK_H

#include "circularbytearrayh"
#include "qaudio.h"
#include "qaudioformat.h"
#include "qaudiosink.h"
#include "qbuffer.h"
#include "qobject.h"
#include "qtmetamacros.h"
#include <memory>

class AudioSink : public QObject
{
    Q_OBJECT

public:
    AudioSink();

    void play();
    void handleStateChanged(QAudio::State newState);
    void stopAudioOutput();
    void setRunning(bool newRunning);

    // TODO: make private
    std::unique_ptr<QBuffer> input;
    std::shared_ptr<QByteArray> byteBuffer{new CircularBuffer(80'000)};

private:
    QAudioFormat audioFormat{};
    std::unique_ptr<QAudioSink> audio;

    bool running = false;
    bool first = true;

signals:
    void needData();
};

#endif // AUDIOSINK_H
