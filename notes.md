# First Level Heading

Paragraph.

## Second Level Heading

Paragraph.

- bullet
+ other bullet
* another bullet
    * child bullet

1. ordered
2. next ordered

### Third Level Heading

Some *italic* and **bold** text and `inline code`.

An empty line starts a new paragraph.

Use two spaces at the end  
to force a line break.

A horizontal ruler follows:

---

Add links inline like [this link to the Qt homepage](https://www.qt.io),
or with a reference like [this other link to the Qt homepage][1].

    Add code blocks with
    four spaces at the front.

> A blockquote
> starts with >
>
> and has the same paragraph rules as normal text.

First Level Heading in Alternate Style
======================================

Paragraph.

Second Level Heading in Alternate Style
---------------------------------------

Paragraph.

[1]: https://www.qt.io

# demod atladığımız aşama

    // To mix the data down, generate a digital complex exponential
    // (with the same length as x1) with phase -F_offset/Fs
    // fc1 = np.exp(-1.0j*2.0*np.pi* F_offset/Fs*np.arange(len(x1)))
    // Now, just multiply x1 and the digital complex expontential
    // x2 = x1 * fc1

# play2() with comments

    void MainWindow::play2(){
        if(first) {
        // read_data = rawDataProvider->readData();
        // next_data = fm->aa(read_data);
        //     prepare();
        //     byteBuffer=byteBuffer2;
        // read_data = rawDataProvider->readData();
        // next_data = fm->aa(read_data);
        //     prepare();
            first=0;
            QThread::sleep(2);
        }
        // TODO: Bunlar da ayrı thread'e, olmuyomuş::(
        audio->start(input);
        vis1->update(next_data);
        vis2->update(next_data);
        vis3->update(read_data);
        // prepare();
        // input->close();
        // input->setData(*byteBuffer2);
        // input->open(QIODevice::ReadOnly);
        // audio->start(input);
        // byteBuffer->append(*byteBuffer2);
        // prepare();
    }

# düz play()

    // QObject::connect(audioThread, &QThread::started, [=]() {
    //     prepare();
    // });
    // QObject::connect(audioThread, &QThread::finished, audioThread, &QObject::deleteLater);
    // audioThread->start();
    // audio->moveToThread(audioThread);
    // QObject::connect(audioThread, &QThread::started, [=]() {
        // input = new QBuffer(byteBuffer);
        // input->open(QIODevice::ReadOnly);
        // audio = new QAudioSink(audioFormat);
    // });
    // QObject::connect(player, &QIODevice::aboutToClose, audioThread, &QThread::quit);
    // QObject::connect(audioThread, &QThread::finished, player, &QObject::deleteLater);
    // QObject::connect(audioThread, &QThread::finished, audioThread, &QObject::deleteLater);
    // audioThread->start();
    if(!first) {
    delete byteBuffer;
    byteBuffer=byteBuffer2;
    // initialize parameters
    // qreal duration = 1.000;     // duration in seconds
    // qreal frequency = 1000;     // frequency
    // create a QAudioDeviceInfo object, to make sure that our audioFormat is supported by the device
    // QAudioDeviceInfo deviceInfo(QAudioDeviceInfo::defaultOutputDevice());
    // if(!deviceInfo.isFormatSupported(audioFormat))
    // {
    //     qWarning() << "Raw audio format not supported by backend, cannot play audio.";
    //     return;
    // }
    // QAudioDevice info(QMediaDevices::defaultAudioOutput());
    // if (!info.isFormatSupported(audioFormat)) {
    //     qWarning() << "Raw audio format not supported by backend, cannot play audio.";
    //     return;
    // }
    // Make a QBuffer with our QByteArray
    delete input;
    input = new QBuffer(byteBuffer);
    input->open(QIODevice::ReadOnly);
    // audio->start(&sourceFile);
    audio = new QAudioSink(audioFormat);
    // audio->setBufferSize(1000000);
    connect(audio, &QAudioSink::stateChanged, this, &MainWindow::handleStateChanged);
    // Create an audio output with our QAudioFormat
    // QAudioOutput* audio = new QAudioOutput(audioFormat, this);
    // connect up signal stateChanged to a lambda to get feedback
    // connect(audio, &QAudioOutput::stateChanged, [audio, input](QAudio::State newState)
    //         {
    //             if (newState == QAudio::IdleState)   // finished playing (i.e., no more data)
    //             {
    //                 qDebug() << "finished playing sound";
    //                 delete audio;
    //                 delete input;
    //                 //delete byteBuffer;  // I tried to delete byteBuffer pointer (because it may leak memories), but got compiler error. I need to figure this out later.
    //             }
    //             // should also handle more states, e.g., errors. I need to figure out on how to do this later.
    //         });
    // // start the audio (i.e., play sound from the QAudioOutput object that we just created)
    audio->start(input);
    vis1->update(next_data);
    }
    // prepare();
    if(first) {
        first=0;
        play();
    }
}

# RtAudio testim

    // Two-channel sawtooth wave generator.
    int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
            double streamTime, RtAudioStreamStatus status, void *userData )
    {
        unsigned int i, j;
        double *buffer = (double *) outputBuffer;
        double *lastValues = (double *) userData;
        if ( status )
            std::cout << "Stream underflow detected!" << std::endl;
        // Write interleaved audio data.
        for ( i=0; i<nBufferFrames; i++ ) {
            for ( j=0; j<2; j++ ) {
                *buffer++ = lastValues[j];
                lastValues[j] += 0.005 * (j+1+(j*0.1));
                if ( lastValues[j] >= 1.0 ) lastValues[j] -= 2.0;
            }
        }
        return 0;
    }
    
    RtAudio dac;
    std::vector<unsigned int> deviceIds = dac.getDeviceIds();
    if ( deviceIds.size() < 1 ) {
        std::cout << "\nNo audio devices found!\n";
        exit( 0 );
    }
    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;
    unsigned int sampleRate = 44100;
    unsigned int bufferFrames = 256; // 256 sample frames
    double data[2] = {0, 0};
    if ( dac.openStream( &parameters, NULL, RTAUDIO_FLOAT64, sampleRate,
                       &bufferFrames, &saw, (void *)&data ) ) {
        std::cout << '\n' << dac.getErrorText() << '\n' << std::endl;
        exit( 0 ); // problem with device settings
    }
    // Stream is open ... now start it.
    if ( dac.startStream() ) {
        std::cout << dac.getErrorText() << std::endl;
        goto cleanup;
    }
    char input;
    std::cout << "\nPlaying ... press <enter> to quit.\n";
    // std::cin.get( input );
    sleep(0);
    // Block released ... stop the stream
    if ( dac.isStreamRunning() )
        dac.stopStream();  // or could call dac.abortStream();
    cleanup:
        if ( dac.isStreamOpen() ) dac.closeStream();

# packed_bytes_to_iq() yanlış çözümler
    // data.clear();
    // for(int j=0;j<BUFFER_SIZE;j+=2){
    //     float i = bytes[j];
    //     float q = bytes[j+1];
    //     data.push_back(std::complex(i,q));
    // }
    // return;

    // data.clear();
    // for(int j=0;j<BUFFER_SIZE;j+=2){
    //     float i = static_cast<float>(bytes[j]);
    //     float q = static_cast<float>(bytes[j+1]);
    //     data.push_back(std::complex(i,q));
    // }
    // return;

    /*
    def read_samples(self, num_samples=DEFAULT_READ_SIZE):
        """Read specified number of complex samples from tuner.

        Real and imaginary parts are normalized to be in the range [-1, 1].
        Data is safe after this call (will not get overwritten by another one).

        Arguments:
            num_samples (:obj:`int`, optional): Number of samples to read.
                Defaults to :attr:`DEFAULT_READ_SIZE`.

        Returns:
            The samples read as either a :class:`list` or :class:`numpy.ndarray`
            (if available).
        """
        num_bytes = 2*num_samples

        raw_data = self.read_bytes(num_bytes)
        iq = self.packed_bytes_to_iq(raw_data)

        return iq

    def packed_bytes_to_iq(self, bytes):
        """Unpack a sequence of bytes to a sequence of normalized complex numbers

        This is called automatically by :meth:`read_samples`.

        Returns:
            The unpacked iq values as either a :class:`list` or
            :class:`numpy.ndarray` (if available).
        """
        if has_numpy:
            # use NumPy array
            data = np.ctypeslib.as_array(bytes)
            iq = data.astype(np.float64).view(np.complex128)
            iq /= 127.5
            iq -= (1 + 1j)
        else:
            # use normal list
            iq = [complex(i/(255/2) - 1, q/(255/2) - 1) for i, q in izip(bytes[::2], bytes[1::2])]

        return iq
*/

        // std::copy(reinterpret_cast<const char*>(&bytes[0]),
        //           reinterpret_cast<const char*>(&bytes[4]),
        //           reinterpret_cast<char*>(&result));
        // float i = static_cast<float>(bytes[i]) * 255.0f;
        // float q = static_cast<float>(bytes[i+1]) * 255.0f;

# işe yaramayan fft

```
#include <iostream>
#include <complex>
#include <cmath>
#include <iterator>

unsigned int bitReverse(unsigned int x, int log2n)
{
    int n = 0;
    int mask = 0x1;
    for (int i = 0; i < log2n; i++)
    {
        n <<= 1;
        n |= (x & 1);
        x >>= 1;
    }
    return n;
}

const double PI = 3.1415926536;

template<class Iter_T>
void fft(Iter_T a, Iter_T b, int log2n)
{
    typedef typename std::iterator_traits<Iter_T>::value_type complex;
    const std::complex J(0, 1);
    int n = 1 << log2n;
    for (unsigned int i = 0; i < n; ++i)
    {
        b[bitReverse(i, log2n)] = a[i];
    }

    for (int s = 1; s <= log2n; ++s)
    {
        int m = 1 << s;
        int m2 = m >> 1;
        std::complex w(1, 0);
        std::complex wm = std::exp(-J * (PI / m2));
        for (int j = 0; j < m2; ++j)
        {
            for (int k = j; k < n; k += m)
            {
                std::complex t = w * b[k + m2];
                std::complex u = b[k];
                b[k] = u + t;
                b[k + m2] = u - t;
            }
            w *= wm;
        }
    }
}
```

# işe yaramayan sinyaller

```
    // ---- TEST ----
    connect(input.get(), &QIODevice::readyRead, [=] {
        std::cout << "readyRead" << std::endl;
        std::cout << input->bytesAvailable() << std::endl;
    });

    connect(input.get(), &QBuffer::readyRead, [=] {
        std::cout << "readyRead" << std::endl;
        std::cout << input->bytesAvailable() << std::endl;
    });

    connect(input.get(), &QIODevice::bytesWritten, [=] {
        std::cout << "bytesWritten" << std::endl;
        std::cout << input->bytesAvailable() << std::endl;
    });

    connect(input.get(), &QBuffer::bytesWritten, [=] {
        std::cout << "readyRead" << std::endl;
        std::cout << input->bytesAvailable() << std::endl;
    });
```

# audioplayer'ı cmaketen çıkardım
