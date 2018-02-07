#include "CppUnitTest.h"

#include <Physics_model.h>
#include <Physics_object.h>
#include <Ac3d_file_reader.h>
#include <Collision_strategy_simple.h>
#include <Collision_strategy_multi_threaded.h>
#include <Collision_strategy_open_cl.h>
#include <Contact_manifold.h>

#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Physics;
using namespace Dubious::Math;
using namespace Dubious::Utility;

namespace Physics_test {

class Collision_strategy_test
    : public ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<Collision_strategy_test> {
public:
    TEST_METHOD(collision_strategy_simple)
    {
        std::vector<std::shared_ptr<Physics_object>>                        objects;
        std::map<Collision_strategy::Physics_object_pair, Contact_manifold> manifolds;
        Collision_strategy_simple                                           strategy(0.05f, 0.5f);
        setup_objects(objects);
        strategy.find_contacts(objects, manifolds);
        Assert::IsTrue(verify_result(objects, manifolds));
    }

    TEST_METHOD(collision_strategy_multi_threaded)
    {
        std::vector<std::shared_ptr<Physics_object>>                        objects;
        std::map<Collision_strategy::Physics_object_pair, Contact_manifold> manifolds;
        Collision_strategy_multi_threaded strategy(0.05f, 0.5f, 4);
        setup_objects(objects);
        strategy.find_contacts(objects, manifolds);
        Assert::IsTrue(verify_result(objects, manifolds));
    }

    TEST_METHOD(collision_strategy_open_cl)
    {
        std::vector<std::shared_ptr<Physics_object>>                        objects;
        std::map<Collision_strategy::Physics_object_pair, Contact_manifold> manifolds;
        Collision_strategy_open_cl strategy(0.05f, 0.5f, 4, 8);
        setup_objects(objects);
        strategy.find_contacts(objects, manifolds);
        Assert::IsTrue(verify_result(objects, manifolds));
    }

private:
    void setup_objects(std::vector<std::shared_ptr<Physics_object>>& objects)
    {
        // We want to try to test that collision detection works for the
        // various methods of breaking down the input vector. The OpenCL
        // strategy breaks down the vector into sub-vectors of a length
        // specified, it ALSO breaks down the sub-vectors in order to test
        // collisions between sub-vectors. For this reason we want out test input
        // to be a vector of objects that can be split twice. The resulting 4
        // sub-vectors should demonstrate collisions of objects within each
        // sub-vector as well as between each sub-vector.
        // Therefore:
        // We want the smallest sub-vector to contain 4 objects:
        //   - One which collides with other sub-vectors
        //   - Two which collide with each other
        //   - One which doesn't collide with anything
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube(1.0f, 1.0f, 1.0f);
        std::shared_ptr<Physics_model>   model      = std::make_shared<Physics_model>(*model_file);

        objects.resize(16);
        std::for_each(objects.begin(), objects.end(), [model](std::shared_ptr<Physics_object>& o) {
            o.reset(new Physics_object(model, 1));
        });

        // The first index of every sub-sub-vector collides
        objects[0]->coordinate_space().position()  = Point(10, 10, 10);
        objects[4]->coordinate_space().position()  = Point(10.1f, 10, 10);
        objects[8]->coordinate_space().position()  = Point(10.2f, 10, 10);
        objects[12]->coordinate_space().position() = Point(10.3f, 10, 10);

        // The second and third collide with each other
        objects[1]->coordinate_space().position()  = Point(20, 10, 10);
        objects[2]->coordinate_space().position()  = Point(20.1f, 10, 10);
        objects[5]->coordinate_space().position()  = Point(30, 10, 10);
        objects[6]->coordinate_space().position()  = Point(30.1f, 10, 10);
        objects[9]->coordinate_space().position()  = Point(40, 10, 10);
        objects[10]->coordinate_space().position() = Point(40.1f, 10, 10);
        objects[13]->coordinate_space().position() = Point(50, 10, 10);
        objects[14]->coordinate_space().position() = Point(50.1f, 10, 10);

        // The fourth doesn't collide with anything
        objects[3]->coordinate_space().position()  = Point(60, 10, 10);
        objects[7]->coordinate_space().position()  = Point(70, 10, 10);
        objects[11]->coordinate_space().position() = Point(80, 10, 10);
        objects[15]->coordinate_space().position() = Point(90, 10, 10);
    }

    bool verify_result(
        const std::vector<std::shared_ptr<Physics_object>>&                        objects,
        const std::map<Collision_strategy::Physics_object_pair, Contact_manifold>& manifolds)
    {
        return manifolds.size() == 10 &&
               // The first index of every sub-sub-vector collides
               verify_pair(0, 4, objects, manifolds) && verify_pair(0, 8, objects, manifolds) &&
               verify_pair(0, 12, objects, manifolds) && verify_pair(4, 8, objects, manifolds) &&
               verify_pair(4, 12, objects, manifolds) && verify_pair(8, 12, objects, manifolds) &&
               // The second and third collide with each other
               verify_pair(1, 2, objects, manifolds) && verify_pair(5, 6, objects, manifolds) &&
               verify_pair(9, 10, objects, manifolds) && verify_pair(13, 14, objects, manifolds);
    }

    bool verify_pair(
        size_t a, size_t b, const std::vector<std::shared_ptr<Physics_object>>& objects,
        const std::map<Collision_strategy::Physics_object_pair, Contact_manifold>& manifolds)
    {
        for (const auto& result : manifolds) {
            if ((std::get<0>(result.first) == objects[a].get() &&
                 std::get<1>(result.first) == objects[b].get()) ||
                (std::get<1>(result.first) == objects[a].get() &&
                 std::get<0>(result.first) == objects[b].get())) {
                return true;
            }
        }
        return false;
    }
};
}  // namespace Physics_test
