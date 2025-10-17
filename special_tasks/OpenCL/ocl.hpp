#pragma once

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

#ifndef CONST_BUFFER_SIZE
    #define CONST_BUFFER_SIZE 256
#endif

#include <require.hpp>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>

void show_device_info(cl_device_id device, int id = -1);
void show_devices_for_platform(cl_platform_id platform);
void show_platform_info(cl_platform_id platform, int id = -1);
void show_platforms_and_devices();
cl_device_id handle_args(int argc, char **argv, cl_device_type default_device_type);
std::string load_program_source(const std::string &filename);
