#pragma OPENCL EXTENSION cl_intel_printf : enable

__kernel void convolution3D_integer(__global const int *inputBuffer, __global const int *kernelBuffer, __global int *outputBuffer, const int kDepth, const int kHeight, const int kWidth)
{
    int col = get_global_id(0);
    int row = get_global_id(1);
    int slice = get_global_id(2);
    int padDepth = (kDepth-1)/2;
    int padHeight = (kHeight-1)/2;
    int padWidth = (kWidth-1)/2;

    int inWidth = get_global_size(0);
    int inHeight = get_global_size(1);
    int inDepth = get_global_size(2);

    int sum = 0;
    for(int i=0; i<kDepth; ++i)
    {
        for(int j=0; j<kHeight; ++j)
        {
            for(int k=0; k<kWidth; ++k)
            {
                int depth = slice + (i - padDepth);
                int height = row + (j - padHeight);
                int width = col + (k - padWidth);

                if((depth >= 0) && (depth < inDepth) && (height >= 0) && (height < inHeight) && (width >= 0) && (width < inWidth))
                {
                    int inIndex = depth*inHeight*inWidth + height*inWidth + width;
                    int kIndex = i*kHeight*kWidth + j*kWidth + k;
                    sum = sum + (inputBuffer[inIndex] * kernelBuffer[kIndex]);
                }
            }
        }
    }

    int outIndex = slice*inHeight*inWidth + row*inWidth + col;
    outputBuffer[outIndex] = sum;
}