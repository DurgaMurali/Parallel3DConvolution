#include "Convolution.h"

void Convolution::setData(std::string inFilename, int inHeight, int inWidth, int inDepth, std::string kFilename, int kHeight, int kWidth, int kDepth, std::string outFile)
{
    this->inFilename = inFilename;
    this->inHeight = inHeight;
    this->inWidth = inWidth;
    this->inDepth = inDepth;
    this->kFilename = kFilename;
    this->kHeight = kHeight;
    this->kWidth = kWidth;
    this->kDepth = kDepth;
    this->outFile = outFile;
}
