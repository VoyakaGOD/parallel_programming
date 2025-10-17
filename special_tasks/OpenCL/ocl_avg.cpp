#include <iostream>
#include "ocl.hpp"
#include <chrono>

int main(int argc, char **argv)
{
    // 1. get device
    cl_device_id device = handle_args(argc, argv, CL_DEVICE_TYPE_GPU);

    // 2. create context, queue, kernel
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, nullptr);
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, nullptr, nullptr);
    std::string program_src = load_program_source("avg.cl");
    const char *program_c_string = program_src.c_str();
    cl_program program = clCreateProgramWithSource(context, 1, &program_c_string, nullptr, nullptr);
    clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    cl_kernel kernel = clCreateKernel(program, "sum_array", nullptr);

    // 3. create and attach buffers
    require(argc > 1, "You should specify array size");
    int N = std::stoi(argv[1]);
    require(N > 0, "Array size must be positive");
    std::vector<float> array(N);
    for (int i = 0; i < N; i++)
        array[i] = static_cast<float>(i);
    cl_mem buffer = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(float) * N,
        array.data(),
        nullptr
    );
    size_t local_size = 64;
    size_t groups_count = ((N + local_size - 1)/local_size);
    size_t global_size = groups_count * local_size;
    cl_mem partial_sums = clCreateBuffer(
        context,
        CL_MEM_WRITE_ONLY,
        sizeof(float) * groups_count,
        nullptr,
        nullptr
    );
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &partial_sums);
    clSetKernelArg(kernel, 2, sizeof(float)*local_size, nullptr);

    // 4.execute
    auto start = std::chrono::high_resolution_clock::now();
    clEnqueueNDRangeKernel(
        queue,
        kernel,
        1,
        nullptr,
        &global_size,
        &local_size,
        0,
        nullptr,
        nullptr
    );
    clFinish(queue);
    std::vector<float> partial_sum_values(groups_count);
    clEnqueueReadBuffer(
        queue,
        partial_sums,
        CL_TRUE,
        0,
        sizeof(float) * groups_count,
        partial_sum_values.data(),
        0,
        nullptr,
        nullptr
    );

    float avg = 0;
    for(int i = 0; i < partial_sum_values.size(); i++)
        avg += partial_sum_values[i];
    avg /= N;
    auto end = std::chrono::high_resolution_clock::now();

    // 5. show result
    std::cout << "Average = " << avg << std::endl;
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "--> OpenCL time = " << elapsed.count() << "s" << std::endl;

    // 6. free resources
    clReleaseMemObject(buffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    // 7. CPU version
    start = std::chrono::high_resolution_clock::now();
    float cpu_avg = 0;
    for(int i = 0; i < N; i++)
        cpu_avg += array[i];
    cpu_avg /= N;
    end = std::chrono::high_resolution_clock::now();
    std::cout << "CPU Average = " << avg << std::endl;
    elapsed = end - start;
    std::cout << "--> CPU time = " << elapsed.count() << "s" << std::endl;

    return 0;
}
