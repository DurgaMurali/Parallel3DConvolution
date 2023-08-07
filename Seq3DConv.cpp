
#include "Seq3DConv.h"

/*void SeqConvolution::setData(std::string inFilename, int inHeight, int inWidth, int inDepth, std::string kFilename, int kHeight, int kWidth, int kDepth, std::string outFile)
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
}*/

void SeqConvolution::convSetup()
{
    ReadMatrix readFile;
    int datatype = readFile.findFileDatatype(inFilename);

    switch(datatype)
    {
        case dataType::TYPE_INT:
        {
            std::vector<std::vector<std::vector<int>>> imgMatrix(inDepth, std::vector<std::vector<int>>(inHeight, std::vector<int>(inWidth)));
            readFile.readMatrixFromFile<int>(inFilename, inHeight, inWidth, inDepth, imgMatrix);
            std::vector<std::vector<std::vector<int>>> kernelMatrix(kDepth, std::vector<std::vector<int>>(kHeight, std::vector<int>(kWidth)));
            readFile.readMatrixFromFile<int>(kFilename, kHeight, kWidth, kDepth, kernelMatrix);

            std::vector<std::vector<std::vector<int>>> outputMatrix(inDepth, std::vector<std::vector<int>>(inHeight, std::vector<int>(inWidth)));
            //std::cout << "Convolve" << std::endl;
            clock_t time = clock();
            Convolution3D<int>(imgMatrix, kernelMatrix, outputMatrix, outFile);
            time = clock() - time;
            std::cout << "Time = " << time << " ms" << std::endl;
        
            writeToFile<int>(outputMatrix);

            break;
        }

        case dataType::TYPE_FLOAT:
        {
            std::vector<std::vector<std::vector<float>>> imgMatrix(inDepth, std::vector<std::vector<float>>(inHeight, std::vector<float>(inWidth)));
            readFile.readMatrixFromFile<float>(inFilename, inHeight, inWidth, inDepth, imgMatrix);
            std::vector<std::vector<std::vector<float>>> kernelMatrix(kDepth, std::vector<std::vector<float>>(kHeight, std::vector<float>(kWidth)));
            readFile.readMatrixFromFile<float>(kFilename, kHeight, kWidth, kDepth, kernelMatrix);
            
            std::vector<std::vector<std::vector<float>>> outputMatrix(inDepth, std::vector<std::vector<float>>(inHeight, std::vector<float>(inWidth)));
            //std::cout << "Convolve" << std::endl;
            clock_t time = clock();
            Convolution3D<float>(imgMatrix, kernelMatrix, outputMatrix, outFile);
            time = clock() - time;
            std::cout << "Time = " << time << " ms" << std::endl;
        
            writeToFile<float>(outputMatrix);

            break;
        }
         
    }
}

template<typename T>
void SeqConvolution::Convolution3D(const std::vector<std::vector<std::vector<T>>> &imgMatrix, const std::vector<std::vector<std::vector<T>>> &kernelMatrix, std::vector<std::vector<std::vector<T>>> &outputMatrix, std::string outFile)
{
    int depth, height, width;
    int padDepth = (kDepth-1)/2;
    int padHeight = (kHeight-1)/2;
    int padWidth = (kWidth-1)/2;

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
}

/*template <typename T>
void SeqConvolution::writeToFile(const std::vector<std::vector<std::vector<T>>> &outputMatrix)
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
}*/

int main(int argc, char *argv[])
{
    std::cout << "Sequential Convolution" << std::endl;
    SeqConvolution conv;
    conv.setData(argv[1], std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]), argv[5], std::stoi(argv[6]), std::stoi(argv[7]), std::stoi(argv[8]), argv[9]);
    conv.convSetup(); 
}