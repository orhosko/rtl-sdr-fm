#include "rtlsdr.h"
#include "qcheckbox.h"
#include "qcombobox.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qspinbox.h"
#include "qtmetamacros.h"
#include "rtl-sdr.h"
#include <complex>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <QObject>

#define BUFFER_SIZE 1200000

RtlSdr::RtlSdr() {
    prepareUI();
}

void RtlSdr::prepareUI(){
    int count = rtlsdr_get_device_count();
    std::string label = "Device Count:" + std::to_string(count);
    auto device_count_label = new QLabel(label.c_str());

    ui->addRow(device_count_label);

    auto device_strings_label = new QLabel(".");


    auto device_selector = new QComboBox();

    for(int i=0;i<count; i++){
        device_selector->addItem(rtlsdr_get_device_name(i));
    }

    QAbstractSpinBox::connect(device_selector, &QComboBox::currentIndexChanged, [=, this](int newIndex){
        char a[255], b[255], c[255];
        int r = rtlsdr_get_device_usb_strings(newIndex, a, b, c);
        std::string s = std::to_string(newIndex) + "|" + std::string(a) + "|" + std::string(b) + "|" + std::string(c);
        device_strings_label->setText(s.c_str());

        index=newIndex;
        close();
        open();
    });

    emit device_selector->currentIndexChanged(0);

    ui->addRow(device_strings_label);
    ui->addRow(device_selector);

    // auto sample_rate_spinbox = new QSpinBox();
    // sample_rate_spinbox->setMaximum(3'200'000);
    // sample_rate_spinbox->setMinimum(900'001);
    // sample_rate_spinbox->setSingleStep(10000);
    // QAbstractSpinBox::connect(sample_rate_spinbox, &QSpinBox::valueChanged, [this](int newValue) -> void {
    //     int r = rtlsdr_set_sample_rate(this->device, newValue);
    //     std::cout << r << "|" << newValue << std::endl;
    // });

    // ui->addWidget(sample_rate_spinbox);
    // emit sample_rate_spinbox->valueChanged(1'200'000);
    // sample_rate_spinbox->setValue(1'200'000);

    auto center_freq_spinbox = new QDoubleSpinBox();
    center_freq_spinbox->setDecimals(1);
    center_freq_spinbox->setMaximum(107.9);
    center_freq_spinbox->setMinimum(15.1);
    center_freq_spinbox->setSingleStep(0.2);
    QAbstractSpinBox::connect(center_freq_spinbox, &QDoubleSpinBox::valueChanged, [this](double newValue) -> void{
        int r = rtlsdr_set_center_freq(this->device, int(newValue * 1'000'000));
        std::cout << r << "|" << int(newValue*1000000) << std::endl;
    });

    ui->addRow("Center Frequency:", center_freq_spinbox);

    auto forw_search_button = new QPushButton(">>");
    auto back_search_button = new QPushButton("<<");

    ui->addRow(back_search_button, forw_search_button);

    // auto freq_box = new QHBoxLayout();
    // freq_box->addWidget(label1);
    // freq_box->addWidget(center_freq_spinbox);
    // ui->addItem(freq_box);

    center_freq_spinbox->setValue(104.3);
    emit center_freq_spinbox->valueChanged(104.3);

    // TODO: find a good way to delete these
}

void RtlSdr::spectrum(std::vector<std::complex<float> > &s)
{
    auto low = 98'000'000;
    // auto high = 107'900'000;
    // 1.65 * 12 = 19.8
    // 1.2 * 16 = 19.2
    auto bw = 1'200'000;

    rtlsdr_set_tuner_bandwidth(device, bw);

    s.clear();
    for(auto i=0; i<5; i++) {
        rtlsdr_set_center_freq(device, low + (bw/2) + bw*i);

        auto d = readData();
        s.insert( s.end(), d.begin(), d.end() );
    }

    rtlsdr_set_tuner_bandwidth(device, 0);
}

int RtlSdr::open() {

    int count = rtlsdr_get_device_count();

    if(count == 0 ) {
        std::cout << "No device connected." << std::endl;
        return 1;
    }

    std::cout << rtlsdr_get_device_name(index) << std::endl;


    char a[255], b[255], c[255];
    int r = rtlsdr_get_device_usb_strings(index, a, b, c);

    std::cout << r << "|" << a << "|" << b << "|" << c << std::endl;

    if(rtlsdr_open(&device, index)){
        std::cout << "Couldn't open the device!";
        return 1;
    }

    setDevice(device);

    bin_file.open("test1.bin", std::ios::trunc | std::ios::out | std::ios::binary);
    if(!bin_file.is_open()) std::cout << "Couldn't open test1.bin" << std::endl;

    /*
    read_samples(device, 0, 0);
    bin_file.write(reinterpret_cast<char*>(data.data()), data.size() * sizeof(std::complex<float>));
    read_samples(device, 0, 0);
    bin_file.write(reinterpret_cast<char*>(data.data()), data.size() * sizeof(std::complex<float>));
    read_samples(device, 0, 0);
    bin_file.write(reinterpret_cast<char*>(data.data()), data.size() * sizeof(std::complex<float>));
    read_samples(device, 0, 0);
    bin_file.write(reinterpret_cast<char*>(data.data()), data.size() * sizeof(std::complex<float>));
    read_samples(device, 0, 0);
    bin_file.write(reinterpret_cast<char*>(data.data()), data.size() * sizeof(std::complex<float>));
    */
     // for(auto e:rtlsdr.data){
    //     float f1 = e.real();
    //     float f2 = e.imag();
    //     file.write(reinterpret_cast<char*>(&f1), sizeof(f1));
    //     file.write(reinterpret_cast<char*>(&f2), sizeof(f2));
    // }

    // std::cout << "----------------" << std::endl;
    //f.aa();

    return 0;
}

// TODO: ref version
std::vector<std::complex<float>> RtlSdr::readData() {
    std::vector<std::complex<float>> data;
    read_bytes(device, buffer);
    packed_bytes_to_iq(buffer, data);

    return data;
}

void RtlSdr::read_bytes(rtlsdr_dev_t *device, uint8_t buffer[]) {
    rtlsdr_reset_buffer(device);

    int read=0;
    rtlsdr_read_sync(device, buffer, BUFFER_SIZE, &read);

    std::cout << "Read:" << read << std::endl;
}


void RtlSdr::packed_bytes_to_iq(uint8_t bytes[], std::vector<std::complex<float>> &data) {
    data.clear();
    for(int j=0;j<BUFFER_SIZE;j+=2){

        float i = (bytes[j] / 127.5) - 1;
        float q = (bytes[j+1]/ 127.5) - 1;

        data.emplace_back(i,q);
    }

    bin_file.write(reinterpret_cast<char*>(data.data()), data.size() * sizeof(std::complex<float>));
}

void RtlSdr::setDevice(rtlsdr_dev_t *device) {

    std::cout << "setting device" << std::endl;
    this->device=device;

    int center_freq = 104'300'000;
    rtlsdr_set_center_freq(device, center_freq);

    rtlsdr_set_freq_correction(device, 0);
    std::cout << "Correction(ppm):" << rtlsdr_get_freq_correction(device) << std::endl;

    std::cout << rtlsdr_get_tuner_type(device);

    // enum rtlsdr_tuner {
    //     RTLSDR_TUNER_UNKNOWN = 0,
    //     RTLSDR_TUNER_E4000,
    //     RTLSDR_TUNER_FC0012,
    //     RTLSDR_TUNER_FC0013,
    //     RTLSDR_TUNER_FC2580,
    //     RTLSDR_TUNER_R820T,
    //     RTLSDR_TUNER_R828D
    // };

    int gains[100];
    int num_of_gains = rtlsdr_get_tuner_gains(device, gains);

    for(int i=0; i< num_of_gains; i++){
        std::cout << gains[i] << " ";
    }
    std::cout << std::endl;

    rtlsdr_set_agc_mode(device, 1);
    // rtlsdr_set_tuner_gain(device, 197);
    std::cout << "Current Gain:" << rtlsdr_get_tuner_gain(device) << std::endl;


    rtlsdr_set_testmode(device, 0);

    std::cout << "Direct Sampling:" << rtlsdr_get_direct_sampling(device) << std::endl;
    //rtlsdr_set_tuner_gain(rtlsdr_dev_t *dev, int gain);

    // \param bw bandwidth in Hz. Zero means automatic BW selection.
    //rtlsdr_set_tuner_bandwidth(rtlsdr_dev_t *dev, uint32_t bw);


/*!
 * Set the intermediate frequency gain for the device.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param stage intermediate frequency gain stage number (1 to 6 for E4000)
 * \param gain in tenths of a dB, -30 means -3.0 dB.
 * \return 0 on success
 */
// rtlsdr_set_tuner_if_gain(rtlsdr_dev_t *dev, int stage, int gain);

/*!
 * Set the gain mode (automatic/manual) for the device.
 * Manual gain mode must be enabled for the gain setter function to work.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param manual gain mode, 1 means manual gain mode shall be enabled.
 * \return 0 on success
 */
// rtlsdr_set_tuner_gain_mode(rtlsdr_dev_t *dev, int manual);

    //RTLSDR_API int rtlsdr_set_tuner_gain(rtlsdr_dev_t * dev, int gain);

    rtlsdr_set_sample_rate(device, 1.2e6);
    std::cout << "Sample Rate:" << rtlsdr_get_sample_rate(device) << std::endl;

    std::cout << "Offset Tuning:" << rtlsdr_get_offset_tuning(device) << std::endl;

}

int RtlSdr::close(){
    std::cout << "closing rtlsdr" << std::endl;
    bin_file.close();

    if (device) rtlsdr_close(device);
    device=nullptr;

    return 0;
}

void RtlSdr::toggleSave() {
    save_mode = !save_mode;

    if(save_mode){
        bin_file.close();
        checkbox2->setDisabled(true);
    }
    else {
        checkbox2->setDisabled(false);
        if (truncate)
            bin_file.open(file_name, std::ios::trunc | std::ios::out | std::ios::binary);
        else
            bin_file.open(file_name, std::ios::app | std::ios::out | std::ios::binary);
    }
}

    // data = np.ctypeslib.as_array(bytes)
    // iq = data.astype(np.float64).view(np.complex128)
    // iq /= 127.5
    // iq -= (1 + 1j)

    // iq = [complex(i/(255/2) - 1, q/(255/2) - 1) for i, q in izip(bytes[::2], bytes[1::2])]


// bias tee
// read, wait, cancel async
// direct sampling

std::unique_ptr<QLayout> RtlSdr::controlWidget() {

    auto layout = std::make_unique<QFormLayout>();

    checkbox1 = std::make_unique<QCheckBox>("Save IQ data to file.");
    checkbox2 = std::make_unique<QCheckBox>("Append to the file.");
    checkbox1->setChecked(true);
    checkbox2->setChecked(true);

    QCheckBox::connect(checkbox1.get(), &QCheckBox::clicked, layout.get(), [=]{
        toggleSave();
    });

    QCheckBox::connect(checkbox2.get(), &QCheckBox::clicked, layout.get(), [=]{
        truncate=!truncate;
        toggleSave();
        toggleSave();
    });

    layout->addRow(checkbox1.get());

    return layout;
}
