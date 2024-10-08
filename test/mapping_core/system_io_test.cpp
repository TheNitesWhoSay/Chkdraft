#include <gtest/gtest.h>
#include <mapping_core/mapping_core.h>
#include <cross_cut/simple_icu.h>
#include "test_assets.h"
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
    EXPECT_TRUE(hasExtension("one\\two\\three.exe", "exe"));
    EXPECT_TRUE(hasExtension("one/two\\three.exe", "exe"));
    EXPECT_TRUE(hasExtension("one\\two/three.exe", "exe"));
    EXPECT_FALSE(hasExtension("one/two/three.png", "exe"));
    EXPECT_FALSE(hasExtension("one\\two\\three.png", "exe"));
    EXPECT_FALSE(hasExtension("one/two\\three.png", "exe"));
    EXPECT_FALSE(hasExtension("one\\two/three.png", "exe"));
    
    EXPECT_FALSE(hasExtension("asdf", "txt"));
    EXPECT_FALSE(hasExtension("", "txt"));
    EXPECT_FALSE(hasExtension("asdf", ".txt"));
    EXPECT_FALSE(hasExtension("", ".txt"));
}

TEST(SystemIoTest, FixSystemPathSeparators)
{
    const std::string pathSeparator = getSystemPathSeparator();
    const std::string altPathSeparator = pathSeparator.compare("\\") == 0 ? "/" : "\\";
    const std::string regularPath = "C:" + pathSeparator + "dir" + pathSeparator + "file.txt";
    const std::string altRegularPath = "C:" + altPathSeparator + "dir" + altPathSeparator + "file.txt";
    const std::string mixedRegularPath = "C:" + pathSeparator + "dir" + pathSeparator + "file.txt";
    const std::string mixedMultiSepPath = "C:" + pathSeparator + altPathSeparator + "dir" + pathSeparator + pathSeparator + "file.txt";
    std::string fixedRegularPath = fixSystemPathSeparators(regularPath);
    std::string fixedAltRegularPath = fixSystemPathSeparators(altRegularPath);
    std::string fixedMixedRegularPath = fixSystemPathSeparators(mixedRegularPath);
    std::string fixedMixedMultiSepPath = fixSystemPathSeparators(mixedMultiSepPath);
    EXPECT_STREQ(fixedRegularPath.c_str(), regularPath.c_str());
    EXPECT_STREQ(fixedAltRegularPath.c_str(), regularPath.c_str());
    EXPECT_STREQ(fixedMixedRegularPath.c_str(), regularPath.c_str());
    EXPECT_STREQ(fixedMixedMultiSepPath.c_str(), regularPath.c_str());
}

TEST(SystemIoTest, GetSystemPathSeparator)
{
#ifdef WINDOWS_MULTIBYTE
    const std::string systemPathSeparator = getSystemPathSeparator();
    EXPECT_FALSE(systemPathSeparator.empty());
    EXPECT_EQ(systemPathSeparator.length(), 1);
    icux::filestring separatorAsFileString = icux::toFilestring(systemPathSeparator);
    EXPECT_FALSE(separatorAsFileString.empty());
    EXPECT_EQ(separatorAsFileString.length(), 1);
    auto preferredSeparator = icux::toUtf8(std::wstring(1, std::filesystem::path::preferred_separator));
    EXPECT_EQ(1, preferredSeparator.length());
    EXPECT_EQ(separatorAsFileString[0], preferredSeparator[0]);
#else
    constexpr bool typeMatchesCodepoint = std::is_same_v<
        std::remove_const_t<icux::codepoint>,
        std::remove_const_t<decltype(std::filesystem::path::preferred_separator)>>;
    EXPECT_TRUE(typeMatchesCodepoint);
    const std::string systemPathSeparator = getSystemPathSeparator();
    EXPECT_FALSE(systemPathSeparator.empty());
    EXPECT_EQ(systemPathSeparator.length(), 1);
    icux::filestring separatorAsFileString = icux::toFilestring(systemPathSeparator);
    EXPECT_FALSE(separatorAsFileString.empty());
    EXPECT_EQ(separatorAsFileString.length(), 1);
    EXPECT_EQ(separatorAsFileString[0], std::filesystem::path::preferred_separator);
#endif
}

TEST(SystemIoTest, GetSystemFileName)
{
    const std::string pathSeparator = getSystemPathSeparator();
    const std::string altPathSeparator = pathSeparator.compare("\\") == 0 ? "/" : "\\";
    EXPECT_STREQ("file.txt", getSystemFileName("C:" + pathSeparator + "dir" + pathSeparator + "file.txt").c_str());
    EXPECT_STREQ("file.txt", getSystemFileName(pathSeparator + "dir" + pathSeparator + "file.txt").c_str());
    EXPECT_STREQ("file.txt", getSystemFileName("dir" + pathSeparator + "file.txt").c_str());
    EXPECT_STREQ("file.txt", getSystemFileName(pathSeparator + "file.txt").c_str());
    EXPECT_STREQ("file.txt", getSystemFileName("file.txt").c_str());
    EXPECT_STREQ("file.txt", getSystemFileName("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file.txt").c_str());
    EXPECT_STREQ("file.txt", getSystemFileName("some" + altPathSeparator + "longer"
        + pathSeparator + " directory" + altPathSeparator + "file.txt").c_str());
    
    EXPECT_STREQ("file", getSystemFileName("C:" + pathSeparator + "dir" + pathSeparator + "file").c_str());
    EXPECT_STREQ("file", getSystemFileName(pathSeparator + "dir" + pathSeparator + "file").c_str());
    EXPECT_STREQ("file", getSystemFileName("dir" + pathSeparator + "file").c_str());
    EXPECT_STREQ("file", getSystemFileName(pathSeparator + "file").c_str());
    EXPECT_STREQ("file", getSystemFileName("file").c_str());
    EXPECT_STREQ("file", getSystemFileName("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file").c_str());
    EXPECT_STREQ("file", getSystemFileName("some" + pathSeparator + "longer"
        + altPathSeparator + " directory" + pathSeparator + "file").c_str());
    
    EXPECT_TRUE(getSystemFileName("").empty());
    EXPECT_TRUE(getSystemFileName(pathSeparator).empty());
    EXPECT_TRUE(getSystemFileName("C:" + pathSeparator + "dir" + pathSeparator).empty());
    EXPECT_TRUE(getSystemFileName(pathSeparator + "dir" + pathSeparator).empty());
    EXPECT_TRUE(getSystemFileName("dir" + pathSeparator).empty());
    EXPECT_TRUE(getSystemFileName(pathSeparator).empty());
    EXPECT_TRUE(getSystemFileName("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator).empty());
    EXPECT_TRUE(getSystemFileName("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + altPathSeparator).empty());
}

TEST(SystemIoTest, GetSystemFileExtension)
{
    const std::string pathSeparator = getSystemPathSeparator();
    const std::string altPathSeparator = pathSeparator.compare("\\") == 0 ? "/" : "\\";
    
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
    EXPECT_STREQ(".exe", getSystemFileExtension("one\\two\\three.exe", true).c_str());
    EXPECT_STREQ(".png", getSystemFileExtension("one/two/three.png", true).c_str());
    EXPECT_STREQ(".png", getSystemFileExtension("one\\two\\three.png", true).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("asdf", true).empty());
    EXPECT_TRUE(getSystemFileExtension("", true).empty());

    EXPECT_STREQ(".txt", getSystemFileExtension("C:" + pathSeparator + "dir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension(pathSeparator + "dir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("dir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension(pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file.txt", true).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("C:" + pathSeparator + "dir" + pathSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator + "dir" + pathSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension("dir" + pathSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension("file", true).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + altPathSeparator + "file", true).empty());
    
    EXPECT_TRUE(getSystemFileExtension("", true).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("C:" + pathSeparator + "dir" + pathSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator + "dir" + pathSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("dir" + pathSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + pathSeparator + "longer"
        + altPathSeparator + " directory" + pathSeparator, true).empty());

    EXPECT_STREQ(".txt", getSystemFileExtension("C:" + pathSeparator + "d.ir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension(pathSeparator + "d.ir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("d.ir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " di.rectory" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getSystemFileExtension("some" + altPathSeparator + "longer"
        + altPathSeparator + " di.rectory" + pathSeparator + "file.txt", true).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("C:" + pathSeparator + "d.ir" + pathSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator + "d.ir" + pathSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension("d.ir" + pathSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file", true).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + altPathSeparator + "longer"
        + pathSeparator + " directory" + altPathSeparator + "file", true).empty());
    
    EXPECT_TRUE(getSystemFileExtension("", true).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("C:" + pathSeparator + "d.ir" + pathSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator + "d.ir" + pathSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("d.ir" + pathSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " dir.ectory" + pathSeparator, true).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + altPathSeparator + "longer"
        + altPathSeparator + " dir.ectory" + pathSeparator, true).empty());
    
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
    EXPECT_STREQ("exe", getSystemFileExtension("one\\two\\three.exe", false).c_str());
    EXPECT_STREQ("png", getSystemFileExtension("one/two/three.png", false).c_str());
    EXPECT_STREQ("png", getSystemFileExtension("one\\two\\three.png", false).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("asdf", false).empty());
    EXPECT_TRUE(getSystemFileExtension("", false).empty());

    EXPECT_STREQ("txt", getSystemFileExtension("C:" + pathSeparator + "dir" + pathSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension(pathSeparator + "dir" + pathSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("dir" + pathSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension(pathSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file.txt", false).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("C:" + pathSeparator + "dir" + pathSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator + "dir" + pathSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension("dir" + pathSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension("file", false).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file", false).empty());
    
    EXPECT_TRUE(getSystemFileExtension("", false).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("C:" + pathSeparator + "dir" + pathSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator + "dir" + pathSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("dir" + pathSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator, false).empty());

    EXPECT_STREQ("txt", getSystemFileExtension("C:" + pathSeparator + "d.ir" + pathSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension(pathSeparator + "d.ir" + pathSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("d.ir" + pathSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " di.rectory" + pathSeparator + "file.txt", false).c_str());
    
    EXPECT_TRUE(getSystemFileExtension("C:" + pathSeparator + "d.ir" + pathSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator + "d.ir" + pathSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension("d.ir" + pathSeparator + "file", false).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file", false).empty());
    
    EXPECT_TRUE(getSystemFileExtension("", false).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("C:" + pathSeparator + "d.ir" + pathSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator + "d.ir" + pathSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("d.ir" + pathSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension(pathSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " dir.ectory" + pathSeparator, false).empty());
    EXPECT_TRUE(getSystemFileExtension("some" + altPathSeparator + "longer"
        + altPathSeparator + " dir.ectory" + altPathSeparator, false).empty());
}

TEST(SystemIoTest, GetSystemFileDirectory)
{
    const std::string pathSeparator = getSystemPathSeparator();
    const std::string altPathSeparator = pathSeparator.compare("\\") == 0 ? "/" : "\\";
    EXPECT_STREQ(("C:" + pathSeparator + "dir" + pathSeparator).c_str(), getSystemFileDirectory("C:" + pathSeparator + "dir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ((pathSeparator + "dir" + pathSeparator).c_str(), getSystemFileDirectory(pathSeparator + "dir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(("dir" + pathSeparator).c_str(), getSystemFileDirectory("dir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(pathSeparator.c_str(), getSystemFileDirectory(pathSeparator + "file.txt", true).c_str());
    EXPECT_TRUE(getSystemFileDirectory("file.txt", true).empty());
    EXPECT_STREQ(("some" + pathSeparator + "longer" + pathSeparator + " directory" + pathSeparator).c_str(),
        getSystemFileDirectory("some" + pathSeparator + "longer" + pathSeparator + " directory" + pathSeparator + "file.txt", true).c_str());
    
    EXPECT_STREQ(("C:" + pathSeparator + "dir" + pathSeparator).c_str(), getSystemFileDirectory("C:" + pathSeparator + "dir" + pathSeparator + "file", true).c_str());
    EXPECT_STREQ((pathSeparator + "dir" + pathSeparator).c_str(), getSystemFileDirectory(pathSeparator + "dir" + pathSeparator + "file", true).c_str());
    EXPECT_STREQ(("dir" + pathSeparator).c_str(), getSystemFileDirectory("dir" + pathSeparator + "file", true).c_str());
    EXPECT_STREQ(pathSeparator.c_str(), getSystemFileDirectory(pathSeparator + "file", true).c_str());
    EXPECT_TRUE(getSystemFileDirectory("file", true).empty());
    EXPECT_STREQ(("some" + pathSeparator + "longer" + pathSeparator + " directory" + pathSeparator).c_str(),
        getSystemFileDirectory("some" + pathSeparator + "longer" + pathSeparator + " directory" + pathSeparator + "file", true).c_str());
    EXPECT_STREQ(("some" + pathSeparator + "longer" + pathSeparator + " directory" + pathSeparator).c_str(),
        getSystemFileDirectory("some" + altPathSeparator + "longer" + altPathSeparator + " directory" + altPathSeparator + "file", true).c_str());
    
    EXPECT_TRUE(getSystemFileDirectory("").empty());
    EXPECT_STREQ(pathSeparator.c_str(), getSystemFileDirectory(pathSeparator, true).c_str());
    EXPECT_STREQ(("C:" + pathSeparator + "dir" + pathSeparator).c_str(), getSystemFileDirectory("C:" + pathSeparator + "dir" + pathSeparator, true).c_str());
    EXPECT_STREQ((pathSeparator + "dir" + pathSeparator).c_str(), getSystemFileDirectory(pathSeparator + "dir" + pathSeparator, true).c_str());
    EXPECT_STREQ(("dir" + pathSeparator).c_str(), getSystemFileDirectory("dir" + pathSeparator, true).c_str());
    EXPECT_STREQ((pathSeparator).c_str(), getSystemFileDirectory(pathSeparator, true).c_str());
    EXPECT_STREQ(("some" + pathSeparator + "longer" + pathSeparator + " directory" + pathSeparator).c_str(),
        getSystemFileDirectory("some" + pathSeparator + "longer" + pathSeparator + " directory" + pathSeparator, true).c_str());
}

TEST(SystemIoTest, MakeSystemFilePath)
{
    const std::string pathSeparator = getSystemPathSeparator();
    const std::string altPathSeparator = pathSeparator.compare("\\") == 0 ? "/" : "\\";
    EXPECT_STREQ(std::string("C:" + pathSeparator + "dir" + pathSeparator + "name").c_str(),
        makeSystemFilePath("C:" + pathSeparator + "dir" + pathSeparator, "name").c_str());
    EXPECT_STREQ(std::string("C:" + pathSeparator + "dir" + pathSeparator + "name").c_str(),
        makeSystemFilePath("C:" + altPathSeparator + "dir" + altPathSeparator, "name").c_str());
    
    EXPECT_STREQ(std::string("C:" + pathSeparator + "dir" + pathSeparator + "name").c_str(),
        makeSystemFilePath("C:" + pathSeparator + "dir", "name").c_str());
    EXPECT_STREQ(std::string("C:" + pathSeparator + "dir" + pathSeparator + "name").c_str(),
        makeSystemFilePath("C:" + altPathSeparator + "dir", "name").c_str());
}

TEST(SystemIoTest, MakeExtSystemFilePath)
{
    const std::string pathSeparator = getSystemPathSeparator();
    const std::string altPathSeparator = pathSeparator.compare("\\") == 0 ? "/" : "\\";
    EXPECT_STREQ(std::string("C:" + pathSeparator + "dir" + pathSeparator + "name.txt").c_str(),
        makeExtSystemFilePath("C:" + pathSeparator + "dir" + pathSeparator + "name", ".txt").c_str());
    EXPECT_STREQ(std::string("C:" + pathSeparator + "dir" + pathSeparator + "name.txt").c_str(),
        makeExtSystemFilePath("C:" + altPathSeparator + "dir" + altPathSeparator + "name", ".txt").c_str());
    
    EXPECT_STREQ(std::string("C:" + pathSeparator + "dir" + pathSeparator + "name.txt").c_str(),
        makeExtSystemFilePath("C:" + pathSeparator + "dir" + pathSeparator, "name", ".txt").c_str());
    EXPECT_STREQ(std::string("C:" + pathSeparator + "dir" + pathSeparator + "name.txt").c_str(),
        makeExtSystemFilePath("C:" + altPathSeparator + "dir" + altPathSeparator,  "name", ".txt").c_str());
    
    EXPECT_STREQ(std::string("C:" + pathSeparator + "dir" + pathSeparator + "name.txt").c_str(),
        makeExtSystemFilePath("C:" + pathSeparator + "dir", "name", ".txt").c_str());
    EXPECT_STREQ(std::string("C:" + pathSeparator + "dir" + pathSeparator + "name.txt").c_str(),
        makeExtSystemFilePath("C:" + altPathSeparator + "dir",  "name", ".txt").c_str());
}


TEST(SystemIoTest, FixMpqPathSeparators)
{
    const std::string pathSeparator = getArchivePathSeparator();
    const std::string regularPath = "C:" + pathSeparator + "dir" + pathSeparator + "file.txt";
    const std::string mixedRegularPath = "C:" + pathSeparator + "dir" + pathSeparator + "file.txt";
    std::string fixedRegularPath = fixArchivePathSeparators(regularPath);
    std::string fixedMixedRegularPath = fixArchivePathSeparators(regularPath);
    EXPECT_STREQ(fixedRegularPath.c_str(), regularPath.c_str());
    EXPECT_STREQ(fixedMixedRegularPath.c_str(), regularPath.c_str());
}

TEST(SystemIoTest, GetMpqPathSeparator)
{
    EXPECT_STREQ("\\", getArchivePathSeparator().c_str());
}

TEST(SystemIoTest, GetMpqFileName)
{
    const std::string pathSeparator = getArchivePathSeparator();
    EXPECT_STREQ("file.txt", getArchiveFileName(pathSeparator + "dir" + pathSeparator + "file.txt").c_str());
    EXPECT_STREQ("file.txt", getArchiveFileName("dir" + pathSeparator + "file.txt").c_str());
    EXPECT_STREQ("file.txt", getArchiveFileName(pathSeparator + "file.txt").c_str());
    EXPECT_STREQ("file.txt", getArchiveFileName("file.txt").c_str());
    EXPECT_STREQ("file.txt", getArchiveFileName("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file.txt").c_str());
    
    EXPECT_STREQ("file", getArchiveFileName(pathSeparator + "dir" + pathSeparator + "file").c_str());
    EXPECT_STREQ("file", getArchiveFileName("dir" + pathSeparator + "file").c_str());
    EXPECT_STREQ("file", getArchiveFileName(pathSeparator + "file").c_str());
    EXPECT_STREQ("file", getArchiveFileName("file").c_str());
    EXPECT_STREQ("file", getArchiveFileName("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file").c_str());
    
    EXPECT_TRUE(getArchiveFileName("").empty());
    EXPECT_TRUE(getArchiveFileName(pathSeparator).empty());
    EXPECT_TRUE(getArchiveFileName(pathSeparator + "dir" + pathSeparator).empty());
    EXPECT_TRUE(getArchiveFileName("dir" + pathSeparator).empty());
    EXPECT_TRUE(getArchiveFileName(pathSeparator).empty());
    EXPECT_TRUE(getArchiveFileName("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator).empty());
}

TEST(SystemIoTest, GetMpqFileExtension)
{
    const std::string pathSeparator = getArchivePathSeparator();
    
    EXPECT_STREQ(".txt", getArchiveFileExtension("a.txt", true).c_str());
    EXPECT_STREQ(".txt", getArchiveFileExtension("a.b.txt", true).c_str());
    EXPECT_TRUE(getArchiveFileExtension("atxt", true).empty());
    EXPECT_STREQ(".pdf", getArchiveFileExtension("a.pdf", true).c_str());
    EXPECT_STREQ(".pdf", getArchiveFileExtension("a.b.pdf", true).c_str());
    
    EXPECT_STREQ(".txt", getArchiveFileExtension("something.txt", true).c_str());
    EXPECT_STREQ(".txt", getArchiveFileExtension("something.else.txt", true).c_str());
    EXPECT_STREQ(".txt", getArchiveFileExtension("something.b.txt", true).c_str());
    EXPECT_TRUE(getArchiveFileExtension("somethingtxt", true).empty());
    EXPECT_STREQ(".pdf", getArchiveFileExtension("something.pdf", true).c_str());
    EXPECT_STREQ(".pdf", getArchiveFileExtension("something.else.pdf", true).c_str());
    
    EXPECT_TRUE(getArchiveFileExtension("txt", true).empty());
    EXPECT_STREQ(".txt", getArchiveFileExtension(".txt", true).c_str());

    EXPECT_STREQ(".exe", getArchiveFileExtension("one/two/three.exe", true).c_str());
    EXPECT_STREQ(".exe", getArchiveFileExtension("one\\two\\three.exe", true).c_str());
    EXPECT_STREQ(".png", getArchiveFileExtension("one/two/three.png", true).c_str());
    EXPECT_STREQ(".png", getArchiveFileExtension("one\\two\\three.png", true).c_str());
    
    EXPECT_TRUE(getArchiveFileExtension("asdf", true).empty());
    EXPECT_TRUE(getArchiveFileExtension("", true).empty());

    EXPECT_STREQ(".txt", getArchiveFileExtension(pathSeparator + "dir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getArchiveFileExtension("dir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getArchiveFileExtension(pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getArchiveFileExtension("file.txt", true).c_str());
    EXPECT_STREQ(".txt", getArchiveFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file.txt", true).c_str());
    
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator + "dir" + pathSeparator + "file", true).empty());
    EXPECT_TRUE(getArchiveFileExtension("dir" + pathSeparator + "file", true).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator + "file", true).empty());
    EXPECT_TRUE(getArchiveFileExtension("file", true).empty());
    EXPECT_TRUE(getArchiveFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file", true).empty());
    
    EXPECT_TRUE(getArchiveFileExtension("", true).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator, true).empty());\
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator + "dir" + pathSeparator, true).empty());
    EXPECT_TRUE(getArchiveFileExtension("dir" + pathSeparator, true).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator, true).empty());
    EXPECT_TRUE(getArchiveFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator, true).empty());

    EXPECT_STREQ(".txt", getArchiveFileExtension(pathSeparator + "d.ir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getArchiveFileExtension("d.ir" + pathSeparator + "file.txt", true).c_str());
    EXPECT_STREQ(".txt", getArchiveFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " di.rectory" + pathSeparator + "file.txt", true).c_str());
    
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator + "d.ir" + pathSeparator + "file", true).empty());
    EXPECT_TRUE(getArchiveFileExtension("d.ir" + pathSeparator + "file", true).empty());
    EXPECT_TRUE(getArchiveFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file", true).empty());
    
    EXPECT_TRUE(getArchiveFileExtension("", true).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator, true).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator + "d.ir" + pathSeparator, true).empty());
    EXPECT_TRUE(getArchiveFileExtension("d.ir" + pathSeparator, true).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator, true).empty());
    EXPECT_TRUE(getArchiveFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " dir.ectory" + pathSeparator, true).empty());
    
    // Same tests, but do not include trailing separator
    EXPECT_STREQ("txt", getArchiveFileExtension("a.txt", false).c_str());
    EXPECT_STREQ("txt", getArchiveFileExtension("a.b.txt", false).c_str());
    EXPECT_TRUE(getArchiveFileExtension("atxt", false).empty());
    EXPECT_STREQ("pdf", getArchiveFileExtension("a.pdf", false).c_str());
    EXPECT_STREQ("pdf", getArchiveFileExtension("a.b.pdf", false).c_str());
    
    EXPECT_STREQ("txt", getArchiveFileExtension("something.txt", false).c_str());
    EXPECT_STREQ("txt", getArchiveFileExtension("something.else.txt", false).c_str());
    EXPECT_STREQ("txt", getArchiveFileExtension("something.b.txt", false).c_str());
    EXPECT_TRUE(getArchiveFileExtension("somethingtxt", false).empty());
    EXPECT_STREQ("pdf", getArchiveFileExtension("something.pdf", false).c_str());
    EXPECT_STREQ("pdf", getArchiveFileExtension("something.else.pdf", false).c_str());
    
    EXPECT_TRUE(getArchiveFileExtension("txt", false).empty());
    EXPECT_STREQ("txt", getArchiveFileExtension(".txt", false).c_str());

    EXPECT_STREQ("exe", getArchiveFileExtension("one/two/three.exe", false).c_str());
    EXPECT_STREQ("exe", getArchiveFileExtension("one\\two\\three.exe", false).c_str());
    EXPECT_STREQ("png", getArchiveFileExtension("one/two/three.png", false).c_str());
    EXPECT_STREQ("png", getArchiveFileExtension("one\\two\\three.png", false).c_str());
    
    EXPECT_TRUE(getArchiveFileExtension("asdf", false).empty());
    EXPECT_TRUE(getArchiveFileExtension("", false).empty());

    EXPECT_STREQ("txt", getArchiveFileExtension(pathSeparator + "dir" + pathSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getArchiveFileExtension("dir" + pathSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getArchiveFileExtension(pathSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getArchiveFileExtension("file.txt", false).c_str());
    EXPECT_STREQ("txt", getArchiveFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file.txt", false).c_str());
    
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator + "dir" + pathSeparator + "file", false).empty());
    EXPECT_TRUE(getArchiveFileExtension("dir" + pathSeparator + "file", false).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator + "file", false).empty());
    EXPECT_TRUE(getArchiveFileExtension("file", false).empty());
    EXPECT_TRUE(getArchiveFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file", false).empty());
    
    EXPECT_TRUE(getArchiveFileExtension("", false).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator, false).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator + "dir" + pathSeparator, false).empty());
    EXPECT_TRUE(getArchiveFileExtension("dir" + pathSeparator, false).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator, false).empty());
    EXPECT_TRUE(getArchiveFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator, false).empty());

    EXPECT_STREQ("txt", getArchiveFileExtension(pathSeparator + "d.ir" + pathSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getArchiveFileExtension("d.ir" + pathSeparator + "file.txt", false).c_str());
    EXPECT_STREQ("txt", getArchiveFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " di.rectory" + pathSeparator + "file.txt", false).c_str());
    
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator + "d.ir" + pathSeparator + "file", false).empty());
    EXPECT_TRUE(getArchiveFileExtension("d.ir" + pathSeparator + "file", false).empty());
    EXPECT_TRUE(getArchiveFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " directory" + pathSeparator + "file", false).empty());
    
    EXPECT_TRUE(getArchiveFileExtension("", false).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator, false).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator + "d.ir" + pathSeparator, false).empty());
    EXPECT_TRUE(getArchiveFileExtension("d.ir" + pathSeparator, false).empty());
    EXPECT_TRUE(getArchiveFileExtension(pathSeparator, false).empty());
    EXPECT_TRUE(getArchiveFileExtension("some" + pathSeparator + "longer"
        + pathSeparator + " dir.ectory" + pathSeparator, false).empty());
}

TEST(SystemIoTest, MakeMpqFilePath)
{
    const std::string pathSeparator = getArchivePathSeparator();
    EXPECT_STREQ(std::string("dir" + pathSeparator + "name").c_str(),
        makeArchiveFilePath("dir" + pathSeparator, "name").c_str());
    
    EXPECT_STREQ(std::string("dir" + pathSeparator + "name").c_str(),
        makeArchiveFilePath("dir", "name").c_str());
}

TEST(SystemIoTest, MakeExtMpqFilePath)
{
    const std::string pathSeparator = getArchivePathSeparator();
    EXPECT_STREQ(std::string("dir" + pathSeparator + "name.txt").c_str(),
        makeExtArchiveFilePath("dir" + pathSeparator + "name", ".txt").c_str());
}

TEST(SystemIoTest, FindFile)
{
    EXPECT_TRUE(getEmptyDirectory());
}
