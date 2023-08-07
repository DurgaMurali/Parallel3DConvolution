#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include "Parallel3DConvolution.h"

void openCL3DConvolution::kernelSetup(cl_platform_id *platform, cl_device_id *device, cl_context *context, cl_command_queue *queue, cl_program *program, cl_kernel *kernel, std::string &kernelFileName, std::string &kernelName)
{
    std::string kernelFile = readKernel(kernelFileName);
    const size_t sourceSize = kernelFile.size();
    const char* kernelSource = kernelFile.c_str();

    cl_int errorCode;
    errorCode = clGetPlatformIDs(1, platform, NULL);
    errorCode = clGetDeviceIDs(*platform, CL_DEVICE_TYPE_GPU, 1, device, NULL);
    *context = clCreateContext(NULL, 1, device, NULL, NULL, &errorCode);
    *queue = clCreateCommandQueue(*context, *device, 0, &errorCode);
    *program = clCreateProgramWithSource(*context, 1, &kernelSource, &sourceSize, &errorCode);
    errorCode = clBuildProgram(*program, 1, device, NULL, NULL, NULL);
    std::cout << "Build program = " << errorCode << std::endl;

    size_t logSize = 1000;
    char *errorLog = new char[logSize];
    errorCode = clGetProgramBuildInfo(*program, *device, CL_PROGRAM_BUILD_LOG, logSize, errorLog, NULL);
    std::string log(errorLog);

    *kernel = clCreateKernel(*program, kernelName.c_str(), &errorCode);
}

void openCL3DConvolution::executeKernel(cl_command_queue *queue, cl_kernel *kernel, cl_mem &inputBuffer, cl_mem &kernelBuffer, cl_mem &outputBuffer)
{
    cl_int errorCode, value;
    errorCode = clSetKernelArg(*kernel, 0, sizeof(cl_mem), &inputBuffer);
    errorCode = clSetKernelArg(*kernel, 1, sizeof(cl_mem), &kernelBuffer);
    errorCode = clSetKernelArg(*kernel, 2, sizeof(cl_mem), &outputBuffer);
    errorCode = clSetKernelArg(*kernel, 3, sizeof(int), &kDepth);
    errorCode = clSetKernelArg(*kernel, 4, sizeof(int), &kHeight);
    errorCode = clSetKernelArg(*kernel, 5, sizeof(int), &kWidth);
    
    size_t workGroupSize = 80;
    //std::cout << "Convolve" << std::endl;
    size_t globalSize[3] = {static_cast<size_t>(inWidth), static_cast<size_t>(inHeight), static_cast<size_t>(inDepth)};
    errorCode = clEnqueueNDRangeKernel(*queue, *kernel, 3, NULL, globalSize, NULL, 0, NULL, NULL);

    value = clFlush(*queue);
    value = clFinish(*queue);
}

void openCL3DConvolution::setup()
{
    ReadMatrix readFile;
    int datatype = readFile.findFileDatatype(inFilename);
    clock_t time = clock();

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
        case TYPE_INT:
        {
            int *imgMatrix = new int[inDepth*inHeight*inWidth];
            readFile.readMatrixFromFile<int>(inFilename, inDepth*inHeight*inWidth, imgMatrix);
            int *kernelMatrix = new int[kDepth*kHeight*kWidth];
            readFile.readMatrixFromFile<int>(kFilename, kDepth*kHeight*kWidth, kernelMatrix);

            kernelFileName = "Convolution3D_Integer.cl";
            kernelName = "convolution3D_integer";
            kernelSetup(platform, device, context, queue, program, kernel, kernelFileName, kernelName);

            cl_int errorCode;
            inputBuffer = clCreateBuffer(*context, CL_MEM_READ_ONLY, sizeof(int)*inDepth*inHeight*inWidth, NULL, &errorCode);
            kernelBuffer = clCreateBuffer(*context, CL_MEM_READ_ONLY, sizeof(int)*kDepth*kHeight*kWidth, NULL, &errorCode);
            outputBuffer = clCreateBuffer(*context, CL_MEM_READ_WRITE, sizeof(int)*inDepth*inHeight*inWidth, NULL, &errorCode);

            errorCode = clEnqueueWriteBuffer(*queue, inputBuffer, CL_TRUE, 0, sizeof(int)*inDepth*inHeight*inWidth, imgMatrix, 0, NULL, NULL);
            errorCode = clEnqueueWriteBuffer(*queue, kernelBuffer, CL_TRUE, 0, sizeof(int)*kDepth*kHeight*kWidth, kernelMatrix, 0, NULL, NULL);

            cl_int value = clFlush(*queue);
            value = clFinish(*queue);
            
            time = clock() - time;
            std::cout << "Kernel Setup and Input data tranfer time = " << time << " ms" << std::endl;

            executeKernel(queue, kernel, inputBuffer, kernelBuffer, outputBuffer);
            time = clock() - time;
            std::cout << "Kernel execution time = " << time << " ms" << std::endl;

            int *outputMatrix = new int[inDepth*inHeight*inWidth];

            value = clEnqueueReadBuffer(*queue, outputBuffer, CL_TRUE, 0, sizeof(int)*inDepth*inHeight*inWidth, outputMatrix, 0, NULL, NULL);
            value = clFlush(*queue);
            value = clFinish(*queue);
            time = clock() - time;
            std::cout << "Ouput data transfer time = " << time << " ms" << std::endl;

            writeToFile<int>(outputMatrix);

            break;
        }

        case TYPE_FLOAT:
        {
            float *imgMatrix = new float[inDepth*inHeight*inWidth];
            readFile.readMatrixFromFile<float>(inFilename, inDepth*inHeight*inWidth, imgMatrix);
            float *kernelMatrix = new float[kDepth*kHeight*kWidth];
            readFile.readMatrixFromFile<float>(kFilename, kDepth*kHeight*kWidth, kernelMatrix);

            kernelFileName = "Convolution3D_Float.cl";
            kernelName = "convolution3D_float";
            kernelSetup(platform, device, context, queue, program, kernel, kernelFileName, kernelName);

            cl_int errorCode;
            inputBuffer = clCreateBuffer(*context, CL_MEM_READ_ONLY, sizeof(float)*inDepth*inHeight*inWidth, NULL, &errorCode);
            kernelBuffer = clCreateBuffer(*context, CL_MEM_READ_ONLY, sizeof(float)*kDepth*kHeight*kWidth, NULL, &errorCode);
            outputBuffer = clCreateBuffer(*context, CL_MEM_READ_WRITE, sizeof(float)*inDepth*inHeight*inWidth, NULL, &errorCode);

            errorCode = clEnqueueWriteBuffer(*queue, inputBuffer, CL_TRUE, 0, sizeof(float)*inDepth*inHeight*inWidth, imgMatrix, 0, NULL, NULL);
            errorCode = clEnqueueWriteBuffer(*queue, kernelBuffer, CL_TRUE, 0, sizeof(float)*kDepth*kHeight*kWidth, kernelMatrix, 0, NULL, NULL);

            cl_int value = clFlush(*queue);
            value = clFinish(*queue);

            time = clock() - time;
            std::cout << "Kernel Setup and Input data tranfer time = " << time << " ms" << std::endl;

            executeKernel(queue, kernel, inputBuffer, kernelBuffer, outputBuffer);
            time = clock() - time;
            std::cout << "Kernel execution time = " << time << " ms" << std::endl;

            float *outputMatrix = new float[inDepth*inHeight*inWidth];

            value = clEnqueueReadBuffer(*queue, outputBuffer, CL_TRUE, 0, sizeof(float)*inDepth*inHeight*inWidth, outputMatrix, 0, NULL, NULL);
            value = clFlush(*queue);
            value = clFinish(*queue);
            time = clock() - time;
            std::cout << "Ouput data transfer time = " << time << " ms" << std::endl;
        
            writeToFile<float>(outputMatrix);

            break;
        }
    }

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
    //std::cout << "Writing output to file" << std::endl;
    std::ofstream file(outFile);
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
    std::cout << "Opencl Convolution" << std::endl;
    openCL3DConvolution conv;
    conv.setData(argv[1], std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]), argv[5], std::stoi(argv[6]), std::stoi(argv[7]), std::stoi(argv[8]), argv[9]);
    conv.setup();
}