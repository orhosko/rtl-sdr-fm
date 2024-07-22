#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QAudioSink>
#include <QFile>
#include <QIODevice>
#include "qobject.h"
#include "qtmetamacros.h"
#include "qtypes.h"

class AudioPlayer : public QIODevice {
    Q_OBJECT
public:
    AudioPlayer(QObject* parent = nullptr);
    ~AudioPlayer();
    void start(const QString &filePath);

protected:
    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char*, qint64) override;

private slots:
    void handleStateChanged();

private:
    QFile file;
    QAudioSink* audioSink;
    QIODevice* audioIODevice;
};

#endif // AUDIOPLAYER_H
