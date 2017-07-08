#include "CppUnitTest.h"

#include <Quaternion.h>
#include <Unit_vector.h>
#include <Utils.h>
#include <Triple.h>
#include <Quaternion_math.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Math;

namespace Math_test {

class Quaternion_test
    : public ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<Quaternion_test> {
public:
    TEST_METHOD(quaternion_construction)
    {
        {
            Quaternion q;
            Assert::IsTrue(equals(q.w(), 0.0f));
            Assert::IsTrue(q.v() == Vector());
        }
        {
            float      w = 34.5;
            Vector     v(12.4f, 876.9f, 98.04f);
            Quaternion q(w, v);
            Assert::IsTrue(equals(q.w(), w));
            Assert::IsTrue(q.v() == v);

            Quaternion q1(q);
            Assert::IsTrue(equals(q1.w(), w));
            Assert::IsTrue(q1.v() == v);
        }
    }

    // This is a pretty stupid test as I've just unrolled the dot and cross product
    // math that makes up the usual quaternion multiplication. But in theory this will
    // be much less tempting to change, so hopefully it should keep the real code honest
    void q_multiply(const Quaternion& a, const Quaternion& b, float& w, Vector& v)
    {
        w = a.w() * b.w() - (a.v().x() * b.v().x() + a.v().y() * b.v().y() + a.v().z() * b.v().z());
        float x =
            a.w() * b.v().x() + b.w() * a.v().x() + (a.v().y() * b.v().z() - a.v().z() * b.v().y());
        float y =
            a.w() * b.v().y() + b.w() * a.v().y() + (a.v().z() * b.v().x() - a.v().x() * b.v().z());
        float z =
            a.w() * b.v().z() + b.w() * a.v().z() + (a.v().x() * b.v().y() - a.v().y() * b.v().x());
        v = Vector(x, y, z);
    }

    // Just like the quaternion math code, this code is cut and pasted into the unit quaternion
    // test (with chnages to make the values magnitude=1 obviously). If you discover a bug
    // here, you may want to change it in both places.
    TEST_METHOD(quaternion_operators)
    {
        float  qw = 3.5f;
        Vector qv(2.4f, 8.9f, 9.04f);
        float  rw = 4.5f;
        Vector rv(2.14f, 6.39f, 8.94f);

        Quaternion q(qw, qv);
        Quaternion q1(q);
        Quaternion r(rw, rv);

        // equality
        Assert::IsTrue(q == q1);
        Assert::IsTrue(q != r);
        q1 = r;
        Assert::IsTrue(q1 == r);

        // addition subtraction
        q1 = q + r;
        Assert::IsTrue(equals(q1.w(), qw + rw));
        Assert::IsTrue(q1.v() == qv + rv);

        q1 = q - r;
        Assert::IsTrue(equals(q1.w(), qw - rw));
        Assert::IsTrue(q1.v() == qv - rv);

        // multiplication
        q1 = q * r;
        float  w;
        Vector v;
        q_multiply(q, r, w, v);
        Assert::IsTrue(equals(q1.w(), w));
        Assert::IsTrue(v == q1.v());

        // multiplication is not Commutative
        Quaternion q2 = r * q;
        Assert::IsTrue(q1 != q2);

        // vector multiplication
        q1 = q.v() * r;
        q_multiply(Quaternion(0, q.v()), r, w, v);
        Assert::IsTrue(equals(q1.w(), w));
        Assert::IsTrue(v == q1.v());

        q1 = r * q.v();
        q_multiply(r, Quaternion(0, q.v()), w, v);
        Assert::IsTrue(equals(q1.w(), w));
        Assert::IsTrue(v == q1.v());

        // scalar multiplication (is Commutative)
        q1 = q * 5.0f;
        Assert::IsTrue(equals(q1.w(), q.w() * 5.0f));
        Assert::IsTrue(q1.v() == q.v() * 5.0f);

        q1 = 5.0f * q;
        Assert::IsTrue(equals(q1.w(), q.w() * 5.0f));
        Assert::IsTrue(q1.v() == q.v() * 5.0f);
    }
};

}  // namespace Math_test
