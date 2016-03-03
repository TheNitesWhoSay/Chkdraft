#ifndef DEFAULTCHK_H
#define DEFAULTCHK_H
#include "Buffer.h"

/** 
    This file contains section data for an empty starcraft 1.16.1 hybrid map

    Oct notation is used for all characters below space on the ascii table,
    as well as for all otherwise invisible characters.

    Details about CHK sections can be found at: http://www.staredit.net/starcraft/Scenario.chk
*/

bool Get_TYPE(buffer &c);

bool Get_VER (buffer &c);

bool Get_IVER(buffer &c);

bool Get_IVE2(buffer &c);

bool Get_VCOD(buffer &c);

bool Get_IOWN(buffer &c);

bool Get_OWNR(buffer &c);

bool Get_ERA (buffer &c, u16 tileset);

bool Get_DIM (buffer &c, u16 width, u16 height);

bool Get_SIDE(buffer &c);

bool Get_MTXM(buffer &c, u16 width, u16 height);

bool Get_PUNI(buffer &c);

bool Get_UPGR(buffer &c);

bool Get_PTEC(buffer &c);

bool Get_UNIT(buffer &c);

bool Get_ISOM(buffer &c, u16 width, u16 height);

bool Get_TILE(buffer &c, u16 width, u16 height);

bool Get_DD2 (buffer &c);

bool Get_THG2(buffer &c);

bool Get_MASK(buffer &c, u16 width, u16 height);

bool Get_STR (buffer &c);

bool Get_MRGN(buffer &c, u16 width, u16 height);

bool Get_SPRP(buffer &c);

bool Get_FORC(buffer &c);

bool Get_WAV (buffer &c);

bool Get_UNIS(buffer &c);

bool Get_UPGS(buffer &c);

bool Get_TECS(buffer &c);

bool Get_COLR(buffer &c);

bool Get_PUPx(buffer &c);

bool Get_PTEx(buffer &c);

bool Get_UNIx(buffer &c);

bool Get_UPGx(buffer &c);

bool Get_TECx(buffer &c);

bool Get_TRIG(buffer &c);

bool Get_MBRF(buffer &c);

bool Get_UPRP(buffer &c);

bool Get_UPUS(buffer &c);

bool Get_SWNM(buffer &c);

#endif