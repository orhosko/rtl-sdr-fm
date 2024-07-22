#include "rawdataprovider.h"
#include "qfiledialog.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qspinbox.h"
#include <complex>
#include <iostream>
#include <string>
#include <vector>

// RawDataProvider::RawDataProvider() noexcept {}

FileReader::FileReader() {

    auto *file_label = new QLabel(file_name.c_str());

    auto *file_button = new QPushButton("Select File");
    // ui->addWidget(random_button);
    QAbstractButton::connect(file_button, &QPushButton::clicked, [=](){
        auto fileName = QFileDialog::getOpenFileName(file_button, "Open Raw", "/", "Any files (*)");
        std::cout << fileName.toStdString() << std::endl;

        file_name = fileName.toStdString();
        file_label->setText(file_name.c_str());
        this->close();
        this->open();
    });

    ui->addRow(file_label);
    ui->addRow("Select a file:", file_button);

    auto sample_rate_spinbox = new QSpinBox();
    sample_rate_spinbox->setSuffix("k");
    sample_rate_spinbox->setMaximum(9'000);
    sample_rate_spinbox->setMinimum(100);
    sample_rate_spinbox->setSingleStep(100);

    QAbstractSpinBox::connect(sample_rate_spinbox, &QSpinBox::valueChanged, [this](int newValue) -> void {
        setSampleRate(newValue * 1000);
    });

    ui->addRow("Sample Rate:", sample_rate_spinbox);
    emit sample_rate_spinbox->valueChanged(2'400);
    // sample_rate_spinbox->setValue(2'400);
}

int FileReader::open() {
    if(file.is_open()) {
        std::cout << "A file is already open." << std::endl;
        return 1;
    }

    file.open(file_name, std::ios::binary);

    if ( !file.is_open() ) {
        std::cout << "Cannot open the file." << std::endl;
        return 1;
    }

    std::cout << "file opened" << std::endl;
    return 0;
}

std::vector<std::complex<float>> FileReader::readData() {
    std::vector<std::complex<float>> data;

    float f;
    int i = 0;
    std::vector<float> m;
    while (file.read(reinterpret_cast<char*>(&f), sizeof(float))){
        m.push_back(f);

        i++;
        if(i >= 1'200'000) break;
    }

    for(int i=0; i<m.size(); i+=2){
        std::complex<float> temp(m[i],m[i+1]);
        data.push_back(temp);
    }

    std::cout << "test data produced" << data.size() << std::endl;
    return data;
}

auto FileReader::close() -> int {
    file.close();
    return 0;
}
