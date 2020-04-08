#include <gtest/gtest.h>
#include "../MappingCoreLib/MappingCore.h"
#include "TestAssets.h"
#include <regex>

TEST(TextTrigCompilerTest, Basic)
{
    TextTrigCompiler ttc(true, 0x0058A364);
}

void TestCircularity(Sc::Data & scData, MapFile mapFile)
{
    ScenarioPtr scenarioPtr = ScenarioPtr(&((Scenario &)mapFile), [](Scenario*){});

    TextTrigGenerator ttg(true, 0x0058A364);
    std::string textTrigs;
    EXPECT_TRUE(ttg.generateTextTrigs(scenarioPtr, textTrigs));

    std::string preCompileTextTrigs = textTrigs;

    TextTrigCompiler ttc(true, 0x0058A364);
    ttc.compileTriggers(textTrigs, scenarioPtr, scData, 0, mapFile.triggers.numTriggers());

    EXPECT_TRUE(ttg.generateTextTrigs(scenarioPtr, textTrigs));
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
