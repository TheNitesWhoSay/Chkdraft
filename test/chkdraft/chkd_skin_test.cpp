#include <chkdraft/mapping/chkd_skin.h>
#include <gtest/gtest.h>
#include <sstream>

TEST(SkinTest, Output)
{
    std::stringstream ss {};
    ss << ChkdSkin::ClassicGDI;
    EXPECT_STREQ(ss.str().c_str(), "ClassicGDI");

    ss = {};
    ss << ChkdSkin::Classic;
    EXPECT_STREQ(ss.str().c_str(), "Classic");

    ss = {};
    ss << ChkdSkin::RemasteredSD;
    EXPECT_STREQ(ss.str().c_str(), "RemasteredSD");

    ss = {};
    ss << ChkdSkin::RemasteredHD2;
    EXPECT_STREQ(ss.str().c_str(), "RemasteredHD2");

    ss = {};
    ss << ChkdSkin::RemasteredHD;
    EXPECT_STREQ(ss.str().c_str(), "RemasteredHD");

    ss = {};
    ss << ChkdSkin::CarbotHD2;
    EXPECT_STREQ(ss.str().c_str(), "CarbotHD2");

    ss = {};
    ss << ChkdSkin::CarbotHD;
    EXPECT_STREQ(ss.str().c_str(), "CarbotHD");

    ss = {};
    ss << ChkdSkin::Total;
    EXPECT_STREQ(ss.str().c_str(), "Classic");

    ss = {};
    ss << ChkdSkin::Unknown;
    EXPECT_STREQ(ss.str().c_str(), "Classic");

    ss = {};
    ss << ChkdSkin(-1);
    EXPECT_STREQ(ss.str().c_str(), "Classic");
}

TEST(SkinTest, Input)
{
    std::stringstream ss {"ClassicGDI"};
    ChkdSkin skin = ChkdSkin::Unknown;
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::ClassicGDI);

    ss = std::stringstream{"CLASSICGDI"};
    skin = ChkdSkin::Unknown;
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::ClassicGDI);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"Classic"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::Classic);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"CLASSIC"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::Classic);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"RemasteredSD"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::RemasteredSD);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"REMASTEREDSD"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::RemasteredSD);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"RemasteredHD2"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::RemasteredHD2);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"REMASTEREDHD2"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::RemasteredHD2);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"RemasteredHD"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::RemasteredHD);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"REMASTEREDHD"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::RemasteredHD);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"CarbotHD2"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::CarbotHD2);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"CARBOTHD2"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::CarbotHD2);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"CarbotHD"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::CarbotHD);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"CARBOTHD"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::CarbotHD);

    skin = ChkdSkin::Unknown;
    ss = std::stringstream{"carbothd"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::CarbotHD);

    skin = ChkdSkin::Classic;
    ss = std::stringstream{"idkLmao"};
    ss >> skin;
    EXPECT_EQ(skin, ChkdSkin::Unknown);
}

TEST(SkinTest, getSkinUserText)
{
    std::string skinUserText = std::string(getSkinUserText(ChkdSkin::ClassicGDI));
    EXPECT_STREQ(skinUserText.c_str(), "Classic GDI");

    skinUserText = std::string(getSkinUserText(ChkdSkin::Classic));
    EXPECT_STREQ(skinUserText.c_str(), "Classic OpenGL");

    skinUserText = std::string(getSkinUserText(ChkdSkin::RemasteredSD));
    EXPECT_STREQ(skinUserText.c_str(), "Remastered SD");

    skinUserText = std::string(getSkinUserText(ChkdSkin::RemasteredHD2));
    EXPECT_STREQ(skinUserText.c_str(), "Remastered HD2");

    skinUserText = std::string(getSkinUserText(ChkdSkin::RemasteredHD));
    EXPECT_STREQ(skinUserText.c_str(), "Remastered HD");

    skinUserText = std::string(getSkinUserText(ChkdSkin::CarbotHD2));
    EXPECT_STREQ(skinUserText.c_str(), "Carbot HD2");

    skinUserText = std::string(getSkinUserText(ChkdSkin::CarbotHD));
    EXPECT_STREQ(skinUserText.c_str(), "Carbot HD");

    skinUserText = std::string(getSkinUserText(ChkdSkin::Total));
    EXPECT_STREQ(skinUserText.c_str(), "Classic OpenGL");

    skinUserText = std::string(getSkinUserText(ChkdSkin::Unknown));
    EXPECT_STREQ(skinUserText.c_str(), "Classic OpenGL");

    skinUserText = std::string(getSkinUserText(ChkdSkin(-1)));
    EXPECT_STREQ(skinUserText.c_str(), "Classic OpenGL");
}
