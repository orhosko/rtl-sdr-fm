#ifndef RAWDATAPROVIDER_H
#define RAWDATAPROVIDER_H

#include "qformlayout.h"
#include "qlayout.h"
#include <complex>
#include <fstream>
#include <string>
#include <vector>

class RawDataProvider
{
public:
    virtual ~RawDataProvider() = default;
    virtual std::vector<std::complex<float>> readData() = 0;
    virtual void spectrum(std::vector<std::complex<float>> &) = 0;
    // virtual void readData(std::vector<std::complex<float>> &data) = 0;
    virtual int open() = 0;
    virtual int close() = 0;
    virtual QLayout* view() = 0;
    virtual int getSampleRate() = 0;
};

#endif // RAWDATAPROVIDER_H

class FileReader : public RawDataProvider {
public:
    int open() override;
    int close() override;
    QLayout* view() override {return ui;}
    int getSampleRate() override {return sample_rate;}
    void setSampleRate(int sr) {sample_rate = sr;}
    std::vector<std::complex<float>> readData() override;
    void spectrum(std::vector<std::complex<float>> &) override {};
    FileReader();
    ~FileReader() {delete ui;}

    // std::array<std::complex<float>, 600000> readData(int &read) override = 0;
    // void readData(std::vector<std::complex<float>> &data) override = 0;
private:
    std::ifstream file;
    std::string file_name{"C:/Users/orhan.yilmaz/Documents/2.4mhz_104.3mhz.bin"};

    QFormLayout* ui = new QFormLayout();
    int sample_rate=2'400'000;
};
