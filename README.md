# Parallel3DConvolution

## Prerequisites
>Install OpenCL runtime and SDK
>
>Install Cygwin and MinGW on Windows
>
## Commands to compile
>g++ Convolution.cpp Seq3DConv.cpp ReadMatrix.cpp -o conv.o
>
>g++ Convolution.cpp Parallel3DConvolution.cpp ReadMatrix.cpp -o p_conv.o -I <opencl_include_path> -L <opencl_lib_path> -l OpenCL
>
>g++ -pthread Convolution.cpp MultiThreaded3DConvolution.cpp ReadMatrix.cpp -o m_conv.o

## Commands to execute
>conv.o <input_file> <input_height> <input_width> <input_depth> <kernel_file> <kernel_height> <kernel_width> <kernel_depth> <output_file>
>
>p_conv.o <input_file> <input_height> <input_width> <input_depth> <kernel_file> <kernel_height> <kernel_width> <kernel_depth> <output_file>
>
>m_conv.o <input_file> <input_height> <input_width> <input_depth> <kernel_file> <kernel_height> <kernel_width> <kernel_depth> <output_file> <num_Threads>
