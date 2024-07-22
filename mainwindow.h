#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "audiosink.h"
#include "fm_demod.h"
#include "fullspectrum.h"
#include "qaudio.h"
#include "qbytearray.h"
#include "qtconfigmacros.h"
#include "qtimer.h"
#include "qtmetamacros.h"
#include "qwidget.h"
#include "rtlsdr.h"
#include "visualizer.h"
#include "worker.h"
#include <complex>
#include <memory>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    std::vector<float> buffer_data;
    std::vector<float> next_data;
    std::vector<std::complex<float>> read_data;
    std::vector<std::complex<float>> read_data2;
    FM_Demod *fm;

    bool running=0;
    bool first=1;
    int buff_no=1;

    enum class SourceMode{
        RTLSDR = 0,
        File,
    };

    SourceMode source_mode = SourceMode::RTLSDR;
    double cur_freq=104.1;

    void stopAudioOutput();
    void prepare(int section_no);

    std::unique_ptr<Visualizer> vis1 = std::make_unique<TimeSink>();
    std::unique_ptr<Visualizer> vis2 = std::make_unique<FreqSink>();
    std::unique_ptr<Visualizer> vis3 = std::make_unique<FreqSink>();

    std::unique_ptr<FullSpectrum> spec{new FullSpectrum};
private slots:
    void on_pushButton_clicked();
    void handleStateChanged(QAudio::State newState);

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    RawDataProvider *rawDataProvider;
    std::unique_ptr<AudioSink> audio_sink{new AudioSink};
    std::unique_ptr<QTimer> vis_timer{new QTimer(this)};


    QByteArray *byteBuffer;
    std::unique_ptr<QByteArray> byteBuffer2 = std::make_unique<QByteArray>();

    int sample_rate = 40'000;

    Controller *controller= new Controller;
    std::unique_ptr<Controller> reader{new Controller};
};
#endif // MAINWINDOW_H
