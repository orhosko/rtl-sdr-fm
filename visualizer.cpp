#include "visualizer.h"
#include "kiss_fft.h"
#include "kiss_fftr.h"
#include "qlineseries.h"
#include "qlogvalueaxis.h"
#include "qnamespace.h"
#include "qpoint.h"
#include "qvalueaxis.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

TimeSink::TimeSink(){
    chart_view->setChart(chart.get());

    x_axis->setLabelFormat("%g");
    // axisX->setTitleText("Samples");
    // axisY->setTitleText("Audio level");
    chart->addAxis(x_axis.get(), Qt::AlignBottom);
    chart->addAxis(y_axis.get(), Qt::AlignLeft);
    chart->legend()->hide();
    // m_chart->setTitle("data");
}

void TimeSink::update(const std::vector<float> &newData) {
    if(series) delete series;

    series = new QLineSeries();

    int N = newData.size();
    double max=0;
    if(N > 0) {
        for(int i=0; i< resolution; i++) {
            float time = N/40000.0;

            qreal y = newData[i*N/resolution];
            if(std::abs(y) > max) max=std::abs(y);
            // std::cout << "new data:"<< y << std::endl;
            series->append(*new QPointF(time*i/resolution, y));
        }
    }

    chart->addSeries(series);
    x_axis->setRange(0, N/sample_rate);
    y_axis->setRange(-max-0.2, max+0.2);
    series->attachAxis(x_axis.get());
    series->attachAxis(y_axis.get());
}

TimeSink::~TimeSink() = default;

FreqSink::FreqSink(){
    chart_view->setChart(chart.get());

    y_axis->setBase(8.0);
    y_axis->setMinorTickCount(-1);

    x_axis->setLabelFormat("%g");
    // axisX->setTitleText("Samples");
    // axisY->setTitleText("Audio level");
    chart->addAxis(x_axis.get(), Qt::AlignBottom);
    chart->addAxis(y_axis.get(), Qt::AlignLeft);
    chart->legend()->hide();
    // m_chart->setTitle("data");

    if (!cfg) {
        fprintf(stderr, "Failed to allocate FFT configuration\n");
        return;
    }

    input = (kiss_fft_scalar *)malloc(sizeof(kiss_fft_scalar) * nfft);
    input_c = (kiss_fft_cpx *)malloc(sizeof(kiss_fft_cpx) * (nfft / 2 + 1));
    output = (kiss_fft_cpx *)malloc(sizeof(kiss_fft_cpx) * (nfft / 2 + 1));
    if (!input || !output) {
        fprintf(stderr, "Failed to allocate memory for FFT input/output\n");
        free(cfg);
        return;
    }
}

void FreqSink::update(const std::vector<std::complex<float>> &newData, int center_freq=0) {
    sample_rate = 1'200'000;
    center_freq = center_freq * 1'000'000;
    if(series) delete series;

    series = new QLineSeries();
    int N = newData.size();
    if(N > 0) {
        for (int i=0; i<(nfft / 2 +1); i++) {
            input_c[i].r = newData[i].real();
            input_c[i].i = newData[i].imag();
        }

        kiss_fft(cfg_c, input_c, output);

        for (int k = 0; k < (nfft / 2 + 1); k += 4) {
            output[k].r = output[k].r * output[k].r + output[k].i + output[k].i;
            if (output[k].r <= 0) continue;
            series->append(*new QPointF(center_freq + k*sample_rate/(nfft /2 +1), output[k].r));
        }
    }

    // add other half
    chart->addSeries(series);
    x_axis->setRange(center_freq, center_freq + 600'000);
    y_axis->setRange(-1000, 1000);
    series->attachAxis(x_axis.get());
    series->attachAxis(y_axis.get());
}

void FreqSink::update(const std::vector<float> &newData) {
    delete series;

    series = new QLineSeries();

    int N = newData.size();
    if(N > 0) {
        input = const_cast<float *>(&newData[0]);
        kiss_fftr(cfg, input, output);

        for (int k = 0; k < (nfft / 2 + 1); k+=8) {
            output[k].r = output[k].r * output[k].r + output[k].i + output[k].i;
            if (output[k].r <= 0) continue;
            series->append(*new QPointF(k*sample_rate/nfft, output[k].r));
        }
    }

    chart->addSeries(series);
    x_axis->setRange(0, sample_rate /2);
    y_axis->setRange(-1000, 1000);
    series->attachAxis(x_axis.get());
    series->attachAxis(y_axis.get());
}

bool FreqSink::isMusicc(const std::vector<float> &newData) {
    int N = newData.size();
    int nfft = 1024;
    kiss_fft_scalar* input = (kiss_fft_scalar *)malloc(sizeof(kiss_fft_scalar) * nfft);
    kiss_fft_cpx* output = (kiss_fft_cpx *)malloc(sizeof(kiss_fft_cpx) * (nfft / 2 + 1));
    if(N > 0) {
        input = const_cast<float *>(&newData[0]);
        kiss_fftr_cfg cfg = kiss_fftr_alloc(nfft, 0, 0, 0);
        kiss_fftr(cfg, input, output);

        double centroid = 0.0;
        double totalMagnitude = 0.0;

        for (int k = 0; k < (nfft / 2 + 1); k+=16) {
            output[k].r = output[k].r * output[k].r + output[k].i + output[k].i;

            centroid += k * output[k].r;
            totalMagnitude += output[k].r;
        }

        centroid /= totalMagnitude;
        std::cout << centroid << std::endl;
        return (centroid > 50);
    }
    free(input);
    free(output);
    return 0;
}

FreqSink::~FreqSink(){
    free(input);
    free(input_c);
    free(output);
    kiss_fftr_free(cfg);
}
