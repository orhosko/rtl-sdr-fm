#include "audioplayer.h"
#include <QAudioFormat>
#include "qaudio.h"
#include "qaudiosink.h"
#include "qobject.h"
#include "qstring.h"
#include "qtypes.h"

AudioPlayer::AudioPlayer(QObject* parent) : QIODevice(parent), audioSink(nullptr), audioIODevice(nullptr) {}

AudioPlayer::~AudioPlayer() {
    if (audioSink != nullptr) {
        audioSink->stop();
        delete audioSink;
    }
}

void AudioPlayer::start(const QString &filePath) {
    file.setFileName(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setSampleFormat(QAudioFormat::Int16);

    audioSink = new QAudioSink(format, this);
    audioSink->setBufferSize(4096);

    connect(audioSink, &QAudioSink::stateChanged, this, &AudioPlayer::handleStateChanged);

    open(QIODevice::ReadOnly);
    audioIODevice = audioSink->start();
}

auto AudioPlayer::readData(char *data, qint64 maxlen) -> qint64
{
    return file.read(data, maxlen);
}

auto AudioPlayer::writeData(const char *, qint64) -> qint64
{
    return -1;
}

void AudioPlayer::handleStateChanged() {
    if (audioSink->state() == QAudio::IdleState) {
        file.close();
        close();
    }
}
