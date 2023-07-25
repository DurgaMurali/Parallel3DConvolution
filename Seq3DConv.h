#include "ReadMatrix.h"
#include <time.h>

class SeqConvolution
{
    std::string inFilename;
    std::string kFilename;
    std::string outFile;
    int inHeight;
    int inWidth;
    int inDepth;
    int kHeight;
    int kWidth;
    int kDepth;
    public:
    void setData(std::string inFilename, int inHeight, int inWidth, int inDepth, std::string kFilename, int kHeight, int kWidth, int kDepth, std::string outFile);
    void convSetup();
    template<typename T>
    void Convolution3D(const std::vector<std::vector<std::vector<T>>> imgMatrix, const std::vector<std::vector<std::vector<T>>> kernelMatrix, std::vector<std::vector<std::vector<T>>> &outputMatrix, std::string outFile);
};