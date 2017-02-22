// Please start by reading the command relating to the broad_phase_inner kernel.
//
// Given a large enough array of objects to compare, we can break it down into
// smaller subsets. However once two subsets complete an "inner" comparison, we 
// still need to compare every object from subset 1 with every object in subset 2.
// This is relatively easy as the comparisons form a square:
//
// D x x x x
// C x x x x
// B x x x x
// A x x x x
// ---------------
//   0 1 2 3
//
// So this "outer" comparison takes the same approach of finding the area, but since
// the area of a square is so trivial, and doesn't require a sqrt (ie we can use ints),
// the result is obvious.
//
const char broad_phase_outer[] = 

"__kernel void broad_phase_outer( __global const float *items_a, __global const float *items_b, int num_b, __global char *result )\n"
"{\n"
"    int global_id = get_global_id(0);\n"
"    if (global_id >= get_global_size(0)) {\n"
"        return;\n"
"    }\n"

"    int a_i = global_id / num_b;\n"
"    int b_i = global_id % num_b;\n"

"    a_i = a_i*4;\n"
"    b_i = b_i*4;\n"
"    float dx = items_a[a_i+0] - items_b[b_i+0];\n"
"    float dy = items_a[a_i+1] - items_b[b_i+1];\n"
"    float dz = items_a[a_i+2] - items_b[b_i+2];\n"
"    float dist_squared = dx*dx + dy*dy + dz*dz;\n"
"    float radius_squared = (items_a[a_i+3]+items_b[b_i+3]) * (items_a[a_i+3]+items_b[b_i+3]);\n"

"    result[global_id] = (radius_squared > dist_squared);\n"
"}\n"
;