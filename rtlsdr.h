#ifndef RTLSDR_H
#define RTLSDR_H

#include "filewriter.h"
#include "qcheckbox.h"
#include "qformlayout.h"
#include "qlayout.h"
#include "rawdataprovider.h"
#include "rtl-sdr.h"
#include <complex>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#define BUFFER_SIZE 1200000
class RtlSdr : public RawDataProvider,public FileSaveable
{
public:
    RtlSdr();
    ~RtlSdr() override = default;

    std::vector<std::complex<float>> readData() override;
    int open() override;
    int close() override;
    QLayout* view() override {return ui.get();}
    int getSampleRate() override {return sample_rate;}

    void toggleSave() override;
    std::unique_ptr<QLayout> controlWidget() override;

protected:
    void setDevice(rtlsdr_dev_t *device);

private:
    rtlsdr_dev_t *device = nullptr;
    uint8_t buffer[BUFFER_SIZE];

    void read_bytes(rtlsdr_dev_t *device, uint8_t buffer[]);
    void packed_bytes_to_iq(uint8_t bytes[], std::vector<std::complex<float>> &data);

    std::unique_ptr<QFormLayout> ui = std::make_unique<QFormLayout>();
    void prepareUI();

    int index=0;
    int sample_rate = 1'200'000;

    bool save_mode = true;
    bool truncate = true;
    std::string file_name = "test1.bin";
    std::fstream bin_file;

    std::unique_ptr<QCheckBox> checkbox1;
    std::unique_ptr<QCheckBox> checkbox2;

    void spectrum(std::vector<std::complex<float>> &s) override;
};

#endif // RTLSDR_H
