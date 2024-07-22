#include "fm_demod.h"
#include "Filters/SOSFilter.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>
#include <complex>
#include <fstream>

FM_Demod::FM_Demod() {
    //TODO: big sin
    file.open("test.raw", std::ios::out | std::ios::trunc | std::ios::binary);

    if(!file.is_open()) std::cout << "Couldn't open test.raw" << std::endl;
}
auto FM_Demod::aa(std::vector<std::complex<float>> data, int fs) -> std::vector<float> {
    Fs=fs;
    return aa(std::move(data));
}

auto FM_Demod::aa(std::vector<std::complex<float>> data) -> std::vector<float> {

    // if(Fs_ref != nullptr)
    // Fs=*Fs_ref;

    int M = 0;
    // std::cout << "x2:" << data[M] << " " << data[M+1] << std::endl;

    // int Fc = 104'300'000;
    int N = data.size();
    if (N==0) {
        std::cout << "FM Demod: no data." << std::endl;
        return {};
    }

    int f_bw = 200000;
    int dec_rate = Fs / f_bw;

    // std::cout << "dec_rate:" << dec_rate << std::endl;

    // x4 = signal.decimate(x2, dec_rate);
    std::vector<std::complex<double>> decimated;
    for(int i=0; i < N; i++){
        if(i%dec_rate==0) decimated.emplace_back(data[i]);
    }

    int Fs_y = Fs/dec_rate;
    N /= dec_rate;

    // std::cout << "x4:" << decimated[M] << " " << decimated[M+1] << " N: " << N << std::endl;
    // Polar discriminator
    // auto x5 = std::arg(decimated[1:]*std::conj(x4[:-1]));

    std::vector<double> x5(decimated.size());
    for(int i=0; i< N-1 ; i++) {
        x5[i] = std::arg(decimated[i+1] * std::conj(decimated[i]));
    }

    // std::cout << "x5:" << x5[M] << " " << x5[M+1] << std::endl;

    // x5.pop_back();

    float tau = 50e-6;
    // The de-emphasis filter
    double d = Fs_y * tau;
    double x = std::exp(-1/d);   // Calculate the decay between each sample
    //auto b = [1-x];          // Create the filter coefficients
    //auto a = [1,-x];

    // std::cout << "x:" << x << std::endl;

    SOSFilter<1> filter({{1-x,0,0,
                          1,  -x, 0}},{0.3});

    // ------------------------------------------------
    auto x6 = x5;
    for(int i=0; i < x5.size(); i++){
        x6[i]= filter.filter(x5[i]);
    }

    // std::cout << "x6:" << x6[M] << " " << x6[M+1] << " " << x6.size() << std::endl;

    auto audio_freq = 40000.0;
    auto dec_audio = static_cast<int>(Fs_y/audio_freq);
    // auto Fs_audio = Fs_y / dec_audio;

    std::vector<float> x7;
    for(int i=0; i < N; i++){
        if(i%dec_audio==0) x7.push_back(x6[i]);
    }
    N /= dec_audio;

    data_N = N;

    double max=0;
    for(auto v: x7){
        if (std::abs(v)>max) max = std::abs(v);
    }

    // for(auto &v: x7){
    //     v *= 10000 / max;
    // }

    std::cout << "x7:" << x7[M] << " " << x7[M+1] << " " << x7.size() << std::endl;

    for(auto e:x7){
        float d = e * 10000;
        file.write(reinterpret_cast<char*>(&d), sizeof(d));
    }

    return x7;
}

FM_Demod::~FM_Demod(){
    file.close();
    fin.close();
}
