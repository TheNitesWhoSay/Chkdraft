#include "DefaultCHK.h"

bool Get_TYPE(buffer &c)
{
	return c.addStr("TYPE\4\0\0\0RAWB", 12);
}

bool Get_VER (buffer &c)
{
	return c.addStr("VER \2\0\0\0Í\0", 10);
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
	return c.addStr("VCOD\20\4\0\0004\31Êw™Ühq\12`¿Ã§çu§\37)}¦×°:»Ì1$í\27L\23\13e ¢·‘½\30kÃ]İâzÕ7öYdÔcš", 62)
		&& c.addStr("\22\17C\\.Fãtø*\10j7\0067öÖ;\16”c\26Eg\\ì×{÷·\32üÔsú?Œ.Àá\17Ñt\11\7•ãd×u\26ht™§OÚÕ \30", 61)
		&& c.addStr("\37çæ ¾¦¶ã\37Ê\14ïp1Õ\0321M¸$5ãøÇ}á\32XŞô\5\'Cº¬Û\7Üi¾\12¨ìI×X\26?åÛÁŠAÏÀ\5Ê\34r¢±_¥", 65)
		&& c.addStr("Ä#p›„\4á\24€{ÚúÛi\6£ó\17@¾óÎÔãÉË×Z@\0014òh\24ø8Å\32şÖ=KS\5\5ú4\20Eİ‘iş¯àîğóH~İŸ­Üub", 69)
		&& c.addStr("z¬å1\33bg Í6Mà˜!tû\11yq6gÍw_Ö<¢¢¦Æ\32ãÎjNÍ©l†º;µôvıøDğ¼.én)#%/k\10«\'Dz\22Ì™íÜòuÅ<8~", 75)
		&& c.addStr("÷\34\33ÅÑ-”e\6ÉHİ¾2-¬µÉ2fJØ45?\25ß²îë¶\4öM–5B”œbŠÓaR¨{oÜaüôl\24-ş™ê¤\12èÙş\23ĞHDY€fóã", 72)
		&& c.addStr("4Ù\31\26×cş0\30~:›\17±\22ğõŒ\12xXÛ>c¸Œ:ªó7Š\32.\\1ùïãmã~›½>\23ÆDÀ¹¼:Ú¤­°tøW\'‰Gæ?7", 68)
		&& c.addStr("äByZßCî´\12Iè<Ãˆ\32ˆ\1kvŠÃı£\26zNV§Ëº\2^\34ì°¹Év\36‚±9>ÉWÅ\31$8L]/T¸o]W0¡\12Rm\30q^", 68)
		&& c.addStr("\23\6ÃY\37Ü>bÜÚµë\33‘•ù§‘ÕÚ3SÎkõ\0p\1ØîèÀ\12ñÎcë¶ÓxïÌ¥ª]¼¤–«òÒaÿêš¨jí¢½>ía9Á‚’\0266#±", 72)
		&& c.addStr("° $å\5›§ª\15\22›3ƒ’ Ú%°ìü$Ğ8#ü•òt€så\31—P}DE“DÛ¢­\35iD\24îç,‡ÿ82ñM¼)ÚB\'&şÁÒ+©öBz\16", 72)
		&& c.addStr("Ëè|Ñ\17[ìVi·a1´mù%@4ymúS§\13ú¤‚ÎÃEIa\15E,(I`÷ó}É\36\17Ğ‰Á&RøÓM5\24º_\13\7©J\0÷\"&/>", 69)
		&& c.addStr("gû\37¡œ\21ÆiO]fX4\25låTF¯_cÖŠ\14•ß½\15ä¯¿@@L£öQq)í&ø…(\"Õ¿¾Ïú(ÅQ¸\6c\7ì½)úU~q\32@2f", 71)
		&& c.addStr("èÔŞÔ^ü“z=Õ;Íu.€\12Ot‡\33Ìêš©Û|\26Såï«xÁn¤r‰Z˜,pPû¡ß\37k·ÙD\7€‚Vı¿Àƒ\16IĞ[\36hj\16šÂ\13", 71)
		&& c.addStr("/C á™\14ö²àz\34^,È E<\13éˆ¬¹–Æt®ƒ*»\23úeëO\37¦°ŠŠáé¸¹ÕU\25NEò­›>Â5~_’.r¶[h#nÆE\16é;‡", 72)
		&& c.addStr("ÔôAÀã¨\5D¾ä\17Š\23\32Ä7ôZ@Uïy\35KJy:œv…7Ì‚=\17¶`¦“~½\\ÂÄrÇM\33–\20\23\5hh5À{ÿF…C*\1", 66)
		&& c.addStr("\4\5\6\2\1\5\2\0\3\7\7\5\4\6\3", 15);
}

bool Get_IOWN(buffer &c)
{
	return c.addStr("IOWN\14\0\0\0\6\6\6\6\6\6\6\6\0\0\0\0", 20);
}

bool Get_OWNR(buffer &c)
{
	return c.addStr("OWNR\14\0\0\0\6\6\6\6\6\6\6\6\0\0\0\0", 20);
}

bool Get_ERA (buffer &c, u16 tileset)
{
	return c.addStr("ERA \2\0\0\0", 8)
		&& c.add<u16>(tileset);
}

bool Get_DIM (buffer &c, u16 width, u16 height)
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
	u32 size = width*height*2;

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
	return c.addStr("UPGRÔ\6\0\0", 8)
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
	u32 size = ((u32(width))/2+1)*((u32(height)+1))*8;

	return c.addStr("ISOM", 4)
		&& c.add<u32>(size)
		&& c.add<u16>(16, size/2);
}

bool Get_TILE(buffer &c, u16 width, u16 height)
{
	u32 size = (u32(width))*(u32(height))*2;

	return c.addStr("TILE", 4)
		&& c.add<u32>(size)
		&& c.add<u8>(0, size);
}

bool Get_DD2 (buffer &c)
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

bool Get_STR (buffer &c)
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
		&& c.add<u32>(width*32) // 'Anywhere' x-end
		&& c.add<u32>(height*32) // 'Anywhere' y-end
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

bool Get_WAV (buffer &c)
{
	return c.addStr("WAV \0\10\0\0", 8)
		&& c.add<u8>(0, 2048);
}

bool Get_UNIS(buffer &c)
{
	return c.addStr("UNISĞ\17\0\0", 8)
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
	return c.addStr("TECSØ\0\0\0\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1", 32)
		&& c.add<u8>(0, 192);
}

bool Get_COLR(buffer &c)
{
	return c.addStr("COLR\10\0\0\0\0\1\2\3\4\5\6\7", 16);
}

bool Get_PUPx(buffer &c)
{
	return c.addStr("PUPx\16	\0\0", 8)
		&& c.add<u8>(0, 1464)
		&& c.add<u8>(3, 16)
		&& c.addStr("\1\1\0\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\0\0\1\0\1\0\1\1\1\1", 39)
		&& c.add<u8>(0, 67)
		&& c.add<u8>(1, 732);
}

bool Get_PTEx(buffer &c)
{
	return c.addStr("PTExˆ\6\0\0", 8)
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
	return c.addStr("TECxŒ\1\0\0", 8)
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
