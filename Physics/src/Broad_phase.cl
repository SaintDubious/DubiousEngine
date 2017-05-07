// This one is not going to be easy to document, but I'll try my best:
// The basic problem is that given a vector of elements, we want to compare every element pair.
// This part is not so hard, it can be done with a simple loop, like this:
//
// for (size_t i=0; i<vect.size(); ++i) {
//     for (size_t j=i+1; j<vect.size(); ++j) {
//         compare( vect[i], vect[j] );
//     }
// }
//
// The tricky part comes when you're asked, what is i and j for the 100th comparison?
// To solve this you first have to notice that all of the comparisons make up a 
// right triangle. Here's the comparisons we want for 4 objects
// 
// 3
// 2       x
// 1     x x
// 0   x x x
// ---------
//   0 1 2 3
//
// To find the row of the 100th comparison we notice that comparisons 1, 2, and 3 all
// correspond to index 0. Similarly the area under 1 unit along the Y axis is equal to 
// 3.5. If we think about the comparison index as being equal to the area under the y
// axis, we can find y. So what's the area of a right triangle under y? It is:
// area = y * (width-y) + y^2/2
// This ALMOST works for us, but it leaves an awkward y/2 half triangle at the end of
// each row, which skews our result. So the final formula is:
// area = y * (width-y) + y^2/2 - y/2
//
// Rearrange and use the quadratic formula to solve for y and we have the index of the 
// first item in the comparison. From there it's pretty trivial to find the index of 
// the second item.
//
// HOWEVER, we have to use a sqrt, which means we mean floats, which mean we lack
// the precision to work with big numbers. As such we have to do a little correction
// of the result. To correct we just note that the index+1 should be greater then
// the area, and the index itself should be less then the area. We use this to move
// the index up or down a little. 
//
// HOWEVER, even this little fix falls apart for large numbers. From testing I find 
// that the fix starts to become necessary given 1000 items to compare, and stops
// being good enough when we get to about 5000 items to compare.
//
// So the broad_phase_inner kernel works given 5000 or less items to compare. I call
// this "inner" because it's comparing the list with itself. A really large list of
// items can be broken down into smaller items which need an "inner" comparison as
// well as an "outer" (ie compare objects between two lists).
//
const char broad_phase[] = 

"float area_under( float index, float width )\n"
"{\n"
"    return index * (width - index/2.0 - 0.5);\n"
"}\n"

"__kernel void broad_phase_inner( __global const float *items, int num_elements, __global char *result )\n"
"{\n"
"    int global_id = get_global_id(0);\n"
"    if (global_id >= get_global_size(0)) {\n"
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

"    a_i = a_i*4;\n"
"    b_i = b_i*4;\n"
"    float dx = items[a_i+0] - items[b_i+0];\n"
"    float dy = items[a_i+1] - items[b_i+1];\n"
"    float dz = items[a_i+2] - items[b_i+2];\n"
"    float dist_squared = dx*dx + dy*dy + dz*dz;\n"
"    float radius_squared = (items[a_i+3]+items[b_i+3]) * (items[a_i+3]+items[b_i+3]);\n"

"    result[global_id] = (radius_squared > dist_squared);\n"
"}\n"

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