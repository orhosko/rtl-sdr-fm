#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "kiss_fft.h"
#include "kiss_fftr.h"
#include "qchart.h"
#include "qchartview.h"
#include "qlineseries.h"
#include "qlogvalueaxis.h"
#include "qvalueaxis.h"
#include <complex>
#include <memory>
#include <vector>
class Visualizer
{
public:
    virtual ~Visualizer()= default;

    virtual QChartView* view() = 0;
    virtual void update(const std::vector<float> &newData) = 0;
    virtual void update(const std::vector<std::complex<float>> &newData, int center_freq)=0;
};

class TimeSink : public Visualizer
{
public:
    TimeSink();
    ~TimeSink();

    QChartView* view() override {return chart_view.get();}
    void update(const std::vector<float> &newData) override;
    void update(const std::vector<std::complex<float>> &newData, int center_freq) override {};

private:
    std::unique_ptr<QChartView> chart_view{new QChartView};

    std::unique_ptr<QChart> chart{new QChart};
    QLineSeries *series = nullptr;
    std::unique_ptr<QValueAxis> x_axis{new QValueAxis};
    std::unique_ptr<QValueAxis> y_axis{new QValueAxis};

    int sample_rate = 40'000;
    int resolution = 500;
};

class FreqSink : public Visualizer
{
public:
    FreqSink();
    ~FreqSink();

    QChartView* view() override {return chart_view.get();}
    void update(const std::vector<float> &newData) override;
    void update(const std::vector<std::complex<float>> &newData, int center_freq) override;

    bool isMusic{};
    static bool isMusicc(const std::vector<float> &newData);
private:
    std::unique_ptr<QChartView> chart_view{new QChartView};

    std::unique_ptr<QChart> chart{new QChart};
    QLineSeries *series = nullptr;
    std::unique_ptr<QValueAxis> x_axis{new QValueAxis};
    std::unique_ptr<QLogValueAxis> y_axis{new QLogValueAxis};

    kiss_fft_scalar *input;
    kiss_fft_cpx *input_c;
    kiss_fft_cpx *output;

    int sample_rate = 40'000;

    int nfft = 4096; //TODO: not sure whether can be other than 2s power
    kiss_fftr_cfg cfg = kiss_fftr_alloc(nfft, 0, 0, 0);
    kiss_fft_cfg cfg_c = kiss_fft_alloc(nfft / 2 + 1, 0, 0, 0);
};

#endif // VISUALIZER_H
