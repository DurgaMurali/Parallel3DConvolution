#include <iostream>
#include "Convolution.h"
#include <time.h>
#include "CL/opencl.h"

class openCL3DConvolution : public Convolution
{
    public:
    void kernelSetup(cl_platform_id *platform, cl_device_id *device, cl_context *context, cl_command_queue *queue, cl_program *program, cl_kernel *kernel, std::string &kernelFileName, std::string &kernelName);
    void executeKernel(cl_command_queue *queue, cl_kernel *kernel, cl_mem &inputBuffer, cl_mem &kernelBuffer, cl_mem &outputBuffer);
    void setup();
    std::string readKernel(std::string fileName);
    template <typename T>
    void writeToFile(const T* outputMatrix);
}; 