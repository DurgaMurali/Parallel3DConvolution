#include <pthread.h>
#include "ReadMatrix.h"
#include <tuple>

struct threadArgs_I
{
    int kDepth = 0;
    int kHeight = 0;
    int kWidth = 0;
    int inDepth = 0;
    int inHeight = 0;
    int inWidth = 0;
    int startRowIndex = 0; 
    int startColIndex = 0;
    int factor = 0;
    const std::vector<std::vector<std::vector<int>>> *imgMatrix = NULL;
    const std::vector<std::vector<std::vector<int>>> *kernelMatrix = NULL;
    std::vector<std::vector<std::vector<int>>> *outputMatrix = NULL;
};

struct threadArgs_D
{
    int kDepth = 0;
    int kHeight = 0;
    int kWidth = 0;
    int inDepth = 0;
    int inHeight = 0;
    int inWidth = 0;
    int startRowIndex = 0; 
    int startColIndex = 0;
    int factor = 0;
    const std::vector<std::vector<std::vector<float>>> *imgMatrix = NULL;
    const std::vector<std::vector<std::vector<float>>> *kernelMatrix = NULL;
    std::vector<std::vector<std::vector<float>>> *outputMatrix = NULL;
};

class MultiThreaded3DConvolution
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
    int numThreads;
    public:
    void setData(std::string inFilename, int inHeight, int inWidth, int inDepth, std::string kFilename, int kHeight, int kWidth, int kDepth, std::string outFile, int numThreads);
    void setup();
    void createThreads(const std::vector<std::vector<std::vector<int>>> *imgMatrix, const std::vector<std::vector<std::vector<int>>> *kernelMatrix, std::vector<std::vector<std::vector<int>>> *outputMatrix);
    void createThreads(const std::vector<std::vector<std::vector<float>>> *imgMatrix, const std::vector<std::vector<std::vector<float>>> *kernelMatrix, std::vector<std::vector<std::vector<float>>> *outputMatrix);
    static void* Threaded3DConvolution_I(void* arg);
    static void* Threaded3DConvolution_F(void* arg);
    threadArgs_I createThreadArgs(const std::vector<std::vector<std::vector<int>>> *imgMatrix, const std::vector<std::vector<std::vector<int>>> *kernelMatrix, std::vector<std::vector<std::vector<int>>> *outputMatrix, int startRowIndex, int startColIndex, int factor);
    threadArgs_D createThreadArgs(const std::vector<std::vector<std::vector<float>>> *imgMatrix, const std::vector<std::vector<std::vector<float>>> *kernelMatrix, std::vector<std::vector<std::vector<float>>> *outputMatrix, int startRowIndex, int startColIndex, int factor);
    template <typename T>
    static void Convolution3D(const std::vector<std::vector<std::vector<T>>> *imgMatrix, const std::vector<std::vector<std::vector<T>>> *kernelMatrix, std::vector<std::vector<std::vector<T>>> *outputMatrix, int kDepth, int kHeight, int kWidth, int inDepth, int inHeight, int inWidth, int startRowIndex, int startColIndex, int factor);
};