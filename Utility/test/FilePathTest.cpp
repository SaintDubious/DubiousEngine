#include "CppUnitTest.h"

#include <FilePath.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Dubious::Utility::FilePath;

namespace TestUtility
{		
TEST_CLASS(FilePathTest)
{
public:
        
    TEST_METHOD(FilePathTests)
    {
        FilePath T1( "C:/Windows/System32/File.txt" );
        Assert::AreEqual( T1.File(), std::string("File.txt") );
        Assert::AreEqual( T1.Path(), std::string("C:/Windows/System32/") );
        Assert::AreEqual( T1.FullPath(), std::string("C:/Windows/System32/File.txt") );

        FilePath T2( "/Windows/System32/File.txt" );
        Assert::AreEqual( T2.File(), std::string("File.txt") );
        Assert::AreEqual( T2.Path(), std::string("/Windows/System32/") );
        Assert::AreEqual( T2.FullPath(), std::string("/Windows/System32/File.txt") );

        FilePath T3( "Windows/System32/File.txt" );
        Assert::AreEqual( T3.File(), std::string("File.txt") );
        Assert::AreEqual( T3.Path(), std::string("Windows/System32/") );
        Assert::AreEqual( T3.FullPath(), std::string("Windows/System32/File.txt") );

        FilePath T4( "File.txt" );
        Assert::AreEqual( T4.File(), std::string("File.txt") );
        Assert::AreEqual( T4.Path(), std::string("") );
        Assert::AreEqual( T4.FullPath(), std::string("File.txt") );

        FilePath T5( "/File.txt" );
        Assert::AreEqual( T5.File(), std::string("File.txt") );
        Assert::AreEqual( T5.Path(), std::string("/") );
        Assert::AreEqual( T5.FullPath(), std::string("/File.txt") );
    }

};
}