#ifndef FM_DEMOD_H
#define FM_DEMOD_H

#include <complex>
#include <vector>
#include <fstream>
class FM_Demod
{
public:
    FM_Demod();
    ~FM_Demod();
    std::vector<float> aa();
    // std::vector<float> aa(std::vector<std::complex<float>> &data);
    std::vector<float> aa(std::vector<std::complex<float>> data);
    std::vector<float> aa(std::vector<std::complex<float>> data, int fs);
    void setFs(int f) {Fs=f;}
    // void setFs_ref(int *f) {Fs_ref=f;}

    int data_N=0;
    std::ifstream fin;
private:
    // int aa();
    std::fstream file;
    int Fs = 1'200'000;    //Sample rate
    int *Fs_ref = nullptr;
};

#endif // FM_DEMOD_H
