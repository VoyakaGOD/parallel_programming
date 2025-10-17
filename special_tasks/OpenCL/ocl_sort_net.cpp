#include <iostream>
#include "ocl.hpp"
#include <chrono>
#include <random>

typedef int32_t value_t;

bool is_sorted(value_t *array, int size, int direction = 1)
{
    for(int i = 1; i < size; i++)
    {
        if(array[i-1] == array[i])
            continue;
        if(static_cast<bool>(direction) == (array[i-1] > array[i]))
            return false;
    }
    return true;
}

int main(int argc, char **argv)
{
    // 1. get device
    cl_device_id device = handle_args(argc, argv, CL_DEVICE_TYPE_GPU);

    // 2. create context, queue, kernel
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, nullptr);
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, nullptr, nullptr);
    std::string program_src = load_program_source("bitonic.cl");
    const char *program_c_string = program_src.c_str();
    cl_program program = clCreateProgramWithSource(context, 1, &program_c_string, nullptr, nullptr);
    clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    cl_kernel fast_kernel = clCreateKernel(program, "fast_sort", nullptr);
    cl_kernel stride_kernel = clCreateKernel(program, "stride_sort", nullptr);

    // 3. create and attach kernel args
    require(argc > 1, "You should specify array size[log_2(N)]");
    int logN = std::stoi(argv[1]);
    require(logN > 0, "Array size[log_2(N)] should be positive");
    int N = 1 << logN;
    std::cout << "Array size: " << N << std::endl;
    size_t global_size = N;
    size_t max_work_group_size = 0;
    cl_int err = clGetDeviceInfo(
        device,
        CL_DEVICE_MAX_WORK_GROUP_SIZE,
        sizeof(size_t),
        &max_work_group_size,
        nullptr
    );
    size_t local_size = std::min(global_size, max_work_group_size);
    std::cout << "Work group size: " << local_size << std::endl;

    std::random_device random_dev;
    std::mt19937 engine(random_dev());
    std::uniform_int_distribution<value_t> dist(INT32_MIN, INT32_MAX);

    value_t *array = new value_t[N];
    require(array, "Can't allocate memory for array");
    for (int i = 0; i < N; ++i)
        array[i] = dist(engine);

    auto pre_start = std::chrono::high_resolution_clock::now();
    cl_mem buffer = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
        sizeof(value_t) * N,
        array,
        nullptr
    );
    int direction = 1; // Ascending
    clSetKernelArg(fast_kernel, 0, sizeof(cl_mem), &buffer);
    clSetKernelArg(fast_kernel, 1, sizeof(int), &direction);
    clSetKernelArg(stride_kernel, 0, sizeof(cl_mem), &buffer);
    clSetKernelArg(stride_kernel, 3, sizeof(int), &direction);

    // 4.execute
    auto start = std::chrono::high_resolution_clock::now();
    clEnqueueNDRangeKernel(
        queue,
        fast_kernel,
        1,
        nullptr,
        &global_size,
        &local_size,
        0,
        nullptr,
        nullptr
    );
    for(int stage = local_size << 1; stage <= N; stage <<= 1)
    {
        for(int stride = stage >> 1; stride > 0; stride >>= 1)
        {
            clSetKernelArg(stride_kernel, 1, sizeof(int), &stage);
            clSetKernelArg(stride_kernel, 2, sizeof(int), &stride);
            clEnqueueNDRangeKernel(
                queue,
                stride_kernel,
                1,
                nullptr,
                &global_size,
                &local_size,
                0,
                nullptr,
                nullptr
            );
        }
    }
    clFinish(queue);
    clEnqueueReadBuffer(
        queue,
        buffer,
        CL_TRUE,
        0,
        sizeof(value_t) * N,
        array,
        0,
        nullptr,
        nullptr
    );
    auto end = std::chrono::high_resolution_clock::now();

    // 5. show result
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Kernel time: " << elapsed.count();
    std::cout << ", " << (is_sorted(array, N) ? "V" : "X") << std::endl;
    elapsed = end - pre_start;
    std::cout << "Full time: " << elapsed.count() << std::endl;

    // 6. free resources
    clReleaseMemObject(buffer);
    clReleaseKernel(fast_kernel);
    clReleaseKernel(stride_kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return 0;
}
