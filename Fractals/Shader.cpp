#include "Shader.h"

BITMAP Shader::generateFractal(int x, int y, int cx, int cy, Vec2 size)
{
    FILE* fp;
    char* source_str;
    size_t source_size;

    fp = fopen("generateFractal.cl", "r");
    if (!fp) {
        std::cout << "Failed to load Kernel" << std::endl;
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Create memory buffers on the device
    cl_mem RValue = clCreateBuffer(context, CL_MEM_READ_WRITE, (size.x * size.y) * sizeof(int), NULL, &ret);
    cl_mem GValue = clCreateBuffer(context, CL_MEM_READ_WRITE, (size.x * size.y) * sizeof(int), NULL, &ret);

    return BITMAP();
}
