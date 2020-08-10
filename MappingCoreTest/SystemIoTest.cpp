#include <gtest/gtest.h>
#include "../MappingCoreLib/MappingCore.h"
#include "../IcuLib/SimpleIcu.h"
#include <filesystem>
#include <regex>

TEST(SystemIoTest, HasExtension)
{
    EXPECT_TRUE(hasExtension("a.txt", "txt"));
    EXPECT_TRUE(hasExtension("a.txt", ".txt"));
    EXPECT_TRUE(hasExtension("a.b.txt", "txt"));
    EXPECT_TRUE(hasExtension("a.b.txt", ".txt"));
    EXPECT_FALSE(hasExtension("atxt", "txt"));
    EXPECT_FALSE(hasExtension("atxt", ".txt"));
    EXPECT_FALSE(hasExtension("a.btxt", "txt"));
    EXPECT_FALSE(hasExtension("a.btxt", ".txt"));
    EXPECT_FALSE(hasExtension("a.pdf", "txt"));
    EXPECT_FALSE(hasExtension("a.pdf", ".txt"));
    EXPECT_FALSE(hasExtension("a.b.pdf", "txt"));
    EXPECT_FALSE(hasExtension("a.b.pdf", ".txt"));
    
    EXPECT_TRUE(hasExtension("something.txt", "txt"));
    EXPECT_TRUE(hasExtension("something.txt", ".txt"));
    EXPECT_TRUE(hasExtension("something.else.txt", "txt"));
    EXPECT_TRUE(hasExtension("something.b.txt", ".txt"));
    EXPECT_FALSE(hasExtension("somethingtxt", "txt"));
    EXPECT_FALSE(hasExtension("somethingtxt", ".txt"));
    EXPECT_FALSE(hasExtension("something.elsetxt", "txt"));
    EXPECT_FALSE(hasExtension("something.elsetxt", ".txt"));
    EXPECT_FALSE(hasExtension("something.pdf", "txt"));
    EXPECT_FALSE(hasExtension("something.pdf", ".txt"));
    EXPECT_FALSE(hasExtension("something.else.pdf", "txt"));
    EXPECT_FALSE(hasExtension("something.else.pdf", ".txt"));
    
    EXPECT_FALSE(hasExtension("txt", "txt"));
    EXPECT_FALSE(hasExtension("txt", ".txt"));
    EXPECT_TRUE(hasExtension(".txt", "txt"));
    EXPECT_TRUE(hasExtension(".txt", ".txt"));

    EXPECT_TRUE(hasExtension("one/two/three.exe", "exe"));
    EXPECT_TRUE(hasExtension("one\two\three.exe", "exe"));
    EXPECT_FALSE(hasExtension("one/two/three.exe", "png"));
    EXPECT_FALSE(hasExtension("one\two\three.exe", "png"));
    
    EXPECT_FALSE(hasExtension("asdf", "txt"));
    EXPECT_FALSE(hasExtension("", "txt"));
    EXPECT_FALSE(hasExtension("asdf", ".txt"));
    EXPECT_FALSE(hasExtension("", ".txt"));
}

TEST(SystemIoTest, GetSystemFileSeparator)
{
    constexpr bool typeMatchesCodepoint = std::is_same<std::remove_const<icux::codepoint>::type,
        std::remove_const<decltype(std::filesystem::path::preferred_separator)>::type>::value;
    EXPECT_TRUE(typeMatchesCodepoint);
    const std::string systemFileSeparator = getSystemFileSeparator();
    EXPECT_FALSE(systemFileSeparator.empty());
    EXPECT_EQ(systemFileSeparator.length(), 1);
    icux::filestring separatorAsFileString = icux::toFilestring(systemFileSeparator);
    EXPECT_FALSE(separatorAsFileString.empty());
    EXPECT_EQ(separatorAsFileString.length(), 1);
    EXPECT_EQ(separatorAsFileString[0], std::filesystem::path::preferred_separator);
}

TEST(SystemIoTest, GetSystemFileName)
{
    const std::string fileSeparator = getSystemFileSeparator();
    EXPECT_STREQ("file.txt", getSystemFileName("C:" + fileSeparator + "dir" + fileSeparator + "file.txt").c_str());
    EXPECT_STREQ("file.txt", getSystemFileName(fileSeparator + "dir" + fileSeparator + "file.txt").c_str());
    EXPECT_STREQ("file.txt", getSystemFileName("dir" + fileSeparator + "file.txt").c_str());
    EXPECT_STREQ("file.txt", getSystemFileName(fileSeparator + "file.txt").c_str());
    EXPECT_STREQ("file.txt", getSystemFileName("file.txt").c_str());
    EXPECT_STREQ("file.txt", getSystemFileName("some" + fileSeparator + "longer"
        + fileSeparator + " directory" + fileSeparator + "file.txt").c_str());
    
    EXPECT_STREQ("file", getSystemFileName("C:" + fileSeparator + "dir" + fileSeparator + "file").c_str());
    EXPECT_STREQ("file", getSystemFileName(fileSeparator + "dir" + fileSeparator + "file").c_str());
    EXPECT_STREQ("file", getSystemFileName("dir" + fileSeparator + "file").c_str());
    EXPECT_STREQ("file", getSystemFileName(fileSeparator + "file").c_str());
    EXPECT_STREQ("file", getSystemFileName("file").c_str());
    EXPECT_STREQ("file", getSystemFileName("some" + fileSeparator + "longer"
        + fileSeparator + " directory" + fileSeparator + "file").c_str());
    
    EXPECT_TRUE(getSystemFileName("").empty());
    EXPECT_TRUE(getSystemFileName(fileSeparator).empty());
    EXPECT_TRUE(getSystemFileName("C:" + fileSeparator + "dir" + fileSeparator).empty());
    EXPECT_TRUE(getSystemFileName(fileSeparator + "dir" + fileSeparator).empty());
    EXPECT_TRUE(getSystemFileName("dir" + fileSeparator).empty());
    EXPECT_TRUE(getSystemFileName(fileSeparator).empty());
    EXPECT_TRUE(getSystemFileName("some" + fileSeparator + "longer"
        + fileSeparator + " directory" + fileSeparator).empty());
}

TEST(SystemIoTest, GetSystemFileExtension)
{
    const std::string fileSeparator = getSystemFileSeparator();
    
    EXPECT_STREQ(".txt", getSystemFileExtension("a.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("a.b.txt", true).c_str());
    EXPECT_TRUE(getSystemFileExtension("atxt", true).empty());
    EXPECT_STREQ(".pdf", getSystemFileExtension("a.pdf", true).c_str());
    EXPECT_STREQ(".pdf", getSystemFileExtension("a.b.pdf", true).c_str());
    
    EXPECT_STREQ(".txt", getSystemFileExtension("something.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("something.else.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("something.b.txt", true).c_str());
    EXPECT_TRUE(getSystemFileExtension("somethingtxt", true).empty());
    EXPECT_STREQ(".pdf", getSystemFileExtension("something.pdf", true).c_str());
    EXPECT_STREQ(".pdf", getSystemFileExtension("something.else.pdf", true).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("txt", true).empty());
    EXPECT_STREQ(".txt", getSystemFileExtension(".txt", true).c_str());

    EXPECT_STREQ(".exe", getSystemFileExtension("one/two/three.exe", true).c_str());
    EXPECT_STREQ(".exe", getSystemFileExtension("one\two\three.exe", true).c_str());
    EXPECT_STREQ(".png", getSystemFileExtension("one/two/three.png", true).c_str());
    EXPECT_STREQ(".png", getSystemFileExtension("one\two\three.png", true).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("asdf", true).empty());
    EXPECT_TRUE(getSystemFileExtension("", true).empty());

    EXPECT_STREQ(".txt", getSystemFileExtension("C:" + fileSeparator + "dir" + fileSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension(fileSeparator + "dir" + fileSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("dir" + fileSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension(fileSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("some" + fileSeparator + "longer"
        + fileSeparator + " directory" + fileSeparator + "file.txt", true).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("C:" + fileSeparator + "dir" + fileSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator + "dir" + fileSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension("dir" + fileSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension("file", true).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + fileSeparator + "longer"
        + fileSeparator + " directory" + fileSeparator + "file", true).empty());
    
    EXPECT_TRUE(getSystemFileExtension("", true).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("C:" + fileSeparator + "dir" + fileSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator + "dir" + fileSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("dir" + fileSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + fileSeparator + "longer"
        + fileSeparator + " directory" + fileSeparator, true).empty());

    EXPECT_STREQ(".txt", getSystemFileExtension("C:" + fileSeparator + "d.ir" + fileSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension(fileSeparator + "d.ir" + fileSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("d.ir" + fileSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("some" + fileSeparator + "longer"
        + fileSeparator + " di.rectory" + fileSeparator + "file.txt", true).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("C:" + fileSeparator + "d.ir" + fileSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator + "d.ir" + fileSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension("d.ir" + fileSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + fileSeparator + "longer"
        + fileSeparator + " directory" + fileSeparator + "file", true).empty());
    
    EXPECT_TRUE(getSystemFileExtension("", true).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("C:" + fileSeparator + "d.ir" + fileSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator + "d.ir" + fileSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("d.ir" + fileSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + fileSeparator + "longer"
        + fileSeparator + " dir.ectory" + fileSeparator, true).empty());
    
    // Same tests, but do not include trailing separator
    EXPECT_STREQ("txt", getSystemFileExtension("a.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("a.b.txt", false).c_str());
    EXPECT_TRUE(getSystemFileExtension("atxt", false).empty());
    EXPECT_STREQ("pdf", getSystemFileExtension("a.pdf", false).c_str());
    EXPECT_STREQ("pdf", getSystemFileExtension("a.b.pdf", false).c_str());
    
    EXPECT_STREQ("txt", getSystemFileExtension("something.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("something.else.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("something.b.txt", false).c_str());
    EXPECT_TRUE(getSystemFileExtension("somethingtxt", false).empty());
    EXPECT_STREQ("pdf", getSystemFileExtension("something.pdf", false).c_str());
    EXPECT_STREQ("pdf", getSystemFileExtension("something.else.pdf", false).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("txt", false).empty());
    EXPECT_STREQ("txt", getSystemFileExtension(".txt", false).c_str());

    EXPECT_STREQ("exe", getSystemFileExtension("one/two/three.exe", false).c_str());
    EXPECT_STREQ("exe", getSystemFileExtension("one\two\three.exe", false).c_str());
    EXPECT_STREQ("png", getSystemFileExtension("one/two/three.png", false).c_str());
    EXPECT_STREQ("png", getSystemFileExtension("one\two\three.png", false).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("asdf", false).empty());
    EXPECT_TRUE(getSystemFileExtension("", false).empty());

    EXPECT_STREQ("txt", getSystemFileExtension("C:" + fileSeparator + "dir" + fileSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension(fileSeparator + "dir" + fileSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("dir" + fileSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension(fileSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("some" + fileSeparator + "longer"
        + fileSeparator + " directory" + fileSeparator + "file.txt", false).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("C:" + fileSeparator + "dir" + fileSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator + "dir" + fileSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension("dir" + fileSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension("file", false).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + fileSeparator + "longer"
        + fileSeparator + " directory" + fileSeparator + "file", false).empty());
    
    EXPECT_TRUE(getSystemFileExtension("", false).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("C:" + fileSeparator + "dir" + fileSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator + "dir" + fileSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("dir" + fileSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + fileSeparator + "longer"
        + fileSeparator + " directory" + fileSeparator, false).empty());

    EXPECT_STREQ("txt", getSystemFileExtension("C:" + fileSeparator + "d.ir" + fileSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension(fileSeparator + "d.ir" + fileSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("d.ir" + fileSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("some" + fileSeparator + "longer"
        + fileSeparator + " di.rectory" + fileSeparator + "file.txt", false).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("C:" + fileSeparator + "d.ir" + fileSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator + "d.ir" + fileSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension("d.ir" + fileSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + fileSeparator + "longer"
        + fileSeparator + " directory" + fileSeparator + "file", false).empty());
    
    EXPECT_TRUE(getSystemFileExtension("", false).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("C:" + fileSeparator + "d.ir" + fileSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator + "d.ir" + fileSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("d.ir" + fileSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension(fileSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + fileSeparator + "longer"
        + fileSeparator + " dir.ectory" + fileSeparator, false).empty());
}

TEST(SystemIoTest, GetSystemFileDirectory)
{
    const std::string fileSeparator = getSystemFileSeparator();
    EXPECT_STREQ(("C:" + fileSeparator + "dir" + fileSeparator).c_str(), getSystemFileDirectory("C:" + fileSeparator + "dir" + fileSeparator + "file.txt", true).c_str());
    EXPECT_STREQ((fileSeparator + "dir" + fileSeparator).c_str(), getSystemFileDirectory(fileSeparator + "dir" + fileSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(("dir" + fileSeparator).c_str(), getSystemFileDirectory("dir" + fileSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(fileSeparator.c_str(), getSystemFileDirectory(fileSeparator + "file.txt", true).c_str());
    EXPECT_TRUE(getSystemFileDirectory("file.txt", true).empty());
    EXPECT_STREQ(("some" + fileSeparator + "longer" + fileSeparator + " directory" + fileSeparator).c_str(),
        getSystemFileDirectory("some" + fileSeparator + "longer" + fileSeparator + " directory" + fileSeparator + "file.txt", true).c_str());
    
    EXPECT_STREQ(("C:" + fileSeparator + "dir" + fileSeparator).c_str(), getSystemFileDirectory("C:" + fileSeparator + "dir" + fileSeparator + "file", true).c_str());
    EXPECT_STREQ((fileSeparator + "dir" + fileSeparator).c_str(), getSystemFileDirectory(fileSeparator + "dir" + fileSeparator + "file", true).c_str());
    EXPECT_STREQ(("dir" + fileSeparator).c_str(), getSystemFileDirectory("dir" + fileSeparator + "file", true).c_str());
    EXPECT_STREQ(fileSeparator.c_str(), getSystemFileDirectory(fileSeparator + "file", true).c_str());
    EXPECT_TRUE(getSystemFileDirectory("file", true).empty());
    EXPECT_STREQ(("some" + fileSeparator + "longer" + fileSeparator + " directory" + fileSeparator).c_str(),
        getSystemFileDirectory("some" + fileSeparator + "longer" + fileSeparator + " directory" + fileSeparator + "file", true).c_str());
    
    EXPECT_TRUE(getSystemFileDirectory("").empty());
    EXPECT_STREQ(fileSeparator.c_str(), getSystemFileDirectory(fileSeparator, true).c_str());
    EXPECT_STREQ(("C:" + fileSeparator + "dir" + fileSeparator).c_str(), getSystemFileDirectory("C:" + fileSeparator + "dir" + fileSeparator, true).c_str());
    EXPECT_STREQ((fileSeparator + "dir" + fileSeparator).c_str(), getSystemFileDirectory(fileSeparator + "dir" + fileSeparator, true).c_str());
    EXPECT_STREQ(("dir" + fileSeparator).c_str(), getSystemFileDirectory("dir" + fileSeparator, true).c_str());
    EXPECT_STREQ((fileSeparator).c_str(), getSystemFileDirectory(fileSeparator, true).c_str());
    EXPECT_STREQ(("some" + fileSeparator + "longer" + fileSeparator + " directory" + fileSeparator).c_str(),
        getSystemFileDirectory("some" + fileSeparator + "longer" + fileSeparator + " directory" + fileSeparator, true).c_str());
}

TEST(SystemIoTest, MakeSystemFilePath)
{
    const std::string fileSeparator = getSystemFileSeparator();
    makeSystemFilePath("C:" + fileSeparator + "dir" + fileSeparator, "name");

}
