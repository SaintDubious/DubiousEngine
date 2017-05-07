#ifndef INCLUDED_UTILITY_OPENCL
#define INCLUDED_UTILITY_OPENCL

#include <CL/opencl.h>

#include <memory>
#include <tuple>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Utility {

/// @brief Utilities for OpenCL
///
/// Just some wrappers and helper functions to make OpenCL
/// a bit friendlier for my use cases
struct Open_cl
{
    static std::tuple<bool,cl_platform_id,cl_device_id> setup();

    static cl_context create_context( cl_platform_id platform_id, cl_device_id device_id );
    static cl_command_queue create_command_queue( cl_context context, cl_device_id device_id );
    static cl_program create_program( const char* source, cl_context context, cl_device_id device_id );
    static cl_kernel create_kernel( cl_program program, const char* kernel_name );
    static cl_mem create_buffer( cl_context context, cl_mem_flags flags, size_t size );
    static void set_kernel_arg( cl_kernel kernel, cl_uint index, size_t size, const void* arg );
    static void enqueue_write_buffer( cl_command_queue command_queue, cl_mem buffer, cl_bool blocking, size_t size, const void *ptr );
    static void enqueue_read_buffer( cl_command_queue command_queue, cl_mem buffer, cl_bool blocking, size_t size, void *ptr );
    static void enqueue_nd_range_kernel( cl_command_queue command_queue, cl_kernel kernel, size_t *global_work_size, size_t *local_work_size );
};

}}

#endif

