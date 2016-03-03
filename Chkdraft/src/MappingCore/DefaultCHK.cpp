#include "DefaultCHK.h"

bool Get_TYPE(buffer &c)
{
    return c.addStr("TYPE\4\0\0\0RAWB", 12);
}

bool Get_VER(buffer &c)
{
    return c.addStr("VER \2\0\0\0Õ\0", 10);
}

bool Get_IVER(buffer &c)
{
    return c.addStr("IVER\2\0\0\0\12\0", 10);
}

bool Get_IVE2(buffer &c)
{
    return c.addStr("IVE2\2\0\0\0\13\0", 10);
}

bool Get_VCOD(buffer &c)
{
    return c.addStr("VCOD\20\4\0\0004\31\312w\231\334hq\12`\277\303\247\347u\247\37)}\246\327\260:\273\314", 33)
        && c.addStr("1$\355\27L\23\13e \242\267\221\275\30k\215\303]\335\342z\3257\366Yd\324c\232\22\17C\\.", 34)
        && c.addStr("F\343t\370*\10j7\0067\366\326;\16\224c\26Eg\\\354\327{\367\267\32\374\324\236s\372?\214", 33)
        && c.addStr(".\300\341\17\321t\11\7\225\343d\327u\26ht\231\247O\332\325 \30\37\347\346\240\276\246\266", 30)
        && c.addStr("\343\37\312\14\357p1\325\0321M\270$5\343\370\307}\341\32X\336\364\5\'C\272\254\333\7\334", 31)
        && c.addStr("i\276\12\250\217\354I\327X\26?\345\333\301\212A\317\300\5\235\312\34r\242\261_\245\304", 28)
        && c.addStr("#p\233\204\4\341\24\200{\220\332\372\333i\6\243\363\17@\276\363\316\324\343\311\313\327", 27)
        && c.addStr("Z@\0014\362h\24\3708\216\305\32\376\326=KS\5\5\3724\20E\216\335\221i\376\257\340\356\360", 32)
        && c.addStr("\363H~\335\237\255\334ubz\254\3451\33bg \3156M\340\230!t\373\11yq6g\315\177w_\326<\242", 37)
        && c.addStr("\242\246\306\32\343\316jN\315\251l\206\272\235;\265\364v\375\370D\360\274.\351n)#%/k\10", 32)
        && c.addStr("\253\'Dz\22\314\231\355\334\362u\305<8~\367\34\33\305\321-\224e\6\311H\335\2762-\254\265", 32)
        && c.addStr("\3112\201fJ\33045?\25\337\262\356\353\266\4\366M\2265B\224\234b\212\323aR\250{o\334a\374", 34)
        && c.addStr("\364l\24-\376\231\352\244\12\350\331\376\23\320HDY\200f\363\3434\331\215\31\26\327c\376", 29)
        && c.addStr("0\30~:\233\215\17\261\22\360\365\214\12xX\333>c\270\214:\252\363\2167\212\32.\\1\371\357", 32)
        && c.addStr("\343m\343~\233\275>\23\306D\300\271\274:\332\220\244\255\260t\370W\'\211G\346?7\344ByZ", 32)
        && c.addStr("\337C\215\356\264\12I\350<\303\210\32\210\1kv\212\303\375\243\26zNV\247\177\313\272\2^", 30)
        && c.addStr("\34\354\260\271\311v\36\202\2619>\311W\305\31$8L]/T\270o]W\2160\241\12Rm\30q^\23\6\303", 37)
        && c.addStr("Y\37\334>b\334\332\265\353\33\221\225\371\247\221\325\3323S\316k\365\0p\1\177\330\356\350", 29)
        && c.addStr("\300\12\361\316c\353\266\323x\357\314\245\252]\274\244\226\253\362\322a\377\352\232\250", 25)
        && c.addStr("j\355\242\275>\355a9\301\202\222\0266#\261\260\240$\345\5\233\247\252\15\22\2333\203\222", 29)
        && c.addStr(" \332%\260\354\374$\3208#\374\225\362t\200s\345\31\227P}DE\223D\333\242\255\35iD\24\356", 33)
        && c.addStr("\347,\177\207\3778\2362\361M\274)\332B\'&\376\301\322+\251\366Bz\16\313\350|\321\17[\354", 32)
        && c.addStr("Vi\267a1\264m\371%@4ym\372S\247\13\372\244\202\316\303EIa\15E,\217(I`\367\363}\311\36\17", 38)
        && c.addStr("\320\211\301&R\370\323M\2175\24\272\235_\13\7\251J\0\367\"&/>g\373\37\241\234\21\306iO", 33)
        && c.addStr("]fX4\25\220l\345TF\257_c\326\212\14\225\337\275\15\344\257\277@@L\243\366Qq)\355&\370\205", 35)
        && c.addStr("(\"\325\277\276\317\372(\305\177Q\270\6c\7\354\275\217)\372U~q\32@2f\350\324\336\235\324", 32)
        && c.addStr("^\374\223z=\325;\315u.\200\12Ot\207\33\314\217\352\232\251\333|\26S\345\357\253x\301n\244", 32)
        && c.addStr("r\211Z\230,pP\373\241\337\37k\267\331D\7\200\202V\375\277\300\203\16I\320[\36hj\16\232", 32)
        && c.addStr("\302\13/\216C\240\341\231\14\366\262\340z\34^,\310\240E<\13\351\210\254\271\226\306t\256", 29)
        && c.addStr("\203*\273\23\372e\353O\37\246\260\212\212\341\201\351\270\271\325U\25NE\362\255\233>\302", 28)
        && c.addStr("5~_\222.r\266[h#n\306E\16\351;\207\324\364A\300\343\250\5D\276\344\17\212\23\32\3047\364", 34)
        && c.addStr("Z@U\357\235y\35KJy:\234v\2057\314\202=\17\266`\246\223~\275\\\302\304r\307\177\220M\33", 34)
        && c.addStr("\226\20\23\5hh5\300{\377F\205C*\1\4\5\6\2\1\5\2\0\3\7\7\5\4\6\3", 30);
}

bool Get_IOWN(buffer &c)
{
    return c.addStr("IOWN\14\0\0\0\6\6\6\6\6\6\6\6\0\0\0\0", 20);
}

bool Get_OWNR(buffer &c)
{
    return c.addStr("OWNR\14\0\0\0\6\6\6\6\6\6\6\6\0\0\0\0", 20);
}

bool Get_ERA(buffer &c, u16 tileset)
{
    return c.addStr("ERA \2\0\0\0", 8)
        && c.add<u16>(tileset);
}

bool Get_DIM(buffer &c, u16 width, u16 height)
{
    return c.addStr("DIM \4\0\0\0", 8)
        && c.add<u16>(width)
        && c.add<u16>(height);
}

bool Get_SIDE(buffer &c)
{
    return c.addStr("SIDE\14\0\0\0\1\0\2\1\0\2\1\0\7\7\7\4", 20);
}

bool Get_MTXM(buffer &c, u16 width, u16 height)
{
    u32 size = width*height * 2;

    return c.addStr("MTXM", 4)
        && c.add<u32>(size)
        && c.add<u8>(0, size);
}

bool Get_PUNI(buffer &c)
{
    return c.addStr("PUNID\26\0\0", 8)
        && c.add<u8>(1, 5700);
}

bool Get_UPGR(buffer &c)
{
    return c.addStr("UPGR‘\6\0\0", 8)
        && c.add<u8>(0, 1104)
        && c.addStr("\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\1\1\0\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1", 45)
        && c.add<u8>(0, 47)
        && c.add<u8>(1, 552);
}

bool Get_PTEC(buffer &c)
{
    return c.addStr("PTEC\220\3\0\0", 8)
        && c.add<u8>(0, 576)
        && c.add<u8>(1, 24)
        && c.addStr("\0\0\0\0\1\0\1\0\0\0\0\0\1\0\1\0\0\0\1\0\0\0\0", 23)
        && c.add<u8>(1, 289);
}

bool Get_UNIT(buffer &c)
{
    return c.addStr("UNIT\0\0\0\0", 8);
}

bool Get_ISOM(buffer &c, u16 width, u16 height)
{
    u32 size = ((u32(width)) / 2 + 1)*((u32(height) + 1)) * 8;

    return c.addStr("ISOM", 4)
        && c.add<u32>(size)
        && c.add<u16>(16, size / 2);
}

bool Get_TILE(buffer &c, u16 width, u16 height)
{
    u32 size = (u32(width))*(u32(height)) * 2;

    return c.addStr("TILE", 4)
        && c.add<u32>(size)
        && c.add<u8>(0, size);
}

bool Get_DD2(buffer &c)
{
    return c.addStr("DD2 \0\0\0\0", 8);
}

bool Get_THG2(buffer &c)
{
    return c.addStr("THG2\0\0\0\0", 8);
}

bool Get_MASK(buffer &c, u16 width, u16 height)
{
    u32 size = (u32(width))*(u32(height));

    return c.addStr("MASK", 4)
        && c.add<u32>(size)
        && c.add<u8>(255, size);
}

bool Get_STR(buffer &c)
{
    return c.addStr("STR [\10\0\0\0\4\3\10\25\10\62\10;\10C\10K\10S\10", 24)
        && c.add<u16>(2050, 1017)
        && c.addStr("\0Untitled Scenario\0Destroy all enemy buildings.\0", 48)
        && c.addStr("Anywhere\0Force 1\0Force 2\0Force 3\0Force 4\0", 41);
}

bool Get_MRGN(buffer &c, u16 width, u16 height)
{
    return c.addStr("MRGN\0\5\0\0", 8)
        && c.add<u8>(0, 1268)
        && c.add<u32>(width * 32) // 'Anywhere' x-end
        && c.add<u32>(height * 32) // 'Anywhere' y-end
        && c.addStr("\3\0\0\0", 4); // 'Anywhere' string number (3) and elevation (all)
}

bool Get_SPRP(buffer &c)
{
    return c.addStr("SPRP\4\0\0\0\1\0\2\0", 12);
}

bool Get_FORC(buffer &c)
{
    return c.addStr("FORC\24\0\0\0\0\0\0\0\0\0\0\0\4\0\5\0\6\0\7\0\17\17\17\17", 28);
}

bool Get_WAV(buffer &c)
{
    return c.addStr("WAV \0\10\0\0", 8)
        && c.add<u8>(0, 2048);
}

bool Get_UNIS(buffer &c)
{
    return c.addStr("UNIS–\17\0\0", 8)
        && c.add<u8>(1, 228)
        && c.add<u8>(0, 3820);
}

bool Get_UPGS(buffer &c) // 606
{
    return c.addStr("UPGSV\2\0\0", 8)
        && c.add<u8>(1, 46)
        && c.add<u8>(0, 552);
}

bool Get_TECS(buffer &c)
{
    return c.addStr("TECSÿ\0\0\0\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1", 32)
        && c.add<u8>(0, 192);
}

bool Get_COLR(buffer &c)
{
    return c.addStr("COLR\10\0\0\0\0\1\2\3\4\5\6\7", 16);
}

bool Get_PUPx(buffer &c)
{
    return c.addStr("PUPx\16\t\0\0", 8)
        && c.add<u8>(0, 1464)
        && c.add<u8>(3, 16)
        && c.addStr("\1\1\0\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\0\0\1\0\1\0\1\1\1\1", 39)
        && c.add<u8>(0, 67)
        && c.add<u8>(1, 732);
}

bool Get_PTEx(buffer &c)
{
    return c.addStr("PTExà\6\0\0", 8)
        && c.add<u8>(0, 1056)
        && c.add<u8>(1, 44)
        && c.addStr("\0\0\0\0\1\0\1\0\0\0\0\0\1\0\1\0\0\0\1\0\0\0\0\1\0\0\0\0\1\1\0\0\0\1\1\0\0\0\0\0\0\0\0\0", 44)
        && c.add<u8>(1, 528);
}

bool Get_UNIx(buffer &c)
{
    return c.addStr("UNIxH\20\0\0", 8)
        && c.add<u8>(1, 228)
        && c.add<u8>(0, 3940);
}

bool Get_UPGx(buffer &c) // 794
{
    return c.addStr("UPGx\32\3\0\0", 8)
        && c.add<u8>(1, 61)
        && c.add<u8>(0, 733);
}

bool Get_TECx(buffer &c)
{
    return c.addStr("TECxå\1\0\0", 8)
        && c.add<u8>(1, 44)
        && c.add<u8>(0, 352);
}

bool Get_TRIG(buffer &c)
{
    return c.addStr("TRIG\0\0\0\0", 8);
}

bool Get_MBRF(buffer &c)
{
    return c.addStr("MBRF\0\0\0\0", 8);
}

bool Get_UPRP(buffer &c)
{
    return c.addStr("UPRP\0\5\0\0", 8)
        && c.add<u8>(0, 1280);
}

bool Get_UPUS(buffer &c)
{
    return c.addStr("UPUS@\0\0\0", 8)
        && c.add<u8>(0, 64);
}

bool Get_SWNM(buffer &c)
{
    return c.addStr("SWNM\0\4\0\0", 8)
        && c.add<u8>(0, 1024);
}
