#include "fullspectrum.h"
#include "kiss_fft.h"
#include "qnamespace.h"
#include <vector>

FullSpectrum::FullSpectrum() {
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

    input = (kiss_fft_cpx *)malloc(sizeof(kiss_fft_cpx) * nfft);
    output = (kiss_fft_cpx *)malloc(sizeof(kiss_fft_cpx) * nfft);
}

void FullSpectrum::prepare(const std::vector<std::complex<float>> &newData) {
    if(series) delete series;

    auto low = 98'000'000;
    auto high = 104'000'000;

    auto bw = 1'200'000;
    auto sample_rate = 600'000;

    series = new QLineSeries;
    int N = newData.size();
    for(auto i=0; i<bw*5; i+=sample_rate) {
        for (int j=0; j<(nfft / 2 +1); j++) {
            input[j].r = newData[i/2+j].real();
            input[j].i = newData[i/2+j].imag();
        }

        kiss_fft(cfg, input, output);

        for (int k = 0; k < (nfft / 2 + 1); ++k) {
            output[k].r = output[k].r * output[k].r + output[k].i + output[k].i;
            if (output[k].r <= 0) continue;
            series->append(*new QPointF(low + i + 1.1*k*sample_rate/(nfft /2 +1), output[k].r));
        }
    }

    // add center freq
    chart->addSeries(series);
    x_axis->setRange(low, high);
    y_axis->setRange(-1000, 1000);
    series->attachAxis(x_axis.get());
    series->attachAxis(y_axis.get());
}

FullSpectrum::~FullSpectrum(){
    free(input);
    free(output);
    kiss_fft_free(cfg);
}
