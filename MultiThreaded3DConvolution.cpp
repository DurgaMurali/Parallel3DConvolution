#define _WIN32_WINNT 0x0501
#include "MultiThreaded3DConvolution.h"

int main(int argc, char *argv[])
{
    MultiThreaded3DConvolution conv;
    conv.setData(argv[1], std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]), argv[5], std::stoi(argv[6]), std::stoi(argv[7]), std::stoi(argv[8]), argv[9], std::stoi(argv[10]));
    conv.setup();
}

void MultiThreaded3DConvolution::setData(std::string inFilename, int inHeight, int inWidth, int inDepth, std::string kFilename, int kHeight, int kWidth, int kDepth, std::string outFile, int numThreads)
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
    this->numThreads = numThreads;
}

void MultiThreaded3DConvolution::setup()
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
            std::cout << "Convolve" << std::endl;
            
             createThreads(&imgMatrix, &kernelMatrix, &outputMatrix);

            break;
        }

        case dataType::TYPE_FLOAT:
        {
            std::vector<std::vector<std::vector<float>>> imgMatrix(inDepth, std::vector<std::vector<float>>(inHeight, std::vector<float>(inWidth)));
            readFile.readMatrixFromFile<float>(inFilename, inHeight, inWidth, inDepth, imgMatrix);
            std::vector<std::vector<std::vector<float>>> kernelMatrix(kDepth, std::vector<std::vector<float>>(kHeight, std::vector<float>(kWidth)));
            readFile.readMatrixFromFile<float>(kFilename, kHeight, kWidth, kDepth, kernelMatrix);
            
            std::vector<std::vector<std::vector<float>>> outputMatrix(inDepth, std::vector<std::vector<float>>(inHeight, std::vector<float>(inWidth)));
            std::cout << "Convolve" << std::endl;
            
            createThreads(&imgMatrix, &kernelMatrix, &outputMatrix);

            break;
        }
         
    }
}

void MultiThreaded3DConvolution::createThreads(const std::vector<std::vector<std::vector<int>>> *imgMatrix, const std::vector<std::vector<std::vector<int>>> *kernelMatrix, std::vector<std::vector<std::vector<int>>> *outputMatrix)
{
    int factor = inWidth/numThreads;
    std::cout << "Thread count = " << numThreads << ", factor = "<< factor <<std::endl;

    for(int i=0; i<numThreads; ++i)
    {
        pthread_t threads[numThreads];
        threadArgs_I t_arg[numThreads];
        for (int j=0; j<numThreads; ++j)
        {
            int startRowIndex = i * factor;
            int startColIndex = j * factor;

            t_arg[j] = createThreadArgs(imgMatrix, kernelMatrix, outputMatrix, startRowIndex, startColIndex, factor);

            if(pthread_create(&threads[j], NULL, MultiThreaded3DConvolution::Threaded3DConvolution_I, &t_arg[j]) != 0)
                std::cout << "Error creating thread " << j << std::endl;

        }

        for(int j=0; j<numThreads; ++j)
        {
            if(pthread_join(threads[j], NULL) != 0)
                std::cout << "Error in thread join " << j << std::endl;
        }
    }
}

void MultiThreaded3DConvolution::createThreads(const std::vector<std::vector<std::vector<float>>> *imgMatrix, const std::vector<std::vector<std::vector<float>>> *kernelMatrix, std::vector<std::vector<std::vector<float>>> *outputMatrix)
{
    int factor = inWidth/numThreads;
    std::cout << "Thread count = " << numThreads << ", factor = "<< factor <<std::endl;
    clock_t time = clock();

    for(int i=0; i<numThreads; ++i)
    {
        pthread_t threads[numThreads];
        threadArgs_D t_arg[numThreads];
        for (int j=0; j<numThreads; ++j)
        {
            int startRowIndex = i * factor;
            int startColIndex = j * factor;

            t_arg[j] = createThreadArgs(imgMatrix, kernelMatrix, outputMatrix, startRowIndex, startColIndex, factor);

            
            if(pthread_create(&threads[j], NULL, MultiThreaded3DConvolution::Threaded3DConvolution_F, &t_arg[j]) != 0)
                std::cout << "Error creating thread " << j << std::endl;

        }

        for(int j=0; j<numThreads; ++j)
        {
            if(pthread_join(threads[j], NULL) != 0)
                std::cout << "Error in thread join " << j << std::endl;
        }
    }
    time = clock() - time;
    std::cout << "Time = " << time << " ms" << std::endl;

    std::cout << "Writing output to file" << std::endl;
    std::ofstream file(outFile);
    for(int i=0; i<inDepth; i++)
    {
        for(int j=0; j<inHeight; ++j)
        {
            for(int k=0; k<inWidth; ++k)
            {
                file << (*outputMatrix)[i][j][k] << " ";
            }
            file << std::endl;
        }
        file << std::endl;
    }
}


threadArgs_I MultiThreaded3DConvolution::createThreadArgs(const std::vector<std::vector<std::vector<int>>> *imgMatrix, const std::vector<std::vector<std::vector<int>>> *kernelMatrix, std::vector<std::vector<std::vector<int>>> *outputMatrix, int startRowIndex, int startColIndex, int factor)
{
    threadArgs_I t_arg;
    t_arg.kDepth = kDepth;
    t_arg.kHeight = kHeight;
    t_arg.kWidth = kWidth;
    t_arg.inDepth = inDepth;
    t_arg.inHeight = inHeight;
    t_arg.inWidth = inWidth;
    t_arg.startRowIndex = startRowIndex;
    t_arg.startColIndex = startColIndex;
    t_arg.factor = factor;
    t_arg.imgMatrix = imgMatrix;
    t_arg.kernelMatrix = kernelMatrix;
    t_arg.outputMatrix = outputMatrix;

    return t_arg;
}

threadArgs_D MultiThreaded3DConvolution::createThreadArgs(const std::vector<std::vector<std::vector<float>>> *imgMatrix, const std::vector<std::vector<std::vector<float>>> *kernelMatrix, std::vector<std::vector<std::vector<float>>> *outputMatrix, int startRowIndex, int startColIndex, int factor)
{
    threadArgs_D t_arg;
    t_arg.kDepth = kDepth;
    t_arg.kHeight = kHeight;
    t_arg.kWidth = kWidth;
    t_arg.inDepth = inDepth;
    t_arg.inHeight = inHeight;
    t_arg.inWidth = inWidth;
    t_arg.startRowIndex = startRowIndex;
    t_arg.startColIndex = startColIndex;
    t_arg.factor = factor;
    t_arg.imgMatrix = imgMatrix;
    t_arg.kernelMatrix = kernelMatrix;
    t_arg.outputMatrix = outputMatrix;

    return t_arg;
}


void* MultiThreaded3DConvolution::Threaded3DConvolution_I(void* arg)
{
    threadArgs_I* t_args = static_cast<threadArgs_I*>(arg);

    int kDepth = t_args->kDepth;
    int kHeight = t_args->kHeight;
    int kWidth = t_args->kWidth;
    int inDepth = t_args->inDepth;
    int inHeight = t_args->inHeight;
    int inWidth = t_args->inWidth;
    int startRowIndex = t_args->startRowIndex; 
    int startColIndex = t_args->startColIndex;
    int factor = t_args->factor;

    const std::vector<std::vector<std::vector<int>>> *imgMatrix = t_args->imgMatrix;
    const std::vector<std::vector<std::vector<int>>> *kernelMatrix = t_args->kernelMatrix;
    std::vector<std::vector<std::vector<int>>> *outputMatrix = t_args->outputMatrix;
    Convolution3D<int>(imgMatrix, kernelMatrix, outputMatrix, kDepth, kHeight, kWidth, inDepth, inHeight, inWidth, startRowIndex, startColIndex, factor);

    return NULL;
}

void* MultiThreaded3DConvolution::Threaded3DConvolution_F(void* arg)
{
    threadArgs_D* t_args = static_cast<threadArgs_D*>(arg);

    int kDepth = t_args->kDepth;
    int kHeight = t_args->kHeight;
    int kWidth = t_args->kWidth;
    int inDepth = t_args->inDepth;
    int inHeight = t_args->inHeight;
    int inWidth = t_args->inWidth;
    int startRowIndex = t_args->startRowIndex; 
    int startColIndex = t_args->startColIndex;
    int factor = t_args->factor;

    const std::vector<std::vector<std::vector<float>>> *imgMatrix = t_args->imgMatrix;
    const std::vector<std::vector<std::vector<float>>> *kernelMatrix = t_args->kernelMatrix;
    std::vector<std::vector<std::vector<float>>> *outputMatrix = t_args->outputMatrix;
    Convolution3D<float>(imgMatrix, kernelMatrix, outputMatrix, kDepth, kHeight, kWidth, inDepth, inHeight, inWidth, startRowIndex, startColIndex, factor);

    return NULL;
}

template <typename T>
void MultiThreaded3DConvolution::Convolution3D(const std::vector<std::vector<std::vector<T>>> *imgMatrix, const std::vector<std::vector<std::vector<T>>> *kernelMatrix, std::vector<std::vector<std::vector<T>>> *outputMatrix, int kDepth, int kHeight, int kWidth, int inDepth, int inHeight, int inWidth, int startRowIndex, int startColIndex, int factor)
{
    int depth, height, width;
    int padDepth = (kDepth-1)/2;
    int padHeight = (kHeight-1)/2;
    int padWidth = (kWidth-1)/2;

    for(int i=0; i<inDepth; ++i)
    {
        for(int j=startRowIndex; j<startRowIndex+factor; ++j)
        {
            for(int k=startColIndex; k<startColIndex+factor; ++k)
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
                            
                            if((depth >= 0) && (height >= 0) && (width >= 0) && (depth < inDepth) && (height < inHeight) && (width < inWidth))
                            { 
                                sum += (*imgMatrix)[depth][height][width] * (*kernelMatrix)[x][y][z];
                            }
                        }
                    }
                }
                (*outputMatrix)[i][j][k] = sum;
            }
        }
    }

}