#include "CppUnitTest.h"

#include <File_path.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Utility;

namespace Test_utility {

class File_path_test
    : public ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<File_path_test> {
public:
    TEST_METHOD(file_path_test)
    {
        File_path t1("C:/Windows/System32/File.txt");
        Assert::AreEqual(t1.file(), std::string("File.txt"));
        Assert::AreEqual(t1.path(), std::string("C:/Windows/System32/"));
        Assert::AreEqual(t1.full_path(), std::string("C:/Windows/System32/File.txt"));

        File_path t2("/Windows/System32/File.txt");
        Assert::AreEqual(t2.file(), std::string("File.txt"));
        Assert::AreEqual(t2.path(), std::string("/Windows/System32/"));
        Assert::AreEqual(t2.full_path(), std::string("/Windows/System32/File.txt"));

        File_path t3("Windows/System32/File.txt");
        Assert::AreEqual(t3.file(), std::string("File.txt"));
        Assert::AreEqual(t3.path(), std::string("Windows/System32/"));
        Assert::AreEqual(t3.full_path(), std::string("Windows/System32/File.txt"));

        File_path t4("File.txt");
        Assert::AreEqual(t4.file(), std::string("File.txt"));
        Assert::AreEqual(t4.path(), std::string(""));
        Assert::AreEqual(t4.full_path(), std::string("File.txt"));

        File_path t5("/File.txt");
        Assert::AreEqual(t5.file(), std::string("File.txt"));
        Assert::AreEqual(t5.path(), std::string("/"));
        Assert::AreEqual(t5.full_path(), std::string("/File.txt"));
    }
};
}  // namespace Test_utility