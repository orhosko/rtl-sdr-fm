#ifndef FULLSPECTRUM_H
#define FULLSPECTRUM_H

#include "kiss_fft.h"
#include "qchart.h"
#include "qchartview.h"
#include "qlineseries.h"
#include "qlogvalueaxis.h"
#include "qvalueaxis.h"
#include <complex>
#include <memory>
#include <vector>

class FullSpectrum
{
public:
    FullSpectrum();
    ~FullSpectrum();

    QChartView* view()  {return chart_view.get();}
    void prepare(const std::vector<std::complex<float>> &newData) ;
private:
    std::vector<std::complex<float>> spectrum;
    std::unique_ptr<QChartView> chart_view{new QChartView};

    std::unique_ptr<QChart> chart{new QChart};
    QLineSeries *series = nullptr;
    std::unique_ptr<QValueAxis> x_axis{new QValueAxis};
    std::unique_ptr<QLogValueAxis> y_axis{new QLogValueAxis};

    kiss_fft_cpx *input;
    kiss_fft_cpx *output;

    int sample_rate = 1'200'000;

    int nfft = 8192; //TODO: not sure whether can be other than 2s power
    kiss_fft_cfg cfg = kiss_fft_alloc(nfft, 0, 0, 0);
};

#endif // FULLSPECTRUM_H
