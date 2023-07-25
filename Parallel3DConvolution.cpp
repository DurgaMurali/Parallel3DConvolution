#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include "Parallel3DConvolution.h"

void openCL3DConvolution::setData(std::string inFilename, int inHeight, int inWidth, int inDepth, std::string kFilename, int kHeight, int kWidth, int kDepth, std::string outFile)
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

void openCL3DConvolution::kernelSetup(cl_platform_id *platform, cl_device_id *device, cl_context *context, cl_command_queue *queue, cl_program *program, cl_kernel *kernel, std::string &kernelFileName, std::string &kernelName)
{
    std::cout << "Filename = " << kernelFileName << std::endl;
    std::string kernelFile = readKernel(kernelFileName);
    //std::cout << kernelFile;
    const size_t sourceSize = kernelFile.size();
    const char* kernelSource = kernelFile.c_str();

    cl_int errorCode;
    std::cout << "Get platform IDs" << std::endl;
    errorCode = clGetPlatformIDs(1, platform, NULL);
    std::cout << "Error Code = " << errorCode << std::endl;

    std::cout << "Get device IDs" << std::endl;
    errorCode = clGetDeviceIDs(*platform, CL_DEVICE_TYPE_GPU, 1, device, NULL);
    std::cout << "Error Code = " << errorCode << std::endl;

    *context = clCreateContext(NULL, 1, device, NULL, NULL, &errorCode);
    std::cout << "Error Code = " << errorCode << std::endl;

    std::cout << "Create command queue" << std::endl;
    *queue = clCreateCommandQueue(*context, *device, 0, &errorCode);
    std::cout << "Error Code = " << errorCode << std::endl;
    
    std::cout << "Create clprogram" << std::endl;
    *program = clCreateProgramWithSource(*context, 1, &kernelSource, &sourceSize, &errorCode);
    std::cout << "Error Code = " << errorCode << std::endl;
    errorCode = clBuildProgram(*program, 1, device, NULL, NULL, NULL);
    std::cout << "Error Code = " << errorCode << std::endl;

    size_t logSize = 1000;
    char *errorLog = new char[logSize];
    errorCode = clGetProgramBuildInfo(*program, *device, CL_PROGRAM_BUILD_LOG, logSize, errorLog, NULL);
    std::cout << "Error Code = " << errorCode <<  "logSize = " << logSize << std::endl;
    std::string log(errorLog);
    std::cout << log << std::endl;

    std::cout << "Create clkernel - " << kernelName << std::endl;
    *kernel = clCreateKernel(*program, kernelName.c_str(), &errorCode);
    std::cout << "Error Code = " << errorCode << std::endl;
}

void openCL3DConvolution::executeKernel(cl_command_queue *queue, cl_kernel *kernel, cl_mem &inputBuffer, cl_mem &kernelBuffer, cl_mem &outputBuffer)
{
    cl_int errorCode, value;
    std::cout << "set kernel Args" << std::endl;
    errorCode = clSetKernelArg(*kernel, 0, sizeof(cl_mem), &inputBuffer);
    std::cout << "Error Code = " << errorCode << std::endl;
    errorCode = clSetKernelArg(*kernel, 1, sizeof(cl_mem), &kernelBuffer);
    std::cout << "Error Code = " << errorCode << std::endl;
    errorCode = clSetKernelArg(*kernel, 2, sizeof(cl_mem), &outputBuffer);
    std::cout << "Error Code = " << errorCode << std::endl;
    std::cout << "executeKernel" << std::endl;
    errorCode = clSetKernelArg(*kernel, 3, sizeof(int), &kDepth);
    std::cout << "Error Code = " << errorCode << std::endl;
    errorCode = clSetKernelArg(*kernel, 4, sizeof(int), &kHeight);
    std::cout << "Error Code = " << errorCode << std::endl;
    errorCode = clSetKernelArg(*kernel, 5, sizeof(int), &kWidth);
    std::cout << "Error Code = " << errorCode << std::endl;
    
    std::cout << "Enqueue NDRange kernel" << std::endl;
    size_t globalSize[3] = {static_cast<size_t>(inWidth), static_cast<size_t>(inHeight), static_cast<size_t>(inDepth)};
    errorCode = clEnqueueNDRangeKernel(*queue, *kernel, 3, NULL, globalSize, NULL, 0, NULL, NULL);
    std::cout << "Error Code = " << errorCode << std::endl;

    value = clFlush(*queue);
    value = clFinish(*queue);
    std::cout<< "Value = " << value << std::endl;
}

void openCL3DConvolution::setup()
{
    std::cout << "Execute kernel" << std::endl;

    ReadMatrix readFile;
    int datatype = readFile.findFileDatatype(inFilename);

    cl_platform_id *platform = new cl_platform_id();
    cl_device_id *device = new cl_device_id();
    cl_context *context = new cl_context();
    cl_command_queue *queue = new cl_command_queue();
    cl_program *program = new cl_program();
    cl_kernel *kernel = new cl_kernel();
    std::string kernelName = "";
    std::string kernelFileName = "";
    cl_mem inputBuffer, kernelBuffer, outputBuffer;

    switch(datatype)
    {
        case INT:
        {
            int imgMatrix[inDepth*inHeight*inWidth];
            readFile.readMatrixFromFile<int>(inFilename, inDepth*inHeight*inWidth, imgMatrix);
            int kernelMatrix[kDepth*kHeight*kWidth];
            readFile.readMatrixFromFile<int>(kFilename, kDepth*kHeight*kWidth, kernelMatrix);

            for(int i=0; i<kDepth*kHeight*kWidth; ++i)
            std::cout << kernelMatrix[i] << " ";
            std::cout << std::endl;
            for(int i=0; i<inHeight*inWidth*inDepth; ++i)
                std::cout << imgMatrix[i] << " ";
            std::cout << std::endl;

            kernelFileName = "Convolution3d_Integer.cl";
            kernelName = "convolution3D_integer";
            kernelSetup(platform, device, context, queue, program, kernel, kernelFileName, kernelName);

            cl_int errorCode;
            std::cout << "Create buffers - " << std::endl;
            inputBuffer = clCreateBuffer(*context, CL_MEM_READ_WRITE, sizeof(int)*inDepth*inHeight*inWidth, NULL, &errorCode);
            std::cout << "Error Code = " << errorCode << std::endl;
            kernelBuffer = clCreateBuffer(*context, CL_MEM_READ_WRITE, sizeof(int)*kDepth*kHeight*kWidth, NULL, &errorCode);
            std::cout << "Error Code = " << errorCode << std::endl;
            outputBuffer = clCreateBuffer(*context, CL_MEM_READ_WRITE, sizeof(int)*inDepth*inHeight*inWidth, NULL, &errorCode);
            std::cout << "Error Code = " << errorCode << std::endl;

            std::cout << "Enqueue write buffer" << std::endl;
            errorCode = clEnqueueWriteBuffer(*queue, inputBuffer, CL_TRUE, 0, sizeof(int)*inDepth*inHeight*inWidth, &imgMatrix, 0, NULL, NULL);
            std::cout << "Error Code = " << errorCode << std::endl;
            errorCode = clEnqueueWriteBuffer(*queue, kernelBuffer, CL_TRUE, 0, sizeof(int)*kDepth*kHeight*kWidth, &kernelMatrix, 0, NULL, NULL);
            std::cout << "Error Code = " << errorCode << std::endl;

            cl_int value = clFlush(*queue);
            value = clFinish(*queue);
            std::cout<< "Value = " << value << std::endl;

            int imgoutput[inDepth*inHeight*inWidth];
            errorCode = clEnqueueReadBuffer(*queue, inputBuffer, CL_TRUE, 0, sizeof(int)*inDepth*inHeight*inWidth, &imgoutput, 0, NULL, NULL);
            std::cout << "Error Code = " << errorCode << std::endl;

            int koutput[kDepth*kHeight*kWidth];
            errorCode = clEnqueueReadBuffer(*queue, kernelBuffer, CL_TRUE, 0, sizeof(int)*kDepth*kHeight*kWidth, &koutput, 0, NULL, NULL);
            std::cout << "Error Code = " << errorCode << std::endl;

            value = clFlush(*queue);
            value = clFinish(*queue);
            std::cout<< "Value = " << value << std::endl;

            std::cout << "Input \n";
            for(int i=0; i<inDepth*inHeight*inWidth; i++)
            {
                std::cout <<imgoutput[i]<<" ";
            }
            std::cout<<std::endl;

            std::cout << "Kernel\n";
            for(int i=0; i<kDepth*kHeight*kWidth; i++)
            {
                std::cout <<koutput[i]<<" ";
            }
            std::cout<<std::endl;

            std::cout << "executeKernel" << std::endl;
            executeKernel(queue, kernel, inputBuffer, kernelBuffer, outputBuffer);

            int outputMatrix[inDepth*inHeight*inWidth];
            for(int i=0; i<inDepth*inHeight*inWidth; ++i)
                outputMatrix[i] = 0;

            std::cout << "Enqueue read buffer" << std::endl;
            value = clEnqueueReadBuffer(*queue, outputBuffer, CL_TRUE, 0, sizeof(int)*inDepth*inHeight*inWidth, &outputMatrix, 0, NULL, NULL);
            std::cout<< "Value = " << value << std::endl;
            value = clFlush(*queue);
            value = clFinish(*queue);
            std::cout<< "Value = " << value << std::endl;

            std::cout<<"Print output\n";
            for(int i=0; i<inDepth*inHeight*inWidth; i++)
                std::cout << outputMatrix[i] << " ";

            writeToFile<int>(outputMatrix);

            break;
        }

        case DOUBLE:
        {
            double imgMatrix[inDepth*inHeight*inWidth];
            readFile.readMatrixFromFile<double>(inFilename, inDepth*inHeight*inWidth, imgMatrix);
            double kernelMatrix[kDepth*kHeight*kWidth];
            readFile.readMatrixFromFile<double>(kFilename, kDepth*kHeight*kWidth, kernelMatrix);

            for(int i=0; i<kDepth*kHeight*kWidth; ++i)
            std::cout << kernelMatrix[i] << " ";
            std::cout << std::endl;
            for(int i=0; i<inHeight*inWidth*inDepth; ++i)
                std::cout << imgMatrix[i] << " ";
            std::cout << std::endl;

            kernelFileName = "Convolution3d_Integer.cl";
            kernelName = "convolution3D_integer";
            kernelSetup(platform, device, context, queue, program, kernel, kernelFileName, kernelName);

            cl_int errorCode;
            std::cout << "Create buffers - " << std::endl;
            inputBuffer = clCreateBuffer(*context, CL_MEM_READ_WRITE, sizeof(double)*inDepth*inHeight*inWidth, NULL, &errorCode);
            std::cout << "Error Code = " << errorCode << std::endl;
            kernelBuffer = clCreateBuffer(*context, CL_MEM_READ_WRITE, sizeof(double)*kDepth*kHeight*kWidth, NULL, &errorCode);
            std::cout << "Error Code = " << errorCode << std::endl;
            outputBuffer = clCreateBuffer(*context, CL_MEM_READ_WRITE, sizeof(double)*inDepth*inHeight*inWidth, NULL, &errorCode);
            std::cout << "Error Code = " << errorCode << std::endl;

            std::cout << "Enqueue write buffer" << std::endl;
            errorCode = clEnqueueWriteBuffer(*queue, inputBuffer, CL_TRUE, 0, sizeof(double)*inDepth*inHeight*inWidth, &imgMatrix, 0, NULL, NULL);
            std::cout << "Error Code = " << errorCode << std::endl;
            errorCode = clEnqueueWriteBuffer(*queue, kernelBuffer, CL_TRUE, 0, sizeof(double)*kDepth*kHeight*kWidth, &kernelMatrix, 0, NULL, NULL);
            std::cout << "Error Code = " << errorCode << std::endl;

            cl_int value = clFlush(*queue);
            value = clFinish(*queue);
            std::cout<< "Value = " << value << std::endl;

            int imgoutput[inDepth*inHeight*inWidth];
            errorCode = clEnqueueReadBuffer(*queue, inputBuffer, CL_TRUE, 0, sizeof(double)*inDepth*inHeight*inWidth, &imgoutput, 0, NULL, NULL);
            std::cout << "Error Code = " << errorCode << std::endl;

            int koutput[kDepth*kHeight*kWidth];
            errorCode = clEnqueueReadBuffer(*queue, kernelBuffer, CL_TRUE, 0, sizeof(double)*kDepth*kHeight*kWidth, &koutput, 0, NULL, NULL);
            std::cout << "Error Code = " << errorCode << std::endl;

            value = clFlush(*queue);
            value = clFinish(*queue);
            std::cout<< "Value = " << value << std::endl;

            std::cout << "Input \n";
            for(int i=0; i<inDepth*inHeight*inWidth; i++)
            {
                std::cout <<imgoutput[i]<<" ";
            }
            std::cout<<std::endl;

            std::cout << "Kernel\n";
            for(int i=0; i<kDepth*kHeight*kWidth; i++)
            {
                std::cout <<koutput[i]<<" ";
            }
            std::cout<<std::endl;

            std::cout << "executeKernel" << std::endl;
            executeKernel(queue, kernel, inputBuffer, kernelBuffer, outputBuffer);

            double outputMatrix[inDepth*inHeight*inWidth];
            for(int i=0; i<inDepth*inHeight*inWidth; ++i)
                outputMatrix[i] = 0;

            std::cout << "Enqueue read buffer" << std::endl;
            value = clEnqueueReadBuffer(*queue, outputBuffer, CL_TRUE, 0, sizeof(double)*inDepth*inHeight*inWidth, &outputMatrix, 0, NULL, NULL);
            std::cout<< "Value = " << value << std::endl;
            value = clFlush(*queue);
            value = clFinish(*queue);
            std::cout<< "Value = " << value << std::endl;

            std::cout<<"Print output\n";
            for(int i=0; i<inDepth*inHeight*inWidth; i++)
                std::cout << outputMatrix[i] << " ";
        
            writeToFile<double>(outputMatrix);

            break;
        }
    }

    std::cout << std::endl << "Release objects" << std::endl;
    //Cleanup
    clReleaseKernel(*kernel);
    clReleaseProgram(*program);
    clReleaseMemObject(inputBuffer);
    clReleaseMemObject(kernelBuffer);
    clReleaseMemObject(outputBuffer);
    clReleaseCommandQueue(*queue);
    clReleaseContext(*context);

    free(platform);
    free(device);
    free(context);
    free(queue);
    free(program);
    free(kernel);
}

std::string openCL3DConvolution::readKernel(std::string fileName)
{
    std::ifstream file(fileName);
    std::string kernelCode = "";
    std::string line;

    while(std::getline(file, line))
    {
        kernelCode += line + "\n";
    }
    return kernelCode;
}

template <typename T>
void openCL3DConvolution::writeToFile(const T* outputMatrix)
{

    std::ofstream file(outFile);
    std::cout << "\nOutfile = " << outFile << std::endl;
    for(int i=0; i<inDepth; i++)
    {
        for(int j=0; j<inHeight; j++)
        {
            for(int k=0; k<inWidth; k++)
            {
                file<<outputMatrix[i*inHeight*inWidth + j*inWidth + k]<<" ";
            }
            file<<std::endl;
        }
        file<<std::endl;
    }
    file.close();
}

int main(int argc, char *argv[])
{
    openCL3DConvolution conv;
    conv.setData(argv[1], std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]), argv[5], std::stoi(argv[6]), std::stoi(argv[7]), std::stoi(argv[8]), argv[9]);
    //conv.setup();
    conv.setup();
}