
#include "ReadMatrix.h"
#include <time.h>

int inHeight;
int inWidth;
int inDepth;
int kHeight;
int kWidth;
int kDepth;

enum dataType
{
    INT = 1,
    FLOAT = 2,
    DOUBLE = 3
};

template<typename T>
void Sequential3DConvolution(std::vector<std::vector<std::vector<T>>> imgMatrix, std::vector<std::vector<std::vector<T>>> kernelMatrix, std::vector<std::vector<std::vector<T>>> &outputMatrix, std::string outFile, bool padding)
{
    int depth, height, width;
    int padDepth = (kDepth-1)/2;
    int padHeight = (kHeight-1)/2;
    int padWidth = (kWidth-1)/2;

    if(!padding)
    {
        padDepth = 0;
        padHeight = 0;
        padWidth = 0;
    }
    std::cout << "padded height = " << padHeight << ", padded width = " << padWidth << std::endl;

    for(int i=0; i<inDepth; ++i)
    {
        for(int j=0; j<inHeight; ++j)
        {
            for(int k=0; k<inWidth; ++k)
            {
                T sum = 0;

                for(int x=0; x<kDepth; ++x)
                {
                    for(int y=0; y<kHeight; ++y)
                    {
                        for(int z=0; z<kWidth; ++z)
                        {
                            depth = i + (x - padDepth);
                            height = j + (y - padHeight);
                            width = k + (z - padWidth);
                            
                            if(padding && ((depth < 0) || (height < 0) || (width < 0) || (depth >= inDepth) || (height >= inHeight) || (width >= inWidth)))
                                sum += 0;
                            else
                                sum += imgMatrix[depth][height][width] * kernelMatrix[x][y][z];
                        }
                    }
                }
                outputMatrix[i][j][k] = sum;
            }
        }
    }

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
}

int main(int argc, char *argv[])
{
    std::string inFilename = argv[1];
    inHeight = std::stoi(argv[2]); //rows
    inWidth = std::stoi(argv[3]); //columns
    inDepth = std::stoi(argv[4]);

    std::string kFilename = argv[5];
    kHeight = std::stoi(argv[6]);
    kWidth = std::stoi(argv[7]);
    kDepth = std::stoi(argv[8]);

    std::string outFile = argv[9];
    int datatype = std::stoi(argv[10]);

    switch(datatype)
    {
        case dataType::INT:
        {
            std::vector<std::vector<std::vector<int>>> imgMatrix(inDepth, std::vector<std::vector<int>>(inHeight, std::vector<int>(inWidth)));
            readMatrixFromFile(inFilename, inHeight, inWidth, inDepth, imgMatrix);
            std::vector<std::vector<std::vector<int>>> kernelMatrix(kDepth, std::vector<std::vector<int>>(kHeight, std::vector<int>(kWidth)));
            readMatrixFromFile(kFilename, kHeight, kWidth, kDepth, kernelMatrix);
            
            std::vector<std::vector<std::vector<int>>> outputMatrix(inDepth, std::vector<std::vector<int>>(inHeight, std::vector<int>(inWidth)));
            std::cout << "Convolve" << std::endl;
            clock_t time = clock();
            Sequential3DConvolution<int>(imgMatrix, kernelMatrix, outputMatrix, outFile, true);
            time = clock() - time;
            std::cout << "Time = " << time << " ms" << std::endl;

            break;
        }

        case dataType::FLOAT:
        {
            std::vector<std::vector<std::vector<float>>> imgMatrix(inDepth, std::vector<std::vector<float>>(inHeight, std::vector<float>(inWidth)));
            readMatrixFromFile(inFilename, inHeight, inWidth, inDepth, imgMatrix);
            std::vector<std::vector<std::vector<float>>> kernelMatrix(kDepth, std::vector<std::vector<float>>(kHeight, std::vector<float>(kWidth)));
            readMatrixFromFile(kFilename, kHeight, kWidth, kDepth, kernelMatrix);
            
            std::vector<std::vector<std::vector<float>>> outputMatrix(inDepth, std::vector<std::vector<float>>(inHeight, std::vector<float>(inWidth)));
            std::cout << "Convolve" << std::endl;
            clock_t time = clock();
            Sequential3DConvolution<float>(imgMatrix, kernelMatrix, outputMatrix, outFile, true);
            time = clock() - time;
            std::cout << "Time = " << time << " ms" << std::endl;

            break;   
        }

        case dataType::DOUBLE:
        {
            std::vector<std::vector<std::vector<double>>> imgMatrix(inDepth, std::vector<std::vector<double>>(inHeight, std::vector<double>(inWidth)));
            readMatrixFromFile(inFilename, inHeight, inWidth, inDepth, imgMatrix);
            std::vector<std::vector<std::vector<double>>> kernelMatrix(kDepth, std::vector<std::vector<double>>(kHeight, std::vector<double>(kWidth)));
            readMatrixFromFile(kFilename, kHeight, kWidth, kDepth, kernelMatrix);
            
            std::vector<std::vector<std::vector<double>>> outputMatrix(inDepth, std::vector<std::vector<double>>(inHeight, std::vector<double>(inWidth)));
            std::cout << "Convolve" << std::endl;
            clock_t time = clock();
            Sequential3DConvolution<double>(imgMatrix, kernelMatrix, outputMatrix, outFile, true);
            time = clock() - time;
            std::cout << "Time = " << time << " ms" << std::endl;
            
            break;
        }
         
    }
}