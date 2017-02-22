#include <Open_cl.h>

#include <iostream>

using namespace Dubious;

const char *program_source = 
"float area_under( float index, float width )\n"
"{\n"
"    return index * (width - index/2.0 - 0.5);\n"
"}\n"
"\n"
"__kernel void foo( int num_elements, __global int *result_a, __global int *result_b )\n"
"{\n"
"    int num_comparisons = get_global_size(0);\n"
"    int global_id = get_global_id(0);\n"
"    if (global_id >= num_comparisons) {\n"
"        return;\n"
"    }\n"
"    int a_i = (num_elements-0.5) - sqrt((num_elements * (num_elements-1)  - (2 * global_id)) + 0.25 );\n"
"    while (area_under(a_i+1, num_elements) <= (float)global_id) {\n"
"      ++a_i;\n"
"    }\n"
"    while (area_under(a_i, num_elements) > (float)global_id) {\n"
"      --a_i;\n"
"    }\n"
"    int b_i = a_i + global_id + 1 + a_i * ((0.5*a_i) - (num_elements-0.5));\n"
"    result_a[global_id] = a_i;\n"
"    result_b[global_id] = b_i;\n"
"}\n"
;

int main( int argc, char** argv )
{
    try {
        std::cout << "starting up\n";
        cl_platform_id platform_id;
        cl_device_id device_id;
        bool opencl_available;
        cl_int rc;
        std::tie(opencl_available,platform_id,device_id) = Utility::Open_cl::setup();
        cl_context context = Utility::Open_cl::create_context( platform_id, device_id );
        cl_command_queue command_queue = Utility::Open_cl::create_command_queue( context, device_id );

        cl_program program;
        std::cout << program_source << "\n";
        program = clCreateProgramWithSource( context, 1, &program_source, NULL, NULL );
        if (program == NULL) {
            throw std::runtime_error( "clCreateProgramWithSource" );
        }
        rc = clBuildProgram( program, 1, &device_id, NULL, NULL, NULL );
        if (CL_SUCCESS != rc) {
            std::cout << "rc = " << rc << "\n";
            throw std::runtime_error( "clBuildProgram" );
        }
        cl_kernel kernel;
        kernel = clCreateKernel( program, "foo", NULL );
        if (kernel == NULL) {
            throw std::runtime_error( "clCreateKernel" );
        }
        int num_elements = 1000;
        int comparison_count = static_cast<int>(((num_elements-1)*num_elements)/2.0);
        std::cout << "comparison_count = " << comparison_count << "\n";

        cl_mem buffer_result_a;
        buffer_result_a = clCreateBuffer( context, CL_MEM_WRITE_ONLY, comparison_count*sizeof(int), NULL, &rc );
        if (CL_SUCCESS != rc) {
            throw std::runtime_error( "Failed to create buffer_result_a" );
        }
        cl_mem buffer_result_b;
        buffer_result_b = clCreateBuffer( context, CL_MEM_WRITE_ONLY, comparison_count*sizeof(int), NULL, &rc );
        if (CL_SUCCESS != rc) {
            throw std::runtime_error( "Failed to create buffer_result_b" );
        }


        rc = clSetKernelArg( kernel, 0, sizeof(cl_int), &num_elements );
        if (CL_SUCCESS != rc) {
            std::cout << "rc = " << rc << "\n";
            throw std::runtime_error( "clSetKernelArg num_elements" );
        }
        rc = clSetKernelArg( kernel, 1, sizeof(cl_mem), &buffer_result_a );
        if (CL_SUCCESS != rc) {
            std::cout << "rc = " << rc << "\n";
            throw std::runtime_error( "clSetKernelArg buffer_result_a" );
        }
        rc = clSetKernelArg( kernel, 2, sizeof(cl_mem), &buffer_result_b );
        if (CL_SUCCESS != rc) {
            std::cout << "rc = " << rc << "\n";
            throw std::runtime_error( "clSetKernelArg buffer_result_b" );
        }


        size_t global_work_size = comparison_count;
        rc = clEnqueueNDRangeKernel( command_queue, kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL );
        if (CL_SUCCESS != rc) {
            std::cout << "rc = " << rc << "\n";
            throw std::runtime_error( "clEnqueueNDRangeKernel" );
        }

        int *result_a = new int[comparison_count];
        clEnqueueReadBuffer( command_queue, buffer_result_a, CL_TRUE, 0, comparison_count*sizeof(int), result_a, 0, NULL, NULL );
        if (CL_SUCCESS != rc) {
            std::cout << "rc = " << rc << "\n";
            throw std::runtime_error( "clEnqueueNDRangeKernel" );
        }
        int *result_b = new int[comparison_count];
        clEnqueueReadBuffer( command_queue, buffer_result_b, CL_TRUE, 0, comparison_count*sizeof(int), result_b, 0, NULL, NULL );
        if (CL_SUCCESS != rc) {
            std::cout << "rc = " << rc << "\n";
            throw std::runtime_error( "clEnqueueNDRangeKernel" );
        }


        int pair_index = 0;
        int k_error_count = 0;
        int l_error_count = 0;
        for (int k=0; k<num_elements; ++k) {
            for (int l=k+1; l<num_elements; ++l) {
                            
                if (result_a[pair_index] != k) {
                    ++k_error_count;
                }
                if (result_b[pair_index] != l) {
                    ++l_error_count;
                }
//                std::cout << result_a[pair_index] << "," << result_b[pair_index] << " ";
                ++pair_index;
            }
//            std::cout << "\n";
        }
        std::cout << "k_error_count = " << k_error_count << "\n";
        std::cout << "l_error_count = " << l_error_count << "\n";

        std::cout << "cleanup\n";
        clReleaseMemObject( buffer_result_a );
        clReleaseMemObject( buffer_result_b );
        delete [] result_a;
        clReleaseKernel( kernel );
        clReleaseProgram( program );
        clReleaseCommandQueue( command_queue );
        clReleaseContext( context );


        std::cout << "closing\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << "\n";
    }
    catch (...) {
        std::cout << "Caught exception\n";
    }
    return -1;
}
