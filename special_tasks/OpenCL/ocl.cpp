#include "ocl.hpp"

void show_device_info(cl_device_id device, int id)
{
    char name[CONST_BUFFER_SIZE] = {};
    char version[CONST_BUFFER_SIZE] = {};
    char vendor[CONST_BUFFER_SIZE] = {};
    cl_device_type type;
    cl_uint compute_units = 0;
    cl_ulong global_mem = 0;

    clGetDeviceInfo(device, CL_DEVICE_NAME, CONST_BUFFER_SIZE, name, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_VENDOR, CONST_BUFFER_SIZE, vendor, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_VERSION, CONST_BUFFER_SIZE, version, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(global_mem), &global_mem, nullptr);

    if(id > -1)
    std::cout << "    Device #" << id << ":\n";
    else
        std::cout << "    Selected device:\n";
    std::cout << "      Name:           " << name << "\n";
    std::cout << "      Vendor:         " << vendor << "\n";
    std::cout << "      Version:        " << version << "\n";
    std::cout << "      Compute units:  " << compute_units << "\n";
    std::cout << "      Global memory:  " << (global_mem / (1024 * 1024)) << " Mb\n";
    std::cout << "      Type:           ";

    if(type & CL_DEVICE_TYPE_CPU)
        std::cout << "CPU";
    else if(type & CL_DEVICE_TYPE_GPU)
        std::cout << "GPU";
    else if(type & CL_DEVICE_TYPE_ACCELERATOR)
        std::cout << "Accelerator";
    else if(type & CL_DEVICE_TYPE_DEFAULT)
        std::cout << "Default";
    else
        std::cout << "Unknown";

    std::cout << std::endl;
}

void show_devices_for_platform(cl_platform_id platform)
{
    cl_uint num_devices = 0;
    cl_int status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &num_devices);
    require(status == CL_SUCCESS && num_devices > 0, "  [No devices]");

    std::vector<cl_device_id> devices(num_devices);
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_devices, devices.data(), nullptr);
    require(status == CL_SUCCESS, "Can't get device IDs");
    std::cout << "  Number of devices: " << num_devices << std::endl;

    for (cl_uint i = 0; i < num_devices; i++)
        show_device_info(devices[i], static_cast<int>(i));
}

void show_platform_info(cl_platform_id platform, int id)
{
    char name[CONST_BUFFER_SIZE] = {};
    char vendor[CONST_BUFFER_SIZE] = {};
    char version[CONST_BUFFER_SIZE] = {};

    clGetPlatformInfo(platform, CL_PLATFORM_NAME, CONST_BUFFER_SIZE, name, nullptr);
    clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, CONST_BUFFER_SIZE, vendor, nullptr);
    clGetPlatformInfo(platform, CL_PLATFORM_VERSION, CONST_BUFFER_SIZE, version, nullptr);

    if(id > -1)
        std::cout << "Platform #" << id << ":\n";
    else
        std::cout << "Selected platform:\n";
    std::cout << "  Name:    " << name << "\n";
    std::cout << "  Vendor:  " << vendor << "\n";
    std::cout << "  Version: " << version << "\n" << std::endl;
}

void show_platforms_and_devices()
{
    cl_uint num_platforms = 0;

    cl_int status = clGetPlatformIDs(0, nullptr, &num_platforms);
    require(status == CL_SUCCESS, "Can't get num of platforms");
    require(num_platforms > 0, "No OpenCL platforms found");

    std::vector<cl_platform_id> platforms(num_platforms);
    status = clGetPlatformIDs(num_platforms, platforms.data(), nullptr);
    require(status == CL_SUCCESS, "Can't get IDs of platfroms");
    std::cout << "Number of platforms: " << num_platforms << "\n" << std::endl;

    for(cl_uint i = 0; i < num_platforms; i++)
    {
        show_platform_info(platforms[i], static_cast<int>(i));
        show_devices_for_platform(platforms[i]);
    }
}

cl_device_id handle_args(int argc, char **argv, cl_device_type default_device_type)
{
    cl_device_type device_type = default_device_type;

    for(int i = 1; i < argc; i++)
    {
        if(std::strcmp(argv[i], "--clinfo") == 0)
        {
            show_platforms_and_devices();
            exit(0);
        }
        else if(std::strcmp(argv[i], "--cpu") == 0)
        {
            device_type = CL_DEVICE_TYPE_CPU;
            break;
        }
        else if(std::strcmp(argv[i], "--gpu") == 0)
        {
            device_type = CL_DEVICE_TYPE_GPU;
            break;
        }
    }

    cl_platform_id platform;
    cl_uint num_platforms;
    cl_int status = clGetPlatformIDs(1, &platform, &num_platforms);
    require(status == CL_SUCCESS && num_platforms > 0, "Can't get platforms");
    show_platform_info(platform);

    cl_device_id device;
    cl_uint num_devices;
    status = clGetDeviceIDs(platform, device_type, 1, &device, &num_devices);
    require(status == CL_SUCCESS && num_devices > 0, "Can't get device");
    show_device_info(device);

    std::cout << std::endl;
    return device;
}

std::string load_program_source(const std::string &filename)
{
    std::ifstream file(filename);
    require(file.is_open(), "Can't open kernel source file");
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}
