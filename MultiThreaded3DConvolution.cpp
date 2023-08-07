#define _WIN32_WINNT 0x0501
#include "MultiThreaded3DConvolution.h"

int main(int argc, char *argv[])
{
    std::cout << "Multithreaded Convolution" << std::endl;
    MultiThreaded3DConvolution conv;
    conv.setData(argv[1], std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]), argv[5], std::stoi(argv[6]), std::stoi(argv[7]), std::stoi(argv[8]), argv[9], std::stoi(argv[10]));
    conv.setup();
}

void MultiThreaded3DConvolution::setData(std::string inFilename, int inHeight, int inWidth, int inDepth, std::string kFilename, int kHeight, int kWidth, int kDepth, std::string outFile, int numThreads)
{
    Convolution::setData(inFilename, inHeight, inWidth, inDepth, kFilename, kHeight, kWidth, kDepth, outFile);
    this->numThreads = numThreads;
}

void MultiThreaded3DConvolution::setup()
{
    ReadMatrix readFile;
    int datatype = readFile.findFileDatatype(inFilename);

    switch(datatype)
    {
        case dataType::TYPE_FLOAT:
        {
            std::vector<std::vector<std::vector<float>>> imgMatrix(inDepth, std::vector<std::vector<float>>(inHeight, std::vector<float>(inWidth)));
            readFile.readMatrixFromFile<float>(inFilename, inHeight, inWidth, inDepth, imgMatrix);
            std::vector<std::vector<std::vector<float>>> kernelMatrix(kDepth, std::vector<std::vector<float>>(kHeight, std::vector<float>(kWidth)));
            readFile.readMatrixFromFile<float>(kFilename, kHeight, kWidth, kDepth, kernelMatrix);
            
            std::vector<std::vector<std::vector<float>>> outputMatrix(inDepth, std::vector<std::vector<float>>(inHeight, std::vector<float>(inWidth)));
            //std::cout << "Convolve" << std::endl;
            
            createThreads(&imgMatrix, &kernelMatrix, &outputMatrix);
            writeToFile<float>(outputMatrix);

            break;
        }
          
        case dataType::TYPE_INT:
        {
            std::vector<std::vector<std::vector<int>>> imgMatrix(inDepth, std::vector<std::vector<int>>(inHeight, std::vector<int>(inWidth)));
            readFile.readMatrixFromFile<int>(inFilename, inHeight, inWidth, inDepth, imgMatrix);
            std::vector<std::vector<std::vector<int>>> kernelMatrix(kDepth, std::vector<std::vector<int>>(kHeight, std::vector<int>(kWidth)));
            readFile.readMatrixFromFile<int>(kFilename, kHeight, kWidth, kDepth, kernelMatrix);

            std::vector<std::vector<std::vector<int>>> outputMatrix(inDepth, std::vector<std::vector<int>>(inHeight, std::vector<int>(inWidth)));
            //std::cout << "Convolve" << std::endl;
            
            createThreads(&imgMatrix, &kernelMatrix, &outputMatrix);
            writeToFile<int>(outputMatrix);

            break;
        }
    }
}

void MultiThreaded3DConvolution::createThreads(const std::vector<std::vector<std::vector<int>>> *imgMatrix, const std::vector<std::vector<std::vector<int>>> *kernelMatrix, std::vector<std::vector<std::vector<int>>> *outputMatrix)
{
    int factor_w = inWidth/numThreads;
    int factor_h = inHeight/numThreads;
    int threads_h = numThreads;
    int threads_w = numThreads;
    
    if(inHeight%numThreads != 0)
        threads_h++;
    
    if(inWidth%numThreads != 0)
        threads_w++;

    std::cout << "Thread count = " << numThreads << ", factor_h = "<< factor_h << ", factor_w = " << factor_w << ", threads_h = " << threads_h << ", threads_w = " << threads_w << std::endl;
    clock_t time = clock();

    for(int i=0; i<threads_h; ++i)
    {
        int startRowIndex = i * factor_h;
        int endRowIndex = startRowIndex + factor_h;
        if(i == threads_h-1)
            endRowIndex = inHeight;

        pthread_t threads[threads_w];
        threadArgs_I t_arg[threads_w];

        for (int j=0; j<threads_w; ++j)
        {
            int startColIndex = j * factor_w;
            int endColIndex = startColIndex + factor_w;
            if(j == threads_w-1)
                endColIndex = inWidth;

            t_arg[j] = createThreadArgs(imgMatrix, kernelMatrix, outputMatrix, startRowIndex, endRowIndex, startColIndex, endColIndex);

            if(pthread_create(&threads[j], NULL, MultiThreaded3DConvolution::Threaded3DConvolution_I, &t_arg[j]) != 0)
                std::cout << "Error creating thread " << j << std::endl;

        }

        for(int j=0; j<threads_w; ++j)
        {
            if(pthread_join(threads[j], NULL) != 0)
                std::cout << "Error in thread join " << j << std::endl;
        }
    }
    time = clock() - time;
    std::cout << "Time = " << time << " ms" << std::endl;
}

void MultiThreaded3DConvolution::createThreads(const std::vector<std::vector<std::vector<float>>> *imgMatrix, const std::vector<std::vector<std::vector<float>>> *kernelMatrix, std::vector<std::vector<std::vector<float>>> *outputMatrix)
{
    int factor_w = inWidth/numThreads;
    int factor_h = inHeight/numThreads;
    int threads_h = numThreads;
    int threads_w = numThreads;

    if(inHeight%numThreads != 0)
        threads_h++;
    
    if(inWidth%numThreads != 0)
        threads_w++;
    
    std::cout << "Thread count = " << numThreads << ", factor_h = "<< factor_h << ", factor_w = " << factor_w << ", threads_h = " << threads_h << ", threads_w = " << threads_w << std::endl;
    clock_t time = clock();

    pthread_t threads[threads_w*threads_h];
    threadArgs_D t_arg[threads_w*threads_h];

    for(int i=0; i<threads_h; ++i)
    {
        int startRowIndex = i * factor_h;
        int endRowIndex = startRowIndex + factor_h;
        if(i == threads_h-1)
            endRowIndex = inHeight;

        //pthread_t threads[threads_w];
        //threadArgs_D t_arg[threads_w];
        for (int j=0; j<threads_w; ++j)
        {
            int startColIndex = j * factor_w;
            int endColIndex = startColIndex + factor_w;
            if(j == threads_w-1)
                endColIndex = inWidth;

            int thread_index = i*threads_h + j; 
            t_arg[thread_index] = createThreadArgs(imgMatrix, kernelMatrix, outputMatrix, startRowIndex, endRowIndex, startColIndex, endColIndex);
            
            if(pthread_create(&threads[thread_index], NULL, MultiThreaded3DConvolution::Threaded3DConvolution_F, &t_arg[thread_index]) != 0)
                std::cout << "Error creating thread " << j << std::endl;

        }

        /*for(int j=0; j<threads_w; ++j)
        {
            if(pthread_join(threads[j], NULL) != 0)
                std::cout << "Error in thread join " << j << std::endl;
        }*/
    }

    for(int j=0; j<threads_w*threads_h; ++j)
    {
        if(pthread_join(threads[j], NULL) != 0)
            std::cout << "Error in thread join " << j << std::endl;
    }

    time = clock() - time;
    std::cout << "Time = " << time << " ms" << std::endl;
}


threadArgs_I MultiThreaded3DConvolution::createThreadArgs(const std::vector<std::vector<std::vector<int>>> *imgMatrix, const std::vector<std::vector<std::vector<int>>> *kernelMatrix, std::vector<std::vector<std::vector<int>>> *outputMatrix, int startRowIndex, int endRowIndex, int startColIndex, int endColIndex)
{
    threadArgs_I t_arg;
    t_arg.kDepth = kDepth;
    t_arg.kHeight = kHeight;
    t_arg.kWidth = kWidth;
    t_arg.inDepth = inDepth;
    t_arg.inHeight = inHeight;
    t_arg.inWidth = inWidth;
    t_arg.startRowIndex = startRowIndex;
    t_arg.endRowIndex = endRowIndex;
    t_arg.startColIndex = startColIndex;
    t_arg.endColIndex = endColIndex;
    t_arg.imgMatrix = imgMatrix;
    t_arg.kernelMatrix = kernelMatrix;
    t_arg.outputMatrix = outputMatrix;

    return t_arg;
}

threadArgs_D MultiThreaded3DConvolution::createThreadArgs(const std::vector<std::vector<std::vector<float>>> *imgMatrix, const std::vector<std::vector<std::vector<float>>> *kernelMatrix, std::vector<std::vector<std::vector<float>>> *outputMatrix, int startRowIndex, int endRowIndex, int startColIndex, int endColIndex)
{
    threadArgs_D t_arg;
    t_arg.kDepth = kDepth;
    t_arg.kHeight = kHeight;
    t_arg.kWidth = kWidth;
    t_arg.inDepth = inDepth;
    t_arg.inHeight = inHeight;
    t_arg.inWidth = inWidth;
    t_arg.startRowIndex = startRowIndex;
    t_arg.endRowIndex = endRowIndex;
    t_arg.startColIndex = startColIndex;
    t_arg.endColIndex = endColIndex;
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
    int endRowIndex = t_args->endRowIndex;
    int startColIndex = t_args->startColIndex;
    int endColIndex = t_args->endColIndex;

    const std::vector<std::vector<std::vector<int>>> *imgMatrix = t_args->imgMatrix;
    const std::vector<std::vector<std::vector<int>>> *kernelMatrix = t_args->kernelMatrix;
    std::vector<std::vector<std::vector<int>>> *outputMatrix = t_args->outputMatrix;
    Convolution3D<int>(imgMatrix, kernelMatrix, outputMatrix, kDepth, kHeight, kWidth, inDepth, inHeight, inWidth, startRowIndex, endRowIndex, startColIndex, endColIndex);

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
    int endRowIndex = t_args->endRowIndex;
    int startColIndex = t_args->startColIndex;
    int endColIndex = t_args->endColIndex;

    const std::vector<std::vector<std::vector<float>>> *imgMatrix = t_args->imgMatrix;
    const std::vector<std::vector<std::vector<float>>> *kernelMatrix = t_args->kernelMatrix;
    std::vector<std::vector<std::vector<float>>> *outputMatrix = t_args->outputMatrix;
    Convolution3D<float>(imgMatrix, kernelMatrix, outputMatrix, kDepth, kHeight, kWidth, inDepth, inHeight, inWidth, startRowIndex, endRowIndex, startColIndex, endColIndex);

    return NULL;
}

template <typename T>
void MultiThreaded3DConvolution::Convolution3D(const std::vector<std::vector<std::vector<T>>> *imgMatrix, const std::vector<std::vector<std::vector<T>>> *kernelMatrix, std::vector<std::vector<std::vector<T>>> *outputMatrix, int kDepth, int kHeight, int kWidth, int inDepth, int inHeight, int inWidth, int startRowIndex, int endRowIndex, int startColIndex, int endColIndex)
{
    int depth, height, width;
    int padDepth = (kDepth-1)/2;
    int padHeight = (kHeight-1)/2;
    int padWidth = (kWidth-1)/2;

    for(int i=0; i<inDepth; ++i)
    {
        for(int j=startRowIndex; j<endRowIndex; ++j)
        {
            for(int k=startColIndex; k<endColIndex; ++k)
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