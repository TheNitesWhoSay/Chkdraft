#include <gtest/gtest.h>
#include "../MappingCoreLib/MappingCore.h"
#include <regex>

/**
    This file purposefully tests several operations that involve truncating values, using underflows and overflows, the following pragmas disable warnings for these
*/
#pragma warning(push)
#pragma warning(disable: 4305) // Disable warnings for '=' truncation of non-constants
#pragma warning(disable: 4307) // Disable warnings for integral constant overflow
#pragma warning(disable: 4309) // Disable warnings for '=' truncation of constant value

TEST(BasicsTest, DataTypeSizes)
{
    EXPECT_EQ(sizeof(u64), 8);
    EXPECT_EQ(sizeof(s64), 8);
    EXPECT_EQ(sizeof(u32), 4);
    EXPECT_EQ(sizeof(s32), 4);
    EXPECT_EQ(sizeof(u16), 2);
    EXPECT_EQ(sizeof(s16), 2);
    EXPECT_EQ(sizeof(u8), 1);
    EXPECT_EQ(sizeof(s8), 1);
}

TEST(BasicsTest, DataTypeSigns)
{
    EXPECT_GT((u64)-1, (u64)0);
    EXPECT_GT((u32)-1, (u32)0);
    EXPECT_GT((u16)-1, (u16)0);
    EXPECT_GT((u8)-1, (u8)0);
    
    EXPECT_LT((s64)-1, (s64)0);
    EXPECT_LT((s32)-1, (s32)0);
    EXPECT_LT((s16)-1, (s16)0);
    EXPECT_LT((s8)-1, (s8)0);
}

TEST(BasicsTest, UnsignedMax)
{
    constexpr u8 u8_0 = 0;
    constexpr u8 u8_1 = 1;
    constexpr u16 u16_0 = 0;
    constexpr u16 u16_1 = 1;
    constexpr u32 u32_0 = 0;
    constexpr u32 u32_1 = 1;
    constexpr u64 u64_0 = 0;
    constexpr u64 u64_1 = 1;

    u8 u8_result = 0;
    u16 u16_result = 0;
    u32 u32_result = 0;
    u64 u64_result = 0;

    // MaxUnsigned > 0
    EXPECT_GT(u8_max, u8_0);
    EXPECT_GT(u16_max, u16_0);
    EXPECT_GT(u32_max, u32_0);
    EXPECT_GT(u64_max, u64_0);

    // Setup MaxUnsigned-1
    u8_result = u8_max - u8_1;
    u16_result = u16_max - u16_1;
    u32_result = u32_max - u32_1;
    u64_result = u64_max - u64_1;

    // MaxUnsigned-1 > 0 (for types larger than 1 bit)
    EXPECT_GT(u8_result, u8_0);
    EXPECT_GT(u16_result, u16_0);
    EXPECT_GT(u32_result, u32_0);
    EXPECT_GT(u64_result, u64_0);
    
    // MaxUnsigned-1 < MaxUnsigned
    EXPECT_LT(u8_result, (u8)u8_max);
    EXPECT_LT(u16_result, (u16)u16_max);
    EXPECT_LT(u32_result, (u32)u32_max);
    EXPECT_LT(u64_result, (u64)u64_max);

    // Setup MaxUnsigned+1
    u8_result = u8_max + u8_1;
    u16_result = u16_max + u16_1;
    u32_result = u32_max + u32_1;
    u64_result = u64_max + u64_1;

    // MaxUnsigned+1 == 0 (overflow to 0)
    EXPECT_EQ(u8_result, u8_0);
    EXPECT_EQ(u16_result, u16_0);
    EXPECT_EQ(u32_result, u32_0);
    EXPECT_EQ(u64_result, u64_0);
    
    // MaxUnsigned+1 < MaxUnsigned (overflow)
    EXPECT_LT(u8_result, u8_max);
    EXPECT_LT(u16_result, u16_max);
    EXPECT_LT(u32_result, u32_max);
    EXPECT_LT(u64_result, u64_max);
}

TEST(BasicsTest, SignedMaxMin)
{
    constexpr s8 s8_0 = 0;
    constexpr s8 s8_1 = 1;
    constexpr s16 s16_0 = 0;
    constexpr s16 s16_1 = 1;
    constexpr s32 s32_0 = 0;
    constexpr s32 s32_1 = 1;
    constexpr s64 s64_0 = 0;
    constexpr s64 s64_1 = 1;
    
    s8 s8_result = 0;
    s16 s16_result = 0;
    s32 s32_result = 0;
    s64 s64_result = 0;

    // MaxSigned > 0
    EXPECT_GT(s8_max, s8_0);
    EXPECT_GT(s16_max, s16_0);
    EXPECT_GT(s32_max, s32_0);
    EXPECT_GT(s64_max, s64_0);

    // MaxSigned > MinSigned
    EXPECT_GT(s8_max, s8_min);
    EXPECT_GT(s16_max, s16_min);
    EXPECT_GT(s32_max, s32_min);
    EXPECT_GT(s64_max, s64_min);

    // Setup MaxSigned-1
    s8_result = s8_max - s8_1;
    s16_result = s16_max - s16_1;
    s32_result = s32_max - s32_1;
    s64_result = s64_max - s64_1;

    // MaxSigned-1 > 0 (for types larger than 1 bit)
    EXPECT_GT(s8_result, s8_0);
    EXPECT_GT(s16_result, s16_0);
    EXPECT_GT(s32_result, s32_0);
    EXPECT_GT(s64_result, s64_0);

    // MaxSigned-1 < MaxSigned
    EXPECT_LT(s8_result, s8_max);
    EXPECT_LT(s16_result, s16_max);
    EXPECT_LT(s32_result, s32_max);
    EXPECT_LT(s64_result, s64_max);

    // MaxSigned-1 > MinSigned (for types larger than 1 bit)
    EXPECT_GT(s8_result, s8_min);
    EXPECT_GT(s16_result, s16_min);
    EXPECT_GT(s32_result, s32_min);
    EXPECT_GT(s64_result, s64_min);

    // Setup MaxSigned+1
    s8_result = s8_max + s8_1;
    s16_result = s16_max + s16_1;
    s32_result = s32_max + s32_1;
    s64_result = s64_max + s64_1;

    // MaxSigned+1 < 0 (overflow to negative, for types larger than 1 bit)
    EXPECT_LT(s8_result, s8_0);
    EXPECT_LT(s16_result, s16_0);
    EXPECT_LT(s32_result, s32_0);
    EXPECT_LT(s64_result, s64_0);

    // MaxSigned+1 < MaxSigned (overflow)
    EXPECT_LT(s8_result, s8_max);
    EXPECT_LT(s16_result, s16_max);
    EXPECT_LT(s32_result, s32_max);
    EXPECT_LT(s64_result, s64_max);

    // MaxSigned+1 == MinSigned (overflow to minimum)
    EXPECT_EQ(s8_result, s8_min);
    EXPECT_EQ(s16_result, s16_min);
    EXPECT_EQ(s32_result, s32_min);
    EXPECT_EQ(s64_result, s64_min);

    // MinSigned < 0 (for types larger than 1 bit)
    EXPECT_LT(s8_min, s8_0);
    EXPECT_LT(s16_min, s16_0);
    EXPECT_LT(s32_min, s32_0);
    EXPECT_LT(s64_min, s64_0);

    // MinSigned < MaxSigned
    EXPECT_LT(s8_min, s8_max);
    EXPECT_LT(s16_min, s16_max);
    EXPECT_LT(s32_min, s32_max);
    EXPECT_LT(s64_min, s64_max);

    // Setup MinSigned-1
    s8_result = s8_min - s8_1;
    s16_result = s16_min - s16_1;
    s32_result = s32_min - s32_1;
    s64_result = s64_min - s64_1;

    // MinSigned-1 > 0 (for types larger than 1 bit)
    EXPECT_GT(s8_result, (s8)0);
    EXPECT_GT(s16_result, (s16)0);
    EXPECT_GT(s32_result, (s32)0);
    EXPECT_GT(s64_result, (s64)0);

    // MinSigned-1 > MinSigned (underflow)
    EXPECT_GT(s8_result, s8_min);
    EXPECT_GT(s16_result, s16_min);
    EXPECT_GT(s32_result, s32_min);
    EXPECT_GT(s64_result, s64_min);

    // MinSigned-1 == MaxSigned (underflow to maximum)
    EXPECT_EQ(s8_result, s8_max);
    EXPECT_EQ(s16_result, s16_max);
    EXPECT_EQ(s32_result, s32_max);
    EXPECT_EQ(s64_result, s64_max);

    // Setup MinSigned+1
    s8_result = s8_min + s8_1;
    s16_result = s16_min + s16_1;
    s32_result = s32_min + s32_1;
    s64_result = s64_min + s64_1;

    // MinSigned+1 < 0 (for types larger than 1 bit)
    EXPECT_LT(s8_result, s8_0);
    EXPECT_LT(s16_result, s16_0);
    EXPECT_LT(s32_result, s32_0);
    EXPECT_LT(s64_result, s64_0);

    // MinSigned+1 > MinSigned
    EXPECT_GT(s8_result, s8_min);
    EXPECT_GT(s16_result, s16_min);
    EXPECT_GT(s32_result, s32_min);
    EXPECT_GT(s64_result, s64_min);

    // MinSigned+1 < MaxSigned (for types larger than 1 bit)
    EXPECT_LT(s8_result, s8_max);
    EXPECT_LT(s16_result, s16_max);
    EXPECT_LT(s32_result, s32_max);
    EXPECT_LT(s64_result, s64_max);
}

TEST(BasicsTest, Bits)
{
    u8 u8_0 = 0;

    EXPECT_TRUE((u8_0 | BIT_0) == BIT_0);
    EXPECT_TRUE((u8_0 | BIT_1) == BIT_1);
    EXPECT_TRUE((u8_0 | BIT_2) == BIT_2);
    EXPECT_TRUE((u8_0 | BIT_3) == BIT_3);
    EXPECT_TRUE((u8_0 | BIT_4) == BIT_4);
    EXPECT_TRUE((u8_0 | BIT_5) == BIT_5);
    EXPECT_TRUE((u8_0 | BIT_6) == BIT_6);
    EXPECT_TRUE((u8_0 | BIT_7) == BIT_7);

    EXPECT_TRUE((u8_0 | BIT_0 | BIT_1) == BIT_0 + BIT_1);
    EXPECT_TRUE((u8_0 | BIT_1 | BIT_2) == BIT_1 + BIT_2);
    EXPECT_TRUE((u8_0 | BIT_2 | BIT_3) == BIT_2 + BIT_3);
    EXPECT_TRUE((u8_0 | BIT_3 | BIT_4) == BIT_3 + BIT_4);
    EXPECT_TRUE((u8_0 | BIT_4 | BIT_5) == BIT_4 + BIT_5);
    EXPECT_TRUE((u8_0 | BIT_5 | BIT_6) == BIT_5 + BIT_6);
    EXPECT_TRUE((u8_0 | BIT_6 | BIT_7) == BIT_6 + BIT_7);

    EXPECT_TRUE((u8_0 | BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6 | BIT_7) == u8_max);
    
    u16 u16_0 = 0;
    
    EXPECT_TRUE((u16_0 | BIT_0) == BIT_0);
    EXPECT_TRUE((u16_0 | BIT_1) == BIT_1);
    EXPECT_TRUE((u16_0 | BIT_2) == BIT_2);
    EXPECT_TRUE((u16_0 | BIT_3) == BIT_3);
    EXPECT_TRUE((u16_0 | BIT_4) == BIT_4);
    EXPECT_TRUE((u16_0 | BIT_5) == BIT_5);
    EXPECT_TRUE((u16_0 | BIT_6) == BIT_6);
    EXPECT_TRUE((u16_0 | BIT_7) == BIT_7);
    EXPECT_TRUE((u16_0 | BIT_8) == BIT_8);
    EXPECT_TRUE((u16_0 | BIT_9) == BIT_9);
    EXPECT_TRUE((u16_0 | BIT_10) == BIT_10);
    EXPECT_TRUE((u16_0 | BIT_11) == BIT_11);
    EXPECT_TRUE((u16_0 | BIT_12) == BIT_12);
    EXPECT_TRUE((u16_0 | BIT_13) == BIT_13);
    EXPECT_TRUE((u16_0 | BIT_14) == BIT_14);
    EXPECT_TRUE((u16_0 | BIT_15) == BIT_15);
    
    EXPECT_TRUE((u16_0 | BIT_0 | BIT_1) == BIT_0 + BIT_1);
    EXPECT_TRUE((u16_0 | BIT_1 | BIT_2) == BIT_1 + BIT_2);
    EXPECT_TRUE((u16_0 | BIT_2 | BIT_3) == BIT_2 + BIT_3);
    EXPECT_TRUE((u16_0 | BIT_3 | BIT_4) == BIT_3 + BIT_4);
    EXPECT_TRUE((u16_0 | BIT_4 | BIT_5) == BIT_4 + BIT_5);
    EXPECT_TRUE((u16_0 | BIT_5 | BIT_6) == BIT_5 + BIT_6);
    EXPECT_TRUE((u16_0 | BIT_6 | BIT_7) == BIT_6 + BIT_7);
    EXPECT_TRUE((u16_0 | BIT_7 | BIT_8) == BIT_7 + BIT_8);
    EXPECT_TRUE((u16_0 | BIT_8 | BIT_9) == BIT_8 + BIT_9);
    EXPECT_TRUE((u16_0 | BIT_9 | BIT_10) == BIT_9 + BIT_10);
    EXPECT_TRUE((u16_0 | BIT_10 | BIT_11) == BIT_10 + BIT_11);
    EXPECT_TRUE((u16_0 | BIT_11 | BIT_12) == BIT_11 + BIT_12);
    EXPECT_TRUE((u16_0 | BIT_12 | BIT_13) == BIT_12 + BIT_13);
    EXPECT_TRUE((u16_0 | BIT_13 | BIT_14) == BIT_13 + BIT_14);
    EXPECT_TRUE((u16_0 | BIT_14 | BIT_15) == BIT_14 + BIT_15);

    EXPECT_TRUE((u16_0 | BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6 | BIT_7 | BIT_8 | BIT_9 | BIT_10 | BIT_11 | BIT_12 | BIT_13 | BIT_14 | BIT_15) == u16_max);

    u32 u32_0 = 0;
    
    EXPECT_TRUE((u32_0 | BIT_0) == BIT_0);
    EXPECT_TRUE((u32_0 | BIT_1) == BIT_1);
    EXPECT_TRUE((u32_0 | BIT_2) == BIT_2);
    EXPECT_TRUE((u32_0 | BIT_3) == BIT_3);
    EXPECT_TRUE((u32_0 | BIT_4) == BIT_4);
    EXPECT_TRUE((u32_0 | BIT_5) == BIT_5);
    EXPECT_TRUE((u32_0 | BIT_6) == BIT_6);
    EXPECT_TRUE((u32_0 | BIT_7) == BIT_7);
    EXPECT_TRUE((u32_0 | BIT_8) == BIT_8);
    EXPECT_TRUE((u32_0 | BIT_9) == BIT_9);
    EXPECT_TRUE((u32_0 | BIT_10) == BIT_10);
    EXPECT_TRUE((u32_0 | BIT_11) == BIT_11);
    EXPECT_TRUE((u32_0 | BIT_12) == BIT_12);
    EXPECT_TRUE((u32_0 | BIT_13) == BIT_13);
    EXPECT_TRUE((u32_0 | BIT_14) == BIT_14);
    EXPECT_TRUE((u32_0 | BIT_15) == BIT_15);
    EXPECT_TRUE((u32_0 | BIT_16) == BIT_16);
    EXPECT_TRUE((u32_0 | BIT_17) == BIT_17);
    EXPECT_TRUE((u32_0 | BIT_18) == BIT_18);
    EXPECT_TRUE((u32_0 | BIT_19) == BIT_19);
    EXPECT_TRUE((u32_0 | BIT_20) == BIT_20);
    EXPECT_TRUE((u32_0 | BIT_21) == BIT_21);
    EXPECT_TRUE((u32_0 | BIT_22) == BIT_22);
    EXPECT_TRUE((u32_0 | BIT_23) == BIT_23);
    EXPECT_TRUE((u32_0 | BIT_24) == BIT_24);
    EXPECT_TRUE((u32_0 | BIT_25) == BIT_25);
    EXPECT_TRUE((u32_0 | BIT_26) == BIT_26);
    EXPECT_TRUE((u32_0 | BIT_27) == BIT_27);
    EXPECT_TRUE((u32_0 | BIT_28) == BIT_28);
    EXPECT_TRUE((u32_0 | BIT_29) == BIT_29);
    EXPECT_TRUE((u32_0 | BIT_30) == BIT_30);
    EXPECT_TRUE((u32_0 | BIT_31) == BIT_31);
    
    EXPECT_TRUE((u32_0 | BIT_0 | BIT_1) == BIT_0 + BIT_1);
    EXPECT_TRUE((u32_0 | BIT_1 | BIT_2) == BIT_1 + BIT_2);
    EXPECT_TRUE((u32_0 | BIT_2 | BIT_3) == BIT_2 + BIT_3);
    EXPECT_TRUE((u32_0 | BIT_3 | BIT_4) == BIT_3 + BIT_4);
    EXPECT_TRUE((u32_0 | BIT_4 | BIT_5) == BIT_4 + BIT_5);
    EXPECT_TRUE((u32_0 | BIT_5 | BIT_6) == BIT_5 + BIT_6);
    EXPECT_TRUE((u32_0 | BIT_6 | BIT_7) == BIT_6 + BIT_7);
    EXPECT_TRUE((u32_0 | BIT_7 | BIT_8) == BIT_7 + BIT_8);
    EXPECT_TRUE((u32_0 | BIT_8 | BIT_9) == BIT_8 + BIT_9);
    EXPECT_TRUE((u32_0 | BIT_9 | BIT_10) == BIT_9 + BIT_10);
    EXPECT_TRUE((u32_0 | BIT_10 | BIT_11) == BIT_10 + BIT_11);
    EXPECT_TRUE((u32_0 | BIT_11 | BIT_12) == BIT_11 + BIT_12);
    EXPECT_TRUE((u32_0 | BIT_12 | BIT_13) == BIT_12 + BIT_13);
    EXPECT_TRUE((u32_0 | BIT_13 | BIT_14) == BIT_13 + BIT_14);
    EXPECT_TRUE((u32_0 | BIT_14 | BIT_15) == BIT_14 + BIT_15);
    EXPECT_TRUE((u32_0 | BIT_15 | BIT_16) == BIT_15 + BIT_16);
    EXPECT_TRUE((u32_0 | BIT_16 | BIT_17) == BIT_16 + BIT_17);
    EXPECT_TRUE((u32_0 | BIT_17 | BIT_18) == BIT_17 + BIT_18);
    EXPECT_TRUE((u32_0 | BIT_18 | BIT_19) == BIT_18 + BIT_19);
    EXPECT_TRUE((u32_0 | BIT_19 | BIT_20) == BIT_19 + BIT_20);
    EXPECT_TRUE((u32_0 | BIT_20 | BIT_21) == BIT_20 + BIT_21);
    EXPECT_TRUE((u32_0 | BIT_21 | BIT_22) == BIT_21 + BIT_22);
    EXPECT_TRUE((u32_0 | BIT_22 | BIT_23) == BIT_22 + BIT_23);
    EXPECT_TRUE((u32_0 | BIT_23 | BIT_24) == BIT_23 + BIT_24);
    EXPECT_TRUE((u32_0 | BIT_24 | BIT_25) == BIT_24 + BIT_25);
    EXPECT_TRUE((u32_0 | BIT_25 | BIT_26) == BIT_25 + BIT_26);
    EXPECT_TRUE((u32_0 | BIT_26 | BIT_27) == BIT_26 + BIT_27);
    EXPECT_TRUE((u32_0 | BIT_27 | BIT_28) == BIT_27 + BIT_28);
    EXPECT_TRUE((u32_0 | BIT_28 | BIT_29) == BIT_28 + BIT_29);
    EXPECT_TRUE((u32_0 | BIT_29 | BIT_30) == BIT_29 + BIT_30);
    EXPECT_TRUE((u32_0 | BIT_30 | BIT_31) == BIT_30 + BIT_31);
    
    EXPECT_TRUE((u32_0 | BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6 | BIT_7 | BIT_8 | BIT_9 | BIT_10 | BIT_11 | BIT_12 | BIT_13 | BIT_14 | BIT_15
        | BIT_16 | BIT_17 | BIT_18 | BIT_19 | BIT_20 | BIT_21 | BIT_22 | BIT_23 | BIT_24 | BIT_25 | BIT_26 | BIT_27 | BIT_28 | BIT_29 | BIT_30 | BIT_31) == u32_max);
}

TEST(BasicsTest, xBits)
{
    u8 x8BIT_TEST_0 = ~BIT_0;
    u8 x8BIT_TEST_1 = ~BIT_1;
    u8 x8BIT_TEST_2 = ~BIT_2;
    u8 x8BIT_TEST_3 = ~BIT_3;
    u8 x8BIT_TEST_4 = ~BIT_4;
    u8 x8BIT_TEST_5 = ~BIT_5;
    u8 x8BIT_TEST_6 = ~BIT_6;
    u8 x8BIT_TEST_7 = ~BIT_7;
    
    EXPECT_EQ(x8BIT_0, x8BIT_TEST_0);
    EXPECT_EQ(x8BIT_1, x8BIT_TEST_1);
    EXPECT_EQ(x8BIT_2, x8BIT_TEST_2);
    EXPECT_EQ(x8BIT_3, x8BIT_TEST_3);
    EXPECT_EQ(x8BIT_4, x8BIT_TEST_4);
    EXPECT_EQ(x8BIT_5, x8BIT_TEST_5);
    EXPECT_EQ(x8BIT_6, x8BIT_TEST_6);
    EXPECT_EQ(x8BIT_7, x8BIT_TEST_7);
    
    u16 x16BIT_TEST_0 = ~BIT_0;
    u16 x16BIT_TEST_1 = ~BIT_1;
    u16 x16BIT_TEST_2 = ~BIT_2;
    u16 x16BIT_TEST_3 = ~BIT_3;
    u16 x16BIT_TEST_4 = ~BIT_4;
    u16 x16BIT_TEST_5 = ~BIT_5;
    u16 x16BIT_TEST_6 = ~BIT_6;
    u16 x16BIT_TEST_7 = ~BIT_7;
    u16 x16BIT_TEST_8 = ~BIT_8;
    u16 x16BIT_TEST_9 = ~BIT_9;
    u16 x16BIT_TEST_10 = ~BIT_10;
    u16 x16BIT_TEST_11 = ~BIT_11;
    u16 x16BIT_TEST_12 = ~BIT_12;
    u16 x16BIT_TEST_13 = ~BIT_13;
    u16 x16BIT_TEST_14 = ~BIT_14;
    u16 x16BIT_TEST_15 = ~BIT_15;
    
    EXPECT_EQ(x16BIT_0, x16BIT_TEST_0);
    EXPECT_EQ(x16BIT_1, x16BIT_TEST_1);
    EXPECT_EQ(x16BIT_2, x16BIT_TEST_2);
    EXPECT_EQ(x16BIT_3, x16BIT_TEST_3);
    EXPECT_EQ(x16BIT_4, x16BIT_TEST_4);
    EXPECT_EQ(x16BIT_5, x16BIT_TEST_5);
    EXPECT_EQ(x16BIT_6, x16BIT_TEST_6);
    EXPECT_EQ(x16BIT_7, x16BIT_TEST_7);
    EXPECT_EQ(x16BIT_8, x16BIT_TEST_8);
    EXPECT_EQ(x16BIT_9, x16BIT_TEST_9);
    EXPECT_EQ(x16BIT_10, x16BIT_TEST_10);
    EXPECT_EQ(x16BIT_11, x16BIT_TEST_11);
    EXPECT_EQ(x16BIT_12, x16BIT_TEST_12);
    EXPECT_EQ(x16BIT_13, x16BIT_TEST_13);
    EXPECT_EQ(x16BIT_14, x16BIT_TEST_14);
    EXPECT_EQ(x16BIT_15, x16BIT_TEST_15);
    
    u32 x32BIT_TEST_0 = ~BIT_0;
    u32 x32BIT_TEST_1 = ~BIT_1;
    u32 x32BIT_TEST_2 = ~BIT_2;
    u32 x32BIT_TEST_3 = ~BIT_3;
    u32 x32BIT_TEST_4 = ~BIT_4;
    u32 x32BIT_TEST_5 = ~BIT_5;
    u32 x32BIT_TEST_6 = ~BIT_6;
    u32 x32BIT_TEST_7 = ~BIT_7;
    u32 x32BIT_TEST_8 = ~BIT_8;
    u32 x32BIT_TEST_9 = ~BIT_9;
    u32 x32BIT_TEST_10 = ~BIT_10;
    u32 x32BIT_TEST_11 = ~BIT_11;
    u32 x32BIT_TEST_12 = ~BIT_12;
    u32 x32BIT_TEST_13 = ~BIT_13;
    u32 x32BIT_TEST_14 = ~BIT_14;
    u32 x32BIT_TEST_15 = ~BIT_15;
    u32 x32BIT_TEST_16 = ~BIT_16;
    u32 x32BIT_TEST_17 = ~BIT_17;
    u32 x32BIT_TEST_18 = ~BIT_18;
    u32 x32BIT_TEST_19 = ~BIT_19;
    u32 x32BIT_TEST_20 = ~BIT_20;
    u32 x32BIT_TEST_21 = ~BIT_21;
    u32 x32BIT_TEST_22 = ~BIT_22;
    u32 x32BIT_TEST_23 = ~BIT_23;
    u32 x32BIT_TEST_24 = ~BIT_24;
    u32 x32BIT_TEST_25 = ~BIT_25;
    u32 x32BIT_TEST_26 = ~BIT_26;
    u32 x32BIT_TEST_27 = ~BIT_27;
    u32 x32BIT_TEST_28 = ~BIT_28;
    u32 x32BIT_TEST_29 = ~BIT_29;
    u32 x32BIT_TEST_30 = ~BIT_30;
    u32 x32BIT_TEST_31 = ~BIT_31;
    
    EXPECT_EQ(x32BIT_0, x32BIT_TEST_0);
    EXPECT_EQ(x32BIT_1, x32BIT_TEST_1);
    EXPECT_EQ(x32BIT_2, x32BIT_TEST_2);
    EXPECT_EQ(x32BIT_3, x32BIT_TEST_3);
    EXPECT_EQ(x32BIT_4, x32BIT_TEST_4);
    EXPECT_EQ(x32BIT_5, x32BIT_TEST_5);
    EXPECT_EQ(x32BIT_6, x32BIT_TEST_6);
    EXPECT_EQ(x32BIT_7, x32BIT_TEST_7);
    EXPECT_EQ(x32BIT_8, x32BIT_TEST_8);
    EXPECT_EQ(x32BIT_9, x32BIT_TEST_9);
    EXPECT_EQ(x32BIT_10, x32BIT_TEST_10);
    EXPECT_EQ(x32BIT_11, x32BIT_TEST_11);
    EXPECT_EQ(x32BIT_12, x32BIT_TEST_12);
    EXPECT_EQ(x32BIT_13, x32BIT_TEST_13);
    EXPECT_EQ(x32BIT_14, x32BIT_TEST_14);
    EXPECT_EQ(x32BIT_15, x32BIT_TEST_15);
    EXPECT_EQ(x32BIT_16, x32BIT_TEST_16);
    EXPECT_EQ(x32BIT_17, x32BIT_TEST_17);
    EXPECT_EQ(x32BIT_18, x32BIT_TEST_18);
    EXPECT_EQ(x32BIT_19, x32BIT_TEST_19);
    EXPECT_EQ(x32BIT_20, x32BIT_TEST_20);
    EXPECT_EQ(x32BIT_21, x32BIT_TEST_21);
    EXPECT_EQ(x32BIT_22, x32BIT_TEST_22);
    EXPECT_EQ(x32BIT_23, x32BIT_TEST_23);
    EXPECT_EQ(x32BIT_24, x32BIT_TEST_24);
    EXPECT_EQ(x32BIT_25, x32BIT_TEST_25);
    EXPECT_EQ(x32BIT_26, x32BIT_TEST_26);
    EXPECT_EQ(x32BIT_27, x32BIT_TEST_27);
    EXPECT_EQ(x32BIT_28, x32BIT_TEST_28);
    EXPECT_EQ(x32BIT_29, x32BIT_TEST_29);
    EXPECT_EQ(x32BIT_30, x32BIT_TEST_30);
    EXPECT_EQ(x32BIT_31, x32BIT_TEST_31);

    EXPECT_EQ(0, u32_max &
        x32BIT_0 & x32BIT_1 & x32BIT_2 & x32BIT_3 & x32BIT_4 & x32BIT_5 & x32BIT_6 & x32BIT_7 &
        x32BIT_8 & x32BIT_9 & x32BIT_10 & x32BIT_11 & x32BIT_12 & x32BIT_13 & x32BIT_14 & x32BIT_15 &
        x32BIT_16 & x32BIT_17 & x32BIT_18 & x32BIT_19 & x32BIT_20 & x32BIT_21 & x32BIT_22 & x32BIT_23 &
        x32BIT_24 & x32BIT_25 & x32BIT_26 & x32BIT_27 & x32BIT_28 & x32BIT_29 & x32BIT_30 & x32BIT_31);
}

TEST(BasicsTest, BitArrays)
{
    EXPECT_EQ(8, sizeof(u8Bits)/sizeof(u8));
    EXPECT_EQ(16, sizeof(u16Bits)/sizeof(u16));
    EXPECT_EQ(32, sizeof(u32Bits)/sizeof(u32));
    
    EXPECT_EQ(BIT_0, u8Bits[0]);
    EXPECT_EQ(BIT_1, u8Bits[1]);
    EXPECT_EQ(BIT_2, u8Bits[2]);
    EXPECT_EQ(BIT_3, u8Bits[3]);
    EXPECT_EQ(BIT_4, u8Bits[4]);
    EXPECT_EQ(BIT_5, u8Bits[5]);
    EXPECT_EQ(BIT_6, u8Bits[6]);
    EXPECT_EQ(BIT_7, u8Bits[7]);
    
    EXPECT_EQ(BIT_0, u16Bits[0]);
    EXPECT_EQ(BIT_1, u16Bits[1]);
    EXPECT_EQ(BIT_2, u16Bits[2]);
    EXPECT_EQ(BIT_3, u16Bits[3]);
    EXPECT_EQ(BIT_4, u16Bits[4]);
    EXPECT_EQ(BIT_5, u16Bits[5]);
    EXPECT_EQ(BIT_6, u16Bits[6]);
    EXPECT_EQ(BIT_7, u16Bits[7]);
    EXPECT_EQ(BIT_8, u16Bits[8]);
    EXPECT_EQ(BIT_9, u16Bits[9]);
    EXPECT_EQ(BIT_10, u16Bits[10]);
    EXPECT_EQ(BIT_11, u16Bits[11]);
    EXPECT_EQ(BIT_12, u16Bits[12]);
    EXPECT_EQ(BIT_13, u16Bits[13]);
    EXPECT_EQ(BIT_14, u16Bits[14]);
    EXPECT_EQ(BIT_15, u16Bits[15]);
    
    EXPECT_EQ(BIT_0, u32Bits[0]);
    EXPECT_EQ(BIT_1, u32Bits[1]);
    EXPECT_EQ(BIT_2, u32Bits[2]);
    EXPECT_EQ(BIT_3, u32Bits[3]);
    EXPECT_EQ(BIT_4, u32Bits[4]);
    EXPECT_EQ(BIT_5, u32Bits[5]);
    EXPECT_EQ(BIT_6, u32Bits[6]);
    EXPECT_EQ(BIT_7, u32Bits[7]);
    EXPECT_EQ(BIT_8, u32Bits[8]);
    EXPECT_EQ(BIT_9, u32Bits[9]);
    EXPECT_EQ(BIT_10, u32Bits[10]);
    EXPECT_EQ(BIT_11, u32Bits[11]);
    EXPECT_EQ(BIT_12, u32Bits[12]);
    EXPECT_EQ(BIT_13, u32Bits[13]);
    EXPECT_EQ(BIT_14, u32Bits[14]);
    EXPECT_EQ(BIT_15, u32Bits[15]);
    EXPECT_EQ(BIT_16, u32Bits[16]);
    EXPECT_EQ(BIT_17, u32Bits[17]);
    EXPECT_EQ(BIT_18, u32Bits[18]);
    EXPECT_EQ(BIT_19, u32Bits[19]);
    EXPECT_EQ(BIT_20, u32Bits[20]);
    EXPECT_EQ(BIT_21, u32Bits[21]);
    EXPECT_EQ(BIT_22, u32Bits[22]);
    EXPECT_EQ(BIT_23, u32Bits[23]);
    EXPECT_EQ(BIT_24, u32Bits[24]);
    EXPECT_EQ(BIT_25, u32Bits[25]);
    EXPECT_EQ(BIT_26, u32Bits[26]);
    EXPECT_EQ(BIT_27, u32Bits[27]);
    EXPECT_EQ(BIT_28, u32Bits[28]);
    EXPECT_EQ(BIT_29, u32Bits[29]);
    EXPECT_EQ(BIT_30, u32Bits[30]);
    EXPECT_EQ(BIT_31, u32Bits[31]);
    
    u8 u8SingleBit = 1;
    for ( size_t bit=0; bit<8; bit++ )
    {
        EXPECT_EQ(u8SingleBit, u8Bits[bit]);
        u8SingleBit = u8SingleBit << 1;
    }

    u16 u16SingleBit = 1;
    for ( size_t bit=0; bit<8; bit++ )
    {
        EXPECT_EQ(u16SingleBit, u8Bits[bit]);
        u16SingleBit = u16SingleBit << 1;
    }

    u16 u32SingleBit = 1;
    for ( size_t bit=0; bit<8; bit++ )
    {
        EXPECT_EQ(u32SingleBit, u8Bits[bit]);
        u32SingleBit = u32SingleBit << 1;
    }
}

TEST(BasicsTest, NotImplemented)
{
    EXPECT_THROW(throw NotImplemented(), NotImplemented);
    EXPECT_THROW(throw NotImplemented("test"), NotImplemented);
    std::string test = "test";
    try {
        throw NotImplemented(test);
    } catch ( NotImplemented &e ) {
        EXPECT_STREQ(test.c_str(), e.what());
    }
}

#pragma warning(pop)
