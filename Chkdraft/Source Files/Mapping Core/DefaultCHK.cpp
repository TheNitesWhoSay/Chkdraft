#include "DefaultCHK.h"

bool Get_TYPE(buffer &c)
{
	return c.addStr("TYPE\4\0\0\0RAWB", 12);
}

bool Get_VER (buffer &c)
{
	return c.addStr("VER \2\0\0\0�\0", 10);
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
	return c.addStr("VCOD\20\4\0\0004\31�w��hq\12`�ç�u�\37)}�װ:��1$�\27L\23\13e ����\30k��]��z�7�Yd�c�", 62)
		&& c.addStr("\22\17C\\.F�t�*\10j7\0067��;\16�c\26Eg\\��{��\32�Ԟs�?�.��\17�t\11\7��d�u\26ht��O�� \30", 61)
		&& c.addStr("\37�栾���\37�\14�p1�\0321M�$5���}�\32X��\5\'C���\7�i�\12���I�X\26?����A��\5��\34r��_�", 65)
		&& c.addStr("�#p��\4�\24�{����i\6��\17@��������Z@\0014�h\24�8��\32��=KS\5\5�4\20E�ݑi������H~ݟ��ub", 69)
		&& c.addStr("z��1\33bg �6M��!t�\11yq6g�w_�<����\32��jNͩl���;��v��D�.�n)#%/k\10�\'Dz\22̙���u�<8~", 75)
		&& c.addStr("�\34\33��-�e\6�Hݾ2-���2�fJ�45?\25߲��\4�M�5B��b��aR�{o�a��l\24-���\12���\23�HDY�f��", 72)
		&& c.addStr("4ٍ\31\26�c�0\30~:��\17�\22���\12xX�>c��:��7�\32.\\1���m�~��>\23�D���:ڐ���t�W\'�G�?7", 68)
		&& c.addStr("�ByZ�C��\12I�<È\32�\1kv����\26zNV�˺\2^\34찹�v\36��9>�W�\31$8L]/T�o]W�0�\12Rm\30q^", 68)
		&& c.addStr("\23\6�Y\37�>b�ڵ�\33�������3S�k�\0p\1����\12��c��x�̥�]������a�ꚨj�>�a9���\0266#�", 72)
		&& c.addStr("��$�\5���\15\22�3�� �%���$�8#���t�s�\31�P}DE�Dۢ�\35iD\24��,��8�2�M�)�B\'&���+��Bz\16", 72)
		&& c.addStr("��|�\17[�Vi�a1�m�%@4ym�S�\13�����EIa\15E,�(I`��}�\36\17Љ�&R��M�5\24��_\13\7�J\0�\"&/>", 69)
		&& c.addStr("g�\37��\21�iO]fX4\25�l�TF�_c֊\14�߽\15䯿@@L��Qq)�&��(\"տ���(�Q�\6c\7콏)�U~q\32@2f", 71)
		&& c.addStr("��ޝ�^��z=�;�u.�\12Ot�\33̏ꚩ�|\26S��x�n�r�Z�,pP���\37k��D\7��V����\16I�[\36hj\16��\13", 71)
		&& c.addStr("/�C��\14���z\34^,ȠE<\13鈬���t��*�\23�e�O\37�����鸹�U\25NE�>�5~_�.r�[h#n�E\16�;�", 72)
		&& c.addStr("��A��\5D��\17�\23\32�7�Z@U�y\35KJy:�v�7̂=\17�`��~�\\��r��M\33�\20\23\5hh5�{�F�C*\1", 66)
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
	return c.addStr("UPGR�\6\0\0", 8)
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
	return c.addStr("UNIS�\17\0\0", 8)
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
	return c.addStr("TECS�\0\0\0\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1", 32)
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
	return c.addStr("PTEx�\6\0\0", 8)
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
	return c.addStr("TECx�\1\0\0", 8)
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
