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
