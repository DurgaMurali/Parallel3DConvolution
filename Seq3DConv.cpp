
#include "Seq3DConv.h"

void SeqConvolution::setData(std::string inFilename, int inHeight, int inWidth, int inDepth, std::string kFilename, int kHeight, int kWidth, int kDepth, std::string outFile)
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

void SeqConvolution::convSetup()
{
    ReadMatrix readFile;
    int datatype = readFile.findFileDatatype(inFilename);

    switch(datatype)
    {
        case dataType::INT:
        {
            std::vector<std::vector<std::vector<int>>> imgMatrix(inDepth, std::vector<std::vector<int>>(inHeight, std::vector<int>(inWidth)));
            readFile.readMatrixFromFile<int>(inFilename, inHeight, inWidth, inDepth, imgMatrix);
            std::vector<std::vector<std::vector<int>>> kernelMatrix(kDepth, std::vector<std::vector<int>>(kHeight, std::vector<int>(kWidth)));
            readFile.readMatrixFromFile<int>(kFilename, kHeight, kWidth, kDepth, kernelMatrix);

            std::vector<std::vector<std::vector<int>>> outputMatrix(inDepth, std::vector<std::vector<int>>(inHeight, std::vector<int>(inWidth)));
            std::cout << "Convolve" << std::endl;
            clock_t time = clock();
            Convolution3D<int>(imgMatrix, kernelMatrix, outputMatrix, outFile);
            time = clock() - time;
            std::cout << "Time = " << time << " ms" << std::endl;


            break;
        }

        case dataType::DOUBLE:
        {
            std::vector<std::vector<std::vector<double>>> imgMatrix(inDepth, std::vector<std::vector<double>>(inHeight, std::vector<double>(inWidth)));
            readFile.readMatrixFromFile<double>(inFilename, inHeight, inWidth, inDepth, imgMatrix);
            std::vector<std::vector<std::vector<double>>> kernelMatrix(kDepth, std::vector<std::vector<double>>(kHeight, std::vector<double>(kWidth)));
            readFile.readMatrixFromFile<double>(kFilename, kHeight, kWidth, kDepth, kernelMatrix);
            
            std::vector<std::vector<std::vector<double>>> outputMatrix(inDepth, std::vector<std::vector<double>>(inHeight, std::vector<double>(inWidth)));
            std::cout << "Convolve" << std::endl;
            clock_t time = clock();
            Convolution3D<double>(imgMatrix, kernelMatrix, outputMatrix, outFile);
            time = clock() - time;
            std::cout << "Time = " << time << " ms" << std::endl;

            break;
        }
         
    }
}

template<typename T>
void SeqConvolution::Convolution3D(const std::vector<std::vector<std::vector<T>>> imgMatrix, const std::vector<std::vector<std::vector<T>>> kernelMatrix, std::vector<std::vector<std::vector<T>>> &outputMatrix, std::string outFile)
{
    int depth, height, width;
    int padDepth = (kDepth-1)/2;
    int padHeight = (kHeight-1)/2;
    int padWidth = (kWidth-1)/2;

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
                            
                            if((depth < 0) || (height < 0) || (width < 0) || (depth >= inDepth) || (height >= inHeight) || (width >= inWidth))
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
    file.close();
}

int main(int argc, char *argv[])
{
    SeqConvolution conv;
    conv.setData(argv[1], std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]), argv[5], std::stoi(argv[6]), std::stoi(argv[7]), std::stoi(argv[8]), argv[9]);
    conv.convSetup();
    //int datatype = std::stoi(argv[10]);  
}