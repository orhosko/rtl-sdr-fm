#include "audiosink.h"
#include <boost/circular_buffer.hpp>
#include "qaudio.h"
#include "qaudioformat.h"
#include <memory>
#include "qaudiosink.h"
#include "qbuffer.h"
#include "qiodevice.h"
#include "qthread.h"
#include "qtmetamacros.h"
#include <iostream>

AudioSink::AudioSink(){

    audioFormat.setSampleRate(40'000);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleFormat(QAudioFormat::Float);

    input = std::make_unique<QBuffer>(byteBuffer.get());
    input->open(QIODevice::ReadWrite);

    audio = std::make_unique<QAudioSink>(audioFormat);
    connect(audio.get(), &QAudioSink::stateChanged, this, &AudioSink::handleStateChanged);
}

void AudioSink::play(){
    if(first) {
        first=false;
        QThread::sleep(1);
    }

    audio->start(input.get());
}

void AudioSink::handleStateChanged(QAudio::State newState)
{
    // std::cout << "state changed" << newState << std::endl;
    // std::cout << audio->error() << newState << std::endl;

    switch (newState) {
    case QAudio::IdleState:
        // Finished playing (no more data)

        std::cout << "--------- IDLE" << std::endl;
        emit needData();
        if(running) play();
        break;

    case QAudio::StoppedState:
        // Stopped for other reasons
        if (audio->error() != QAudio::NoError) {
            // Error handling
            std::cout << "is there any errors?????" << std::endl;
        }
        // stopAudioOutput();
        break;

    default:// ... other cases as appropriate
        break;
    }
}

void AudioSink::stopAudioOutput()
{
    audio->stop();
    input->close();
}

void AudioSink::setRunning(bool newRunning)
{
    running = newRunning;
}
