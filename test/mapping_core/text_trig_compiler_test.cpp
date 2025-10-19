#include <gtest/gtest.h>
#include <mapping_core/mapping_core.h>
#include "test_assets.h"
#include <regex>

TEST(TextTrigCompilerTest, Basic)
{
    TextTrigCompiler ttc(true, 0x0058A364);
}

void TestCircularity(Sc::Data & scData, MapFile mapFile)
{
    TextTrigGenerator ttg(true, 0x0058A364);
    std::string textTrigs;
    EXPECT_TRUE(ttg.generateTextTrigs((Scenario &)mapFile, textTrigs, scData));

    std::string preCompileTextTrigs = textTrigs;

    TextTrigCompiler ttc(true, 0x0058A364);
    ttc.compileTriggers(textTrigs, (Scenario &)mapFile, scData, 0, mapFile.numTriggers());

    EXPECT_TRUE(ttg.generateTextTrigs((Scenario &)mapFile, textTrigs, scData));
    EXPECT_EQ(0, preCompileTextTrigs.compare(textTrigs));
}

TEST(TextTrigCompilerTest, TestMapCircularity)
{
    Sc::Data scData;
    TestAssets::LoadScData(scData);

    std::vector<std::string> testMapFilePaths = {
    };

    for ( auto & testMapFilePath : testMapFilePaths )
    {
        MapFile mapFile(testMapFilePath);
        TestCircularity(scData, testMapFilePath);
    }
}
