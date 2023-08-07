#include "Convolution.h"
#include <time.h>

class SeqConvolution : public Convolution
{
    public:
    void convSetup();
    template<typename T>
    void Convolution3D(const std::vector<std::vector<std::vector<T>>> &imgMatrix, const std::vector<std::vector<std::vector<T>>> &kernelMatrix, std::vector<std::vector<std::vector<T>>> &outputMatrix, std::string outFile);
};