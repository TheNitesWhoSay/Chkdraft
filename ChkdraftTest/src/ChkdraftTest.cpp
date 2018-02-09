#include "pch.h"
#include "../../Chkdraft/src/Chkdraft.h"

TEST(ConstructChkdraft, Basic) {
    Chkdraft chkdraft;
    EXPECT_EQ(chkdraft.CurrDialog(), (HWND)NULL);
    EXPECT_FALSE(chkdraft.EditFocused());
}
