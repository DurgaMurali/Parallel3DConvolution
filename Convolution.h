#include "ReadMatrix.h"

class Convolution
{
    protected:
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
    template <typename T>
    void writeToFile(const std::vector<std::vector<std::vector<T>>> &outputMatrix)
    {
        //std::cout << "Writing output to file" << std::endl;
        std::ofstream file(outFile);
        for(int i=0; i<inDepth; i++)
        {
            for(int j=0; j<inHeight; j++)
            {
                for(int k=0; k<inWidth; k++)
                {
                    file<<outputMatrix[i][j][k]<<" ";
                }
                file<<std::endl;
            }
            file<<std::endl;
        }
        file.close();
    }
};

