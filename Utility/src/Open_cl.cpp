#include "Open_cl.h"

#include <iostream>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Utility {

//////////////////////////////////////////////////////////////
std::tuple<bool,cl_platform_id,cl_device_id> Open_cl::setup()
{
    cl_int rc;
    cl_platform_id platform_ids[100];
    cl_uint num_platforms;
    rc = clGetPlatformIDs( 100, platform_ids, &num_platforms );
    if (CL_SUCCESS != rc || num_platforms == 0) {
        return std::make_tuple( false, nullptr, nullptr ); 
    }
    for (cl_uint i=0; i<num_platforms; ++i) {
        char value[1024];
        size_t returned_size;
        rc = clGetPlatformInfo( platform_ids[i], CL_PLATFORM_PROFILE, 1024, value, &returned_size );
        if (CL_SUCCESS != rc) {
            return std::make_tuple( false, nullptr, nullptr ); 
        }
        std::cout << "CL_PLATFORM_PROFILE[" << i << "]:     " << value << "\n";
        rc = clGetPlatformInfo( platform_ids[i], CL_PLATFORM_NAME, 1024, value, &returned_size );
        if (CL_SUCCESS != rc) {
            return std::make_tuple( false, nullptr, nullptr ); 
        }
        std::cout << "CL_PLATFORM_NAME[" << i << "]:        " << value << "\n";
        rc = clGetPlatformInfo( platform_ids[i], CL_PLATFORM_VERSION, 1024, value, &returned_size );
        if (CL_SUCCESS != rc) {
            return std::make_tuple( false, nullptr, nullptr ); 
        }
        std::cout << "CL_PLATFORM_VERSION[" << i << "]:     " << value << "\n";
        cl_device_id device_ids[100];
        cl_uint num_devices;
        rc = clGetDeviceIDs( platform_ids[i], CL_DEVICE_TYPE_ALL, 1024, device_ids, &num_devices );
        if (CL_SUCCESS != rc) {
            return std::make_tuple( false, nullptr, nullptr ); 
        }
        std::cout << "clGetDeviceIDs returned " << num_devices << " devices\n";
        for (cl_uint j=0; j<num_devices; ++j) {
            rc = clGetDeviceInfo( device_ids[j], CL_DEVICE_NAME, 1024, value, &returned_size );
            if (CL_SUCCESS != rc) {
                return std::make_tuple( false, nullptr, nullptr ); 
            }
            std::cout << "\tCL_DEVICE_NAME:  " << value << "\n";
            cl_uint uint_value;
            rc = clGetDeviceInfo( device_ids[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &uint_value, &returned_size );
            if (CL_SUCCESS != rc) {
                return std::make_tuple( false, nullptr, nullptr ); 
            }
            std::cout << "\tCL_DEVICE_MAX_COMPUTE_UNITS:  " << uint_value << "\n";
            cl_ulong ulong_value;
            rc = clGetDeviceInfo( device_ids[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &ulong_value, &returned_size );
            if (CL_SUCCESS != rc) {
                return std::make_tuple( false, nullptr, nullptr ); 
            }
            std::cout << "\tCL_DEVICE_GLOBAL_MEM_SIZE:  " << ulong_value << "\n";
            rc = clGetDeviceInfo( device_ids[j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &ulong_value, &returned_size );
            if (CL_SUCCESS != rc) {
                return std::make_tuple( false, nullptr, nullptr ); 
            }
            std::cout << "\tCL_DEVICE_LOCAL_MEM_SIZE:  " << ulong_value << "\n";
            size_t size_t_value;
            rc = clGetDeviceInfo( device_ids[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &size_t_value, &returned_size );
            if (CL_SUCCESS != rc) {
                return std::make_tuple( false, nullptr, nullptr ); 
            }
            std::cout << "\tCL_DEVICE_MAX_WORK_GROUP_SIZE:  " << size_t_value << "\n";

            std::cout << "\tsizeof cl_short:                " << sizeof(cl_short) << "\n";
            std::cout << "\tsizeof cl_int:                  " << sizeof(cl_int) << "\n";
            std::cout << "\tsizeof cl_float:                " << sizeof(cl_float) << "\n";

            // I'm  not actually sure how to select a platform and device.
            // But if we get this far, then I guess this is good enough?
            return std::make_tuple( true, platform_ids[i], device_ids[j] );
        }
    }
    return std::make_tuple( false, nullptr, nullptr ); 
}

//////////////////////////////////////////////////////////////
cl_context Open_cl::create_context( cl_platform_id platform_id, cl_device_id device_id )
{
    cl_context_properties properties[3];
    properties[0] = CL_CONTEXT_PLATFORM;
    properties[1] = reinterpret_cast<cl_context_properties>(platform_id);
    properties[2] = 0;
    cl_int rc;

    cl_context context = clCreateContext( properties, 1, &device_id, NULL, NULL, &rc );
    if (CL_SUCCESS != rc) {
        throw std::runtime_error( "clCreateContext" );
    }
    return context;
}

//////////////////////////////////////////////////////////////
cl_command_queue Open_cl::create_command_queue( cl_context context, cl_device_id device_id )
{
    cl_int rc;
    cl_command_queue command_queue;
    command_queue = clCreateCommandQueue( context, device_id, 0, &rc );
    if (CL_SUCCESS != rc) {
        throw std::runtime_error( "clCreateCommandQueue" );
    }
    return command_queue;
}

//////////////////////////////////////////////////////////////
std::tuple<cl_program,cl_kernel> Open_cl::create_kernel( const char* source, const char* kernel_name, cl_context context, cl_device_id device_id )
{
    cl_int rc;
    cl_program program;
    std::cout << source << "\n";
    program = clCreateProgramWithSource( context, 1, &source, NULL, &rc );
    if (CL_SUCCESS != rc) {
        throw std::runtime_error( "clCreateProgramWithSource" );
    }
    rc = clBuildProgram( program, 1, &device_id, NULL, NULL, NULL );
    if (CL_SUCCESS != rc) {
        throw std::runtime_error( "clBuildProgram" );
    }
    cl_kernel kernel;
    kernel = clCreateKernel( program, kernel_name, &rc );
    if (CL_SUCCESS != rc) {
        throw std::runtime_error( "clCreateKernel" );
    }
    return std::make_tuple( program, kernel );
}

//////////////////////////////////////////////////////////////
cl_mem Open_cl::create_buffer( cl_context context, cl_mem_flags flags, size_t size )
{
    cl_int rc;
    cl_mem buffer = clCreateBuffer( context, flags, size, NULL, &rc );
    if (CL_SUCCESS != rc) {
        throw std::runtime_error( "Failed clCreateBuffer" );
    }
    return buffer;
}

//////////////////////////////////////////////////////////////
void Open_cl::set_kernel_arg( cl_kernel kernel, cl_uint index, size_t size, const void* arg )
{
    cl_int rc = clSetKernelArg( kernel, index, size, arg );
    if (CL_SUCCESS != rc) {
        throw std::runtime_error( "Failed clSetKernelArg" );
    }
}

//////////////////////////////////////////////////////////////
void Open_cl::enqueue_write_buffer(cl_command_queue command_queue, cl_mem buffer, cl_bool blocking, size_t size, const void *ptr )
{
    cl_int rc = clEnqueueWriteBuffer( command_queue, buffer, blocking, 0, size, ptr, 0, NULL, NULL );
    if (CL_SUCCESS != rc) {
        throw std::runtime_error( "Failed clEnqueueWriteBuffer" );
    }
}

//////////////////////////////////////////////////////////////
void Open_cl::enqueue_read_buffer( cl_command_queue command_queue, cl_mem buffer, cl_bool blocking, size_t size, void *ptr )
{
    cl_int rc = clEnqueueReadBuffer( command_queue, buffer, blocking, 0, size, ptr, 0, NULL, NULL );
    if (CL_SUCCESS != rc) {
        throw std::runtime_error( "Failed clEnqueueReadBuffer" );
    }
}

//////////////////////////////////////////////////////////////
void Open_cl::enqueue_nd_range_kernel( cl_command_queue command_queue, cl_kernel kernel, size_t count )
{
    cl_int rc = clEnqueueNDRangeKernel( command_queue, kernel, 1, NULL, &count, NULL, 0, NULL, NULL );
    if (CL_SUCCESS != rc) {
        throw std::runtime_error( "Failed clEnqueueNDRangeKernel" );
    }
}


}}

