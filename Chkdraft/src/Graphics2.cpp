// Extended graphical functions -- primarily focused on iscript and animation

#include "Graphics.h"
#include "CommonFiles/CommonFiles.h"
#include "Chkdraft.h"

// iscript get opcode parameter macro, to make it marginally easier to read
#define is_get(t,o) chkd.scData.iScripts.iscriptBin.get<t>(o)

// Some (x,y) lookup table
const POINT arr_512D28[256] = {
    { 0,-256 },{ 6,-256 },{ 13,-256 },{ 19,-255 },{ 25,-255 },
    { 31,-254 },{ 38,-253 },{ 44,-252 },{ 50,-251 },{ 56,-250 },
    { 62,-248 },{ 68,-247 },{ 74,-245 },{ 80,-243 },{ 86,-241 },
    { 92,-239 },{ 98,-237 },{ 104,-234 },{ 109,-231 },{ 115,-229 },
    { 121,-226 },{ 126,-223 },{ 132,-220 },{ 137,-216 },{ 142,-213 },
    { 147,-209 },{ 152,-206 },{ 157,-202 },{ 162,-198 },{ 167,-194 },
    { 172,-190 },{ 177,-185 },{ 181,-181 },{ 185,-177 },{ 190,-172 },
    { 194,-167 },{ 198,-162 },{ 202,-157 },{ 206,-152 },{ 209,-147 },
    { 213,-142 },{ 216,-137 },{ 220,-132 },{ 223,-126 },{ 226,-121 },
    { 229,-115 },{ 231,-109 },{ 234,-104 },{ 237, -98 },{ 239, -92 },
    { 241, -86 },{ 243, -80 },{ 245, -74 },{ 247, -68 },{ 248, -62 },
    { 250, -56 },{ 251, -50 },{ 252, -44 },{ 253, -38 },{ 254, -31 },
    { 255, -25 },{ 255, -19 },{ 256, -13 },{ 256,  -6 },{ 256,   0 },
    { 256,   6 },{ 256,  13 },{ 255,  19 },{ 255,  25 },{ 254,  31 },
    { 253,  38 },{ 252,  44 },{ 251,  50 },{ 250,  56 },{ 248,  62 },
    { 247,  68 },{ 245,  74 },{ 243,  80 },{ 241,  86 },{ 239,  92 },
    { 237,  98 },{ 234, 104 },{ 231, 109 },{ 229, 115 },{ 226, 121 },
    { 223, 126 },{ 220, 132 },{ 216, 137 },{ 213, 142 },{ 209, 147 },
    { 206, 152 },{ 202, 157 },{ 198, 162 },{ 194, 167 },{ 190, 172 },
    { 185, 177 },{ 181, 181 },{ 177, 185 },{ 172, 190 },{ 167, 194 },
    { 162, 198 },{ 157, 202 },{ 152, 206 },{ 147, 209 },{ 142, 213 },
    { 137, 216 },{ 132, 220 },{ 126, 223 },{ 121, 226 },{ 115, 229 },
    { 109, 231 },{ 104, 234 },{ 98, 237 },{ 92, 239 },{ 86, 241 },
    { 80, 243 },{ 74, 245 },{ 68, 247 },{ 62, 248 },{ 56, 250 },
    { 50, 251 },{ 44, 252 },{ 38, 253 },{ 31, 254 },{ 25, 255 },
    { 19, 255 },{ 13, 256 },{ 6, 256 },{ 0, 256 },{ -6, 256 },
    { -13, 256 },{ -19, 255 },{ -25, 255 },{ -31, 254 },{ -38, 253 },
    { -44, 252 },{ -50, 251 },{ -56, 250 },{ -62, 248 },{ -68, 247 },
    { -74, 245 },{ -80, 243 },{ -86, 241 },{ -92, 239 },{ -98, 237 },
    { -104, 234 },{ -109, 231 },{ -115, 229 },{ -121, 226 },{ -126, 223 },
    { -132, 220 },{ -137, 216 },{ -142, 213 },{ -147, 209 },{ -152, 206 },
    { -157, 202 },{ -162, 198 },{ -167, 194 },{ -172, 190 },{ -177, 185 },
    { -181, 181 },{ -185, 177 },{ -190, 172 },{ -194, 167 },{ -198, 162 },
    { -202, 157 },{ -206, 152 },{ -209, 147 },{ -213, 142 },{ -216, 137 },
    { -220, 132 },{ -223, 126 },{ -226, 121 },{ -229, 115 },{ -231, 109 },
    { -234, 104 },{ -237,  98 },{ -239,  92 },{ -241,  86 },{ -243,  80 },
    { -245,  74 },{ -247,  68 },{ -248,  62 },{ -250,  56 },{ -251,  50 },
    { -252,  44 },{ -253,  38 },{ -254,  31 },{ -255,  25 },{ -255,  19 },
    { -256,  13 },{ -256,   6 },{ -256,   0 },{ -256,  -6 },{ -256, -13 },
    { -255, -19 },{ -255, -25 },{ -254, -31 },{ -253, -38 },{ -252, -44 },
    { -251, -50 },{ -250, -56 },{ -248, -62 },{ -247, -68 },{ -245, -74 },
    { -243, -80 },{ -241, -86 },{ -239, -92 },{ -237, -98 },{ -234,-104 },
    { -231,-109 },{ -229,-115 },{ -226,-121 },{ -223,-126 },{ -220,-132 },
    { -216,-137 },{ -213,-142 },{ -209,-147 },{ -206,-152 },{ -202,-157 },
    { -198,-162 },{ -194,-167 },{ -190,-172 },{ -185,-177 },{ -181,-181 },
    { -177,-185 },{ -172,-190 },{ -167,-194 },{ -162,-198 },{ -157,-202 },
    { -152,-206 },{ -147,-209 },{ -142,-213 },{ -137,-216 },{ -132,-220 },
    { -126,-223 },{ -121,-226 },{ -115,-229 },{ -109,-231 },{ -104,-234 },
    { -98,-237 },{ -92,-239 },{ -86,-241 },{ -80,-243 },{ -74,-245 },
    { -68,-247 },{ -62,-248 },{ -56,-250 },{ -50,-251 },{ -44,-252 },
    { -38,-253 },{ -31,-254 },{ -25,-255 },{ -19,-255 },{ -13,-256 },
    { -6,-256 }
};

// Some distance lookup table
const int arr_513528[64] = { 7,    13,   19,   26,   32,   38,   45,   51,   58,
                            65,   71,   78,   85,   92,   99,   107,  114,  122,
                            129,  137,  146,  154,  163,  172,  181,  190,  200,
                            211,  221,  233,  244,  256,  269,  283,  297,  312,
                            329,  346,  364,  384,  405,  428,  452,  479,  509,
                            542,  578,  619,  664,  716,  775,  844,  926,  1023,
                            1141, 1287, 1476, 1726, 2076, 2600, 3471, 5211, 10429,
                            -1 };

// I don't know where this goes, or really how to do this at all. xD
const struct {
    int id; // Reference only, I guess
    UpdateFunction func;
} imgUpdateFxns[18] = { { DRAW_NORMAL,          &Graphics::updateNull },
                        { DRAW_UNKNOWN1,        &Graphics::updateNull },
                        { DRAW_ENEMY_CLOAK,     &Graphics::updateCloaking },
                        { DRAW_OWN_CLOAK,       &Graphics::updateNull },
                        { DRAW_ALLY_CLOAK,      &Graphics::updateDecloaking },
                        { DRAW_CLOAK,           &Graphics::updateCloaking },
                        { DRAW_CLOAKED,         &Graphics::updateNull },
                        { DRAW_DECLOAK,         &Graphics::updateDecloaking },
                        { DRAW_EMP,             &Graphics::updateNull },
                        { DRAW_EFFECT,          &Graphics::updateNull },
                        { DRAW_SHADOW,          &Graphics::updateNull },
                        { DRAW_HPFLOATDRAW,     &Graphics::updateNull },
                        { DRAW_WARP_IN,         &Graphics::updateNull },
                        { DRAW_SELECTION,       &Graphics::updateNull },
                        { DRAW_PLAYER_SIDE,     &Graphics::updateNull },
                        { DRAW_SIZE_RECT,       &Graphics::updateNull },
                        { DRAW_HALLUCINATION,   &Graphics::updateNull },
                        { DRAW_WARP_FLASH,      &Graphics::updateWarpFlash } };
const struct {
    int id; // Reference only, I guess
    RenderFunction norm;
    RenderFunction flip;
} imgRenderFxns[18] = {
    // Not all of these are defined, so any that are missing are set to Fxn0_0 (Normal draw, not flipped).
    // The correct functions are listed in the comment
    { DRAW_NORMAL,          &Graphics::imageRenderFxn0_0, &Graphics::imageRenderFxn0_0 }, // &Graphics::imageRenderFxn0_0, &Graphics::imageRenderFxn0_1 },
    { DRAW_UNKNOWN1,        &Graphics::imageRenderFxn0_0, &Graphics::imageRenderFxn0_0 }, // &Graphics::imageRenderFxn0_0, &Graphics::imageRenderFxn0_1 },
    { DRAW_ENEMY_CLOAK,     &Graphics::imageRenderFxn2_0, &Graphics::imageRenderFxn2_0 }, // &Graphics::imageRenderFxn2_0, &Graphics::imageRenderFxn2_1 },
    { DRAW_OWN_CLOAK,       &Graphics::imageRenderFxn3_0, &Graphics::imageRenderFxn3_0 }, // &Graphics::imageRenderFxn3_0, &Graphics::imageRenderFxn3_1 },
    { DRAW_ALLY_CLOAK,      &Graphics::imageRenderFxn2_0, &Graphics::imageRenderFxn2_0 }, // &Graphics::imageRenderFxn2_0, &Graphics::imageRenderFxn2_1 },
    { DRAW_CLOAK,           &Graphics::imageRenderFxn5_0, &Graphics::imageRenderFxn5_0 }, // &Graphics::imageRenderFxn5_0, &Graphics::imageRenderFxn5_1 },
    { DRAW_CLOAKED,         &Graphics::imageRenderFxn6_0, &Graphics::imageRenderFxn6_0 }, // &Graphics::imageRenderFxn6_0, &Graphics::imageRenderFxn6_1 },
    { DRAW_DECLOAK,         &Graphics::imageRenderFxn5_0, &Graphics::imageRenderFxn5_0 }, // &Graphics::imageRenderFxn5_0, &Graphics::imageRenderFxn5_1 },
    { DRAW_EMP,             &Graphics::imageRenderFxn8_0, &Graphics::imageRenderFxn8_0 }, // &Graphics::imageRenderFxn8_0, &Graphics::imageRenderFxn8_1 },
    { DRAW_EFFECT,          &Graphics::imageRenderFxn9_0, &Graphics::imageRenderFxn9_0 }, // &Graphics::imageRenderFxn9_0, &Graphics::imageRenderFxn9_1 },
    { DRAW_SHADOW,          &Graphics::imageRenderFxn10_0, &Graphics::imageRenderFxn10_0 }, // &Graphics::imageRenderFxn10_0, &Graphics::imageRenderFxn10_1 },
    { DRAW_HPFLOATDRAW,     &Graphics::imageRenderFxn0_0, &Graphics::imageRenderFxn0_0 }, // &Graphics::imageRenderFxn11_0, &Graphics::imageRenderFxn11_0 }, // No flip function
    { DRAW_WARP_IN,         &Graphics::imageRenderFxn0_0, &Graphics::imageRenderFxn0_0 }, // &Graphics::imageRenderFxn12_0, &Graphics::imageRenderFxn12_1 },
    { DRAW_SELECTION,       &Graphics::imageRenderFxn13_0, &Graphics::imageRenderFxn13_0 }, // No flip function
    { DRAW_PLAYER_SIDE,     &Graphics::imageRenderFxn14_0, &Graphics::imageRenderFxn14_0 }, // &Graphics::imageRenderFxn14_0, &Graphics::imageRenderFxn14_1 },
    { DRAW_SIZE_RECT,       &Graphics::imageRenderFxn0_0, &Graphics::imageRenderFxn0_0 }, // &Graphics::imageRenderFxn15_0, &Graphics::imageRenderFxn15_0 }, // No flip function
    { DRAW_HALLUCINATION,   &Graphics::imageRenderFxn16_0, &Graphics::imageRenderFxn16_0 }, // &Graphics::imageRenderFxn16_0, &Graphics::imageRenderFxn16_1 },
    { DRAW_WARP_FLASH,      &Graphics::imageRenderFxn0_0, &Graphics::imageRenderFxn0_0 } // &Graphics::imageRenderFxn17_0, &Graphics::imageRenderFxn17_1 } };
};

bool Graphics::doAnimation()
{
    bool redraw = false;
    if (GetTickCount() >= gticks) // Time to update for next tick?
    {
        gticks = GetTickCount() + FASTEST;
        for (int i = 0; i < THG2Graphics.size(); i++)
        {
            if (THG2Graphics[i].isUnit)
            {
                activeIscriptUnit = THG2Graphics[i].unit;
                unk_unit_6D11F4 = activeIscriptUnit;
                spriteToIscriptLoop(activeIscriptUnit->sprite);
                if (THG2Graphics[i].unit != NULL) // I dunno how subunits are supposed to be handled
                {

                    activeIscriptUnit = THG2Graphics[i].unit;
                    unk_unit_6D11F4 = activeIscriptUnit;
                    spriteToIscriptLoop(activeIscriptUnit->sprite);
                }
            }
            else
            {
                activeIscriptUnit = nullptr;
                unk_unit_6D11F4 = activeIscriptUnit;
                spriteToIscriptLoop(THG2Graphics[i].sprite);
            }
        }

        for (int i = 0; i < UNITGraphics.size(); i++)
        {
            activeIscriptUnit = UNITGraphics[i];
            unk_unit_6D11F4 = activeIscriptUnit;
            spriteToIscriptLoop(activeIscriptUnit->sprite);
            if (UNITGraphics[i]->subUnit != NULL) // I dunno how subunits are supposed to be handled
            {

                activeIscriptUnit = UNITGraphics[i]->subUnit;
                unk_unit_6D11F4 = activeIscriptUnit;
                spriteToIscriptLoop(activeIscriptUnit->sprite);
            }
        }
        activeIscriptUnit = nullptr;
        unk_unit_6D11F4 = nullptr;
        redraw = true;
    }
    return redraw;
}


UnitNode* Graphics::CreateUnitXY(int player, u16 id, int x, int y) {
    UnitNode* tmp = CreateUnit(id, x, y, player);

    if (tmp == NULL)
    {
        //displayLastNetErrForPlayer(player);
        return NULL;
    }
    
    if (spreadsCreep(tmp->unitType, true) || (chkd.scData.UnitDat(tmp->unitType)->SpecialAbilityFlags & USTATUS_ISABUILDING))
    {
        ApplyCreepAtLocationFromUnitType(tmp->unitType, tmp->sprite->position.x, tmp->sprite->position.y);
    }
    updateUnitStatsFinishBuilding(tmp);
    if (unknown_0049EC30(tmp))
    {
        updateUnitStrengthAndApplyDefaultOrders(tmp);
        unknown_00462960(tmp);
        return tmp;
    }
    //if (*((u16*)*p6D1220) <= 0x11)
    //{
    //  ESI = 0x00501B7D;
    //}
    //else
    //{
    //  ESI = ((u16)(*p6D1220) + 0x24) + (*p6D1220);
    //}
    //sprintf_s(&[EBP - 100], 0x100, ESI, getUnitNameStatString(tmp->unitType), tmp->sprite->position.x, tmp->sprite->position.y);
    //UnitPlacementError(EDI, 0x3);
    //tmp->userActionFlags |= 4;

    //orders_SelfDestructing(tmp);

    return NULL;
}

UnitNode* Graphics::CreateUnit(int unit, int x, int y, int player)
{
    int EBP4 = getRand();
    UnitNode* tmp = UnitConstructor(unit, x, y, player, EBP4);
    if (tmp == NULL)
    {
        return NULL;
    }
    // Add to used list
    /*if (tmp->statusFlags & 2) // status flag 2 -- building ?
    {
        CListPushBackHiddenUnitEntry(tmp); // Dunno lol
    }
    else
    {
        CListPushBackUsedUnitEntry(tmp); // Dunno lol
    }*/

    // Add to list !
    if (unitLastCreated != NULL)
    {
        unitLastCreated->next = tmp;
    }
    tmp->prev = unitLastCreated;
    unitLastCreated = tmp;

    if (unit > 0x69 || chkd.scData.UnitDat(unit)->Subunit1 == 0xE4)
    {
        tmp->subUnit = NULL;
        return tmp;
    }
    UnitNode* subunit = UnitConstructor(chkd.scData.UnitDat(unit)->Subunit1, x, y, player, EBP4);
    if (subunit == NULL)
    {
        UnitDestructor(tmp);
        return NULL;
    }
    tmp->subUnit = subunit;
    subunit->subUnit = tmp;
    point pos;
    getImageAttackFrame(&pos, tmp->sprite, 0, IMAGES_SPECIAL_OVERLAY);
    setSpriteMainImgOffset(subunit->sprite, pos.x, pos.y);
    return tmp;
}

UnitNode* Graphics::UnitConstructor(int unitid, int x, int y, int player, int randseed) {
    UnitNode* newUnit = unitEmptyNode;
    if (newUnit == NULL) // No empty nodes?
    {
        // Create one!
        unitNodes.push_back(std::shared_ptr<UnitNode>(new UnitNode()));
        newUnit = unitNodes.back().get(); // Get new node
    }

    if (newUnit == NULL || !unknown_004A0320(newUnit, unitid, x, y, player))
    {
        //unk_59CCA0 = 0x3E; // Error code or something
        return NULL;
    }

    // Remove from empty list -- CListRemoveEmptyUnitEntry( newUnit )
    unitEmptyNode = newUnit->next;
    newUnit->next = NULL;
    newUnit->prev = NULL;

    return newUnit;
}

// Returns true if unitType spreads creep. Hatchery and Creep Colony return parameter
bool Graphics::spreadsCreep(u16 unitType, bool unk) {
    switch (unitType)
    {
        case 131: // Hatchery
        case 143: // Creep Colony
            return unk;
        case 132: // Lair
        case 133: // Hive
        case 144: // Spore Colony
        case 146: // Sunken Colony
            return true;
        default:
            return false;
    }
}

void Graphics::ApplyCreepAtLocationFromUnitType(int unitid, int x, int y)
{
    // ??? Get
}

void Graphics::updateUnitStatsFinishBuilding(UnitNode* unit) {
    if (unit->statusFlags & 2) // GoundedBuilding
    {
        SetConstructionGraphic(unit, 0);
        for (ImageNode* img = unit->sprite->pImageHead; img != NULL; img = img->next) {
            playImageIscript(img, ANIM_BUILT);
        }
        //if ((*p6D0F14) != NULL) return;
        //if (unit->playerID != g_LocalNationID) return;
        // I dunno what these are
        //(*p68C1E8) = NULL;
        //(*p68C1EC) = NULL;
        //(*p68C1B0) = 1;
        //((u8)*p68AC74) = 1;
        //((u8)*p68C1F8) = 1;
    }
    else
    {
        if (unit->statusFlags & 0x10000) // IsAUnit ?
        {
            int direction = chkd.scData.UnitDat(unit->unitType)->UnitDirection;
            if (direction == 0x20) {
                direction = getRand() & 0x1F;
            }
            turnUnit(unit, direction << 3);
        }
        if (unit->unitType >= 0xCB && unit->unitType <= 0xD5) {
            initUnitTrapDoodad(unit);
        }
    }
    return;
}

bool Graphics::unknown_0049EC30(UnitNode* unit)
{
    if(unit->sprite->flags & 0x20)
    {
        points pos = {unit->sprite->position.x, unit->sprite->position.y};
        int EBPC; // ?
        //EDX = unit->position.y
        //ECX = &[EBP-4]
        //EAX = 0
        //if(!canMoveUnit(0, unit, &pos, &EBPC, 0, 0)) // (EAX , ESI, push, push, push, push)
        //{
        //  displayLastNetErrForPlayer(unit->playerID); // ?
        //  return false;
        //}
        //EAX = [EBP-C]
        //EBX = [EBP-A]
        //ECX = [EBP-A]
        //EDX = unit
        //MoveUnit( ? )
        //EDI = unit->subUnit
        
        if(unit->subUnit != NULL && (chkd.scData.UnitDat(unit->unitType)->SpecialAbilityFlags & 0x10) == 0) // Is Subunit
        {
            //incrementUnitScoresEx(1, 0);
            unit->subUnit->statusFlags |= 1;
            unit->subUnit->hitPoints = chkd.scData.UnitDat(unit->subUnit->unitType)->HitPoints;
            //EAX = [EBP-C]
            //EDX = unit->subUnit
            //ECX = 0
            //MoveUnit( ? )
        }
    }
    return true;
}

void Graphics::updateUnitStrengthAndApplyDefaultOrders(UnitNode* unit)
{
    // !!! To do !
    /*
    ESI = unit
    unit->unitType
    if(abilflags[unit->unitType] & 0x400)
    {
        incrementUnitScores(unit, -1);
        unit->statusFlags |= 1;
        incrementUnitScores(unit, 1);
    }
    else
    {
        unit->statusFlags |= 1;
    }
    incrementUnitScoresEx(unit, 1, 1);
    ECX = unit->unitType
    if(abilflags[unit->unitType]) & 0x400000)
    {
        PlaySoundFromDirect(unit, 0x111);
    }
    EAX = unit->unitType
    if(unit->unitType != 0x21 && unit->unitType != 0x65)
    {
        EDX = unit->sprite
        if((unit->sprite->flags & 0x20) && // Hidden
            unit->unitType != 0x49 && // Interceptor
            unit->unitType != 0x55) // Scarab
        {
            initUnitTrapDoodad(unit); // (EDI)
        }
        EBX = 0
        ECX = unit->unitType
        if(unit->unitType == 0xCB || // Floor Missile Trap
            unit->unitType == 0xD1 || // Flooor Gun Trap
            unit->unitType == 0xD2 || // Left Wall Missile Trap
            unit->unitType == 0xD3 || // Left Wall Flame Trap
            unit->unitType == 0xD4 || // Right Wall Missile Trap
            unit->unitType == 0xD5) // Right Wall Flame Trap
        {
            unit->statusFlags |= 0x300;
            unit->visibilityStatus = 0x80000000;
            unit->secondaryOrderTimer = 0;
        }
        EAX = unit->playerID
        EAX = TYPE(unit->playerID)
        if(TYPE(unit->playerID == rescuable))
    0049FB3C | . 3C 03          CMP AL, 3
    0049FB3E | . 0F85 00010000  JNZ StarCraf.0049FC44
        {
            CL = unit->userActionFlags | 1
            AL = unit->orderID
            EDI = 0
            unit->userActionFlags |= 1;
            [EBP-6] = 0;
            [EBP-8] = 0;
            if(unit->orderID == 0) goto a thing 0049FC3B
            do
            {
    
    0049FB70 | > 8B46 78 / MOV EAX, DWORD PTR DS : [ESI + 78]
    0049FB73 | . 3BC3 | CMP EAX, EBX
    0049FB75 | . 0F84 AB000000 | JE StarCraf.0049FC26
    0049FB7B | . 8A48 08 | MOV CL, BYTE PTR DS : [EAX + 8]
    0049FB7E | . 0FB6D1 | MOVZX EDX, CL
    0049FB81 | . 389A 40506600 | CMP BYTE PTR DS : [EDX + 665040], BL
    0049FB87 | . 75 09 | JNZ SHORT StarCraf.0049FB92
    0049FB89 | . 80F9 A1 | CMP CL, 0A1
    0049FB8C | . 0F85 94000000 | JNZ StarCraf.0049FC26
    0049FB92 | > 8BCE | MOV ECX, ESI
    0049FB94 | .E8 3747FDFF | CALL StarCraf.004742D0; removeOrderFromUnitQueue
    0049FB99 | .^EB D5          \JMP SHORT StarCraf.0049FB70
            }
    }
    0049FB9B | > 3935 EC836200  CMP DWORD PTR DS : [6283EC], ESI
    0049FBA1 | . 75 08          JNZ SHORT StarCraf.0049FBAB
    0049FBA3 | . 8B46 04        MOV EAX, DWORD PTR DS : [ESI + 4]
    0049FBA6 | .A3 EC836200    MOV DWORD PTR DS : [6283EC], EAX
    0049FBAB | > 3935 28846200  CMP DWORD PTR DS : [628428], ESI
    0049FBB1 | . 75 08          JNZ SHORT StarCraf.0049FBBB
    0049FBB3 | . 8B0E           MOV ECX, DWORD PTR DS : [ESI]
    0049FBB5 | . 890D 28846200  MOV DWORD PTR DS : [628428], ECX
    0049FBBB | > 8B06           MOV EAX, DWORD PTR DS : [ESI]
    0049FBBD | . 33DB           XOR EBX, EBX
    0049FBBF | . 3BC3           CMP EAX, EBX
    0049FBC1 | . 74 06          JE SHORT StarCraf.0049FBC9
    0049FBC3 | . 8B56 04        MOV EDX, DWORD PTR DS : [ESI + 4]
    0049FBC6 | . 8950 04        MOV DWORD PTR DS : [EAX + 4], EDX
    0049FBC9 | > 8B46 04        MOV EAX, DWORD PTR DS : [ESI + 4]
    0049FBCC | . 3BC3           CMP EAX, EBX
    0049FBCE | . 74 04          JE SHORT StarCraf.0049FBD4
    0049FBD0 | . 8B0E           MOV ECX, DWORD PTR DS : [ESI]
    0049FBD2 | . 8908           MOV DWORD PTR DS : [EAX], ECX
    0049FBD4 | > 891E           MOV DWORD PTR DS : [ESI], EBX
    0049FBD6 | . 895E 04        MOV DWORD PTR DS : [ESI + 4], EBX
    0049FBD9 | .A1 F4836200    MOV EAX, DWORD PTR DS : [6283F4]
    0049FBDE | . 3BC3           CMP EAX, EBX
    0049FBE0 | . 74 2D          JE SHORT StarCraf.0049FC0F
    0049FBE2 | . 3905 34846200  CMP DWORD PTR DS : [628434], EAX
    0049FBE8 | . 75 06          JNZ SHORT StarCraf.0049FBF0
    0049FBEA | . 8935 34846200  MOV DWORD PTR DS : [628434], ESI
    0049FBF0 | > 8906           MOV DWORD PTR DS : [ESI], EAX
    0049FBF2 | . 8B50 04        MOV EDX, DWORD PTR DS : [EAX + 4]
    0049FBF5 | . 8956 04        MOV DWORD PTR DS : [ESI + 4], EDX
    0049FBF8 | . 8B48 04        MOV ECX, DWORD PTR DS : [EAX + 4]
    0049FBFB | . 3BCB           CMP ECX, EBX
    0049FBFD | . 74 02          JE SHORT StarCraf.0049FC01
    0049FBFF | . 8931           MOV DWORD PTR DS : [ECX], ESI
    0049FC01 | > 56             PUSH ESI; / Arg1
    0049FC02 | . 8970 04        MOV DWORD PTR DS : [EAX + 4], ESI; |
    0049FC05 | .E8 26630400    CALL StarCraf.004E5F30; \refreshUnitVision
    0049FC0A | .^E9 D7FEFFFF    JMP StarCraf.0049FAE6
    0049FC0F | > 56             PUSH ESI; / Arg1
    0049FC10 | . 8935 34846200  MOV DWORD PTR DS : [628434], ESI; |
    0049FC16 | . 8935 F4836200  MOV DWORD PTR DS : [6283F4], ESI; |
    0049FC1C | .E8 0F630400    CALL StarCraf.004E5F30; \refreshUnitVision
    0049FC21 | .^E9 C0FEFFFF    JMP StarCraf.0049FAE6
    0049FC26 | > 8B45 F8        MOV EAX, DWORD PTR SS : [EBP - 8]
    0049FC29 | . 57             PUSH EDI; / Arg2
    0049FC2A | . 50             PUSH EAX; | Arg1
    0049FC2B | . 33FF           XOR EDI, EDI; |
    0049FC2D | .BA E4000000    MOV EDX, 0E4; |
    0049FC32 | .B3 A1          MOV BL, 0A1; |
    0049FC34 | .E8 B749FDFF    CALL StarCraf.004745F0; \QueueOrderEx
    0049FC39 | . 33DB           XOR EBX, EBX
    wtf:
    0049FC3B | > 8BCE           MOV ECX, ESI
    0049FC3D | .E8 BE53FDFF    CALL StarCraf.00475000; orders_Nothing2
    0049FC42 | .EB 25          JMP SHORT StarCraf.0049FC69
        }
    0049FC44 | > 3C 07          CMP AL, 7
    0049FC46 | . 75 04          JNZ SHORT StarCraf.0049FC4C
    0049FC48 | .B1 A2          MOV CL, 0A2
    0049FC4A | .EB 18          JMP SHORT StarCraf.0049FC64
    0049FC4C | > 3C 01          CMP AL, 1
    0049FC4E | . 75 0B          JNZ SHORT StarCraf.0049FC5B
    0049FC50 | . 0FB7C9         MOVZX ECX, CX
    0049FC53 | . 8A89 A02E6600  MOV CL, BYTE PTR DS : [ECX + 662EA0]
    0049FC59 | .EB 09          JMP SHORT StarCraf.0049FC64
    0049FC5B | > 0FB7D1         MOVZX EDX, CX
    0049FC5E | . 8A8A 68226600  MOV CL, BYTE PTR DS : [EDX + 662268]
    0049FC64 | > E8 A756FDFF    CALL StarCraf.00475310; orderComputer_cl
    0049FC69 | > 0FB74E 64      MOVZX ECX, WORD PTR DS : [ESI + 64]
    0049FC6D | . 8B048D 8040660>MOV EAX, DWORD PTR DS : [ECX * 4 + 664080]
    0049FC74 | .F6C4 08        TEST AH, 8
    0049FC77 | . 74 54          JE SHORT StarCraf.0049FCCD
    0049FC79 | . 8B46 0C        MOV EAX, DWORD PTR DS : [ESI + C]
    0049FC7C | . 66:8B50 14     MOV DX, WORD PTR DS : [EAX + 14]
    0049FC80 | . 66:8996 D00000>MOV WORD PTR DS : [ESI + D0], DX
    0049FC87 | . 66:8B40 16     MOV AX, WORD PTR DS : [EAX + 16]
    0049FC8B | . 66:8986 D20000>MOV WORD PTR DS : [ESI + D2], AX
    0049FC92 | . 8A86 93000000  MOV AL, BYTE PTR DS : [ESI + 93]
    0049FC98 | . 8A89 68226600  MOV CL, BYTE PTR DS : [ECX + 662268]
    0049FC9E | . 0C 01          OR AL, 1
    0049FCA0 | . 8886 93000000  MOV BYTE PTR DS : [ESI + 93], AL
    0049FCA6 | . 33C0           XOR EAX, EAX
    0049FCA8 | . 68 E4000000    PUSH 0E4; / Arg3 = 000000E4
    0049FCAD | . 50             PUSH EAX; | Arg2 = > 00000000
    0049FCAE | . 66:895D FA     MOV WORD PTR SS : [EBP - 6], BX; |
    0049FCB2 | . 66:895D F8     MOV WORD PTR SS : [EBP - 8], BX; |
    0049FCB6 | . 8B55 F8        MOV EDX, DWORD PTR SS : [EBP - 8]; |
    0049FCB9 | . 52             PUSH EDX; | Arg1
    0049FCBA | .B8 01000000    MOV EAX, 1; |
    0049FCBF | . 8BD6           MOV EDX, ESI; |
    0049FCC1 | .E8 4A4BFDFF    CALL StarCraf.00474810; \order
    0049FCC6 | . 8BCE           MOV ECX, ESI
    0049FCC8 | .E8 3353FDFF    CALL StarCraf.00475000; orders_Nothing2
    0049FCCD | > 53             PUSH EBX; / Arg1
    0049FCCE | . 8BC6           MOV EAX, ESI; |
    0049FCD0 | .E8 2B1BF9FF    CALL StarCraf.00431800; \getUnitStrength
    0049FCD5 | . 66:8986 380100>MOV WORD PTR DS : [ESI + 138], AX
    0049FCDC | . 6A 01          PUSH 1; / Arg1 = 00000001
    0049FCDE | . 8BC6           MOV EAX, ESI; |
    0049FCE0 | .E8 1B1BF9FF    CALL StarCraf.00431800; \getUnitStrength
    0049FCE5 | . 5F             POP EDI
    0049FCE6 | . 66:8986 3A0100>MOV WORD PTR DS : [ESI + 13A], AX
    0049FCED | . 5E             POP ESI
    0049FCEE | . 5B             POP EBX
    0049FCEF | . 8BE5           MOV ESP, EBP
    0049FCF1 | . 5D             POP EBP
    0049FCF2  \.C3             RETN
    */
}

void Graphics::unknown_00462960(UnitNode* unit)
{
    // !!! To do !
    /*
    unknown_00462960
    00462960 / $ 55             PUSH EBP
    00462961 | . 8BEC           MOV EBP, ESP
    00462963 | . 51             PUSH ECX
    00462964 | . 8B45 08        MOV EAX, DWORD PTR SS : [EBP + 8]
    00462967 | . 56             PUSH ESI
    00462968 | . 0FB670 4C      MOVZX ESI, BYTE PTR DS : [EAX + 4C]
    0046296C | . 8D0CF6         LEA ECX, DWORD PTR DS : [ESI + ESI * 8]
    0046296F | . 803C8D E8EE570>CMP BYTE PTR DS : [ECX * 4 + 57EEE8], 1
    00462977 | . 0F85 C5000000  JNZ StarCraf.00462A42
    0046297D | . 0FB748 64      MOVZX ECX, WORD PTR DS : [EAX + 64]
    00462981 | .F681 78016600 >TEST BYTE PTR DS : [ECX + 660178], 2
    00462988 | . 894D FC        MOV DWORD PTR SS : [EBP - 4], ECX
    0046298B | . 0F85 B1000000  JNZ StarCraf.00462A42
    00462991 | . 8B40 0C        MOV EAX, DWORD PTR DS : [EAX + C]
    00462994 | . 53             PUSH EBX
    00462995 | . 0FBF58 16      MOVSX EBX, WORD PTR DS : [EAX + 16]
    00462999 | . 57             PUSH EDI
    0046299A | . 0FBF78 14      MOVSX EDI, WORD PTR DS : [EAX + 14]
    0046299E | . 8B04F5 0C51680>MOV EAX, DWORD PTR DS : [ESI * 8 + 68510C]
    004629A5 | . 85C0           TEST EAX, EAX
    004629A7 | . 74 2C          JE SHORT StarCraf.004629D5
    004629A9 | . 8DA424 0000000>LEA ESP, DWORD PTR SS : [ESP]
    004629B0 | > 8B50 0C / MOV EDX, DWORD PTR DS : [EAX + C]
    004629B3 | . 85D2 | TEST EDX, EDX
    004629B5 | . 75 18 | JNZ SHORT StarCraf.004629CF
    004629B7 | . 0FB750 10 | MOVZX EDX, WORD PTR DS : [EAX + 10]
    004629BB | . 3BD1 | CMP EDX, ECX
    004629BD | . 75 10 | JNZ SHORT StarCraf.004629CF
    004629BF | . 0FBF50 16 | MOVSX EDX, WORD PTR DS : [EAX + 16]
    004629C3 | . 3BD7 | CMP EDX, EDI
    004629C5 | . 75 08 | JNZ SHORT StarCraf.004629CF
    004629C7 | . 0FBF50 18 | MOVSX EDX, WORD PTR DS : [EAX + 18]
    004629CB | . 3BD3 | CMP EDX, EBX
    004629CD | . 74 51 | JE SHORT StarCraf.00462A20
    004629CF | > 8B00 | MOV EAX, DWORD PTR DS : [EAX]
    004629D1 | . 85C0 | TEST EAX, EAX
    004629D3 | . ^ 75 DB          \JNZ SHORT StarCraf.004629B0
    004629D5 | > 8B14F5 0851680>MOV EDX, DWORD PTR DS : [ESI * 8 + 685108]
    004629DC | . 8B82 007D0000  MOV EAX, DWORD PTR DS : [EDX + 7D00]
    004629E2 | . 85C0           TEST EAX, EAX
    004629E4 | . 74 5A          JE SHORT StarCraf.00462A40
    004629E6 | . 8B08           MOV ECX, DWORD PTR DS : [EAX]
    004629E8 | . 898A 007D0000  MOV DWORD PTR DS : [EDX + 7D00], ECX
    004629EE | . 33D2           XOR EDX, EDX
    004629F0 | . 3BCA           CMP ECX, EDX
    004629F2 | . 74 03          JE SHORT StarCraf.004629F7
    004629F4 | . 8951 04        MOV DWORD PTR DS : [ECX + 4], EDX
    004629F7 | > 3BC2           CMP EAX, EDX
    004629F9 | . 74 45          JE SHORT StarCraf.00462A40
    004629FB | . 8D0CF5 0C51680>LEA ECX, DWORD PTR DS : [ESI * 8 + 68510C]
    00462A02 | . 8950 04        MOV DWORD PTR DS : [EAX + 4], EDX
    00462A05 | . 8B11           MOV EDX, DWORD PTR DS : [ECX]
    00462A07 | . 8910           MOV DWORD PTR DS : [EAX], EDX
    00462A09 | . 8B11           MOV EDX, DWORD PTR DS : [ECX]
    00462A0B | . 85D2           TEST EDX, EDX
    00462A0D | . 74 03          JE SHORT StarCraf.00462A12
    00462A0F | . 8942 04        MOV DWORD PTR DS : [EDX + 4], EAX
    00462A12 | > 8901           MOV DWORD PTR DS : [ECX], EAX
    00462A14 | . 66 : 8B4D FC     MOV CX, WORD PTR SS : [EBP - 4]
    00462A18 | .C640 08 01     MOV BYTE PTR DS : [EAX + 8], 1
    00462A1C | . 66 : 8948 10     MOV WORD PTR DS : [EAX + 10], CX
    00462A20 | > 8B4D 08        MOV ECX, DWORD PTR SS : [EBP + 8]
    00462A23 | . 8948 0C        MOV DWORD PTR DS : [EAX + C], ECX
    00462A26 | . 66 : 8978 12     MOV WORD PTR DS : [EAX + 12], DI
    00462A2A | . 66 : 8958 14     MOV WORD PTR DS : [EAX + 14], BX
    00462A2E | . 66 : 8978 16     MOV WORD PTR DS : [EAX + 16], DI
    00462A32 | . 66 : 8958 18     MOV WORD PTR DS : [EAX + 18], BX
    00462A36 | .C640 09 00     MOV BYTE PTR DS : [EAX + 9], 0
    00462A3A | . 8981 34010000  MOV DWORD PTR DS : [ECX + 134], EAX
    00462A40 | > 5F             POP EDI
    00462A41 | . 5B             POP EBX
    00462A42 | > 5E             POP ESI
    00462A43 | . 8BE5           MOV ESP, EBP
    00462A45 | . 5D             POP EBP
    00462A46  \.C2 0400        RETN 4
    */
}

void Graphics::UnitDestructor(UnitNode* unit)
{
    if (unit->subUnit != NULL)
    {
        
        if (chkd.scData.UnitDat(unit->subUnit->unitType)->SpecialAbilityFlags & 10) // has subunit
        {
            UnitDestructor(unit->subUnit);
            unit->subUnit = NULL;
        }
    }
    _UnitDestructor(unit);
    SpriteDestructor(unit->sprite);
    unit->sprite = NULL;
    return;
}

void Graphics::_UnitDestructor(UnitNode* unit)
{
    u32 abilflags = chkd.scData.UnitDat(unit->unitType)->SpecialAbilityFlags;
    if (abilflags & 1) // Building
    {
        //*6D5BD0 = 1 // I duno lol
    }

    UnitDestructor_SpecialCases(unit);

    // We're probably not doing these things
    //killAllLoadedUnits(unit); // killAllLoadedUnits 0049FDD0 000000FF (EBX)
    //selectNewGUIUnitPortrait(1); // selectNewGUIUnitPortrait 0049EB70 000000B3 (?, push)
    //clearBulletTargets(unit); // clearBulletTargets 0048AAC0 0000003F (ECX)
    //ClearUnitFromGlobalAIEntry(unit, 1); // ClearUnitFromGlobalAIEntry 00439D60 000001E2 (push, push)
    //DetachUnitTaskFromTown(0); // DetachUnitTaskFromTown 00434C90 000000B5 (?, push)

    // No AI.
    /*if (unit->pAI != NULL && (u8*)unit->pAI[8] == 1)
    {
        (u32*)unit->pAI[3] = 0; // +C
        if ((u8*)unit->pAI[9] < 64)
        {
            (u8*)unit->pAI[9]++;
        }
    }*/
    unit->pAI = NULL;

    //unknown_004477C0(unit); // Some player medic list thing
    if (unknown_004A0080(unit) == false)
    {
        //setResourceTarget(unit); // setResourceTarget 004EB290 0000004B (EAX)
        if ((unit->sprite->flags & 0x20) == 0) // Hidden
        {
            //updateUnitSorting(unit); // updateUnitSorting 0046A560 0000003C (EAX)
        }
        if ((abilflags & 1) == 0) // Building
        {
            unit->isCloaked = 0;
            IsUnitCloaking(unit);
            if (unit->statusFlags & 0x100) // RequiresDetection
            {
                DoCloak(unit, 0x112); // TPhClo01.wav
            }
        }
        unknown_0049FD00(unit); // (EAX)
        if ((abilflags & 0x20) && // Flying Building
            unit->building.landingTimer != 0)
        {
            //updateBuildingLandFog2(unit, unit->orderTarget.x, unit->orderTarget.y); // updateBuildingLandFog2 00469EC0 00000074 (EAX, push, push)
        }
        
        // We have no pathing
        /*if (unit->path != NULL)
        {
            reAssignPath(unit); // reAssignPath 0042F740 0000004B (ESI)
        }*/

        // We can't build units.
        /*if (unit->currentBuildUnit != NULL)
        {
            if (unit->secondaryOrderID == 0x26 || unit->secondaryOrderID == 0x3F)
            {
                orders_SelfDestructing(unit->currentBuildUnit); // orders_SelfDestructing 00475710 000000E4 (EAX) ?
            }
        }*/

        if (unit->secondaryOrderID != 0x17)
        {
            unit->secondaryOrderID = 0x17;
            unit->secondaryOrderPosition.x = 0;
            unit->secondaryOrderPosition.y = 0;
            unit->currentBuildUnit = NULL;
            unit->secondaryOrderState = 0;
        }

        //ResetUnitPowerup(unit); // ResetUnitPowerup 004F3B70 000000EB (push)
        unknown_0049A7F0(unit); // Deselect unit for all players
        //unknown_0049F7A0(unit); // Reselect other units for player
        removeSelectionCircle(unit->sprite); // removeSelectionCircle 004975D0 00000048 (ECX)
        //incrementUnitScores(unit, -1); // I hate scores
        /*if (unit->statusFlags & 1) // Completed
        {
            incrementUnitScoresEx(-1, 0); // (ECX, push)
        }*/

        // I don't feel like looking in to this
        /*if (*6D0F14 == NULL)
        {
            if (unit->playerID == *512684)
            {
                if (*640880 != NULL)
                {
                    ECX = *597248
                    AX = *(u16*)64088A
                    DL = *(u8*)64088D
                    EAX = CMDRECV_PlaceBuildingAllowed( ? ); // CMDRECV_PlaceBuildingAllowed 0048DBD0 00000062
                    if (EAX == 0)
                    {
                        refreshLayer3And4( ? ); // refreshLayer3And4 0048D9A0 00000081
                        refreshPlaceBuildingLocation( ? ); // refreshPlaceBuildingLocation 0048E310 00000120
                    }
                }
            }
        }*/
        unknown_0049E3F0(unit); // (EAX)
        //*68C1B0 = 1;
        //*(u8*)68AC74 = 1;
        //*(u8*)68C1F8 = 1;
        //*68C1E8 = NULL;
        //*68C1EC = NULL;
    }
}

void Graphics::UnitDestructor_SpecialCases(UnitNode* unit)
{
    /*if ((unit->unitType >= 0xB0 && unit->unitType <= 0xB2) || // Mineral fields 1, 2, 3
        ( // unit_isGeyserUnit 004688B0 00000028 (EAX)
            (unit->statusFlags & 1) != 0 && // Completed
            (unit->unitType == 0x6E || // Refinery
             unit->unitType == 0x9D || // Assimilator
             unit->unitType == 0x95)) ) // Extractor
    {
        //unknown_00468770(unit); // Remove harvesters
    }*/

    switch (unit->unitType)
    {
        case 0x48: // Carrier
        case 0x52: // Gantrithor
        case 0x53: // Reaver
        case 0x51: // Warbringer
            // We don't have hangars
            //killAllHangerUnits(unit); // killAllHangerUnits 00466170 000000FC (ESI ?)
            return;

        case 0x01: // Gost
        case 0x10: // Kerrigan
        case 0x64: // Stukov
        case 0x63: // Duran
        case 0x68: // Infested Duran
            // We don't have nuke dots
            /*if (unit->ghost.nukeDot != NULL)
            {
                //ECX = [EAX+C]
                playSpriteIscript(unit->ghost.nukeDot->sprite, 1, 1); // playSpriteIscript 00499D00 00000036 (ECX, push, push)
            }
            unit->ghost.nukeDot = NULL;*/
            return;

        case 0x49: // Interceptor
        case 0x55: // Scarab
            // We don't have hangar units
            /*if (unit->statusFlags & 1) // Completed
            {
                unknown_004652A0(unit); // EAX // Kill hangar units
            }*/
            return;

        case 0x6C: // Nuke Silo
            // We don't have working silos
            /*if (unit->silo.pNuke != NULL)
            {
                orders_SelfDestructing(unit->silo.pNuke); // orders_SelfDestructing 00475710 000000E4 (EAX)
                unit->silo.pNuke = NULL;
            }*/
            return;

        case 0xE: // Nuke missile
            if (unit->connectedUnit == NULL) return;
            if (unit->connectedUnit->unitType != 0x6C) return; // nuke silo
            unit->connectedUnit->silo.pNuke = NULL;
            unit->connectedUnit->silo.bReady = false;
            return;

        case 0x9C: // Pylon
            if (unit->pylon.pPowerTemplate != NULL)
            {
                SpriteDestructor(unit->pylon.pPowerTemplate);
                unit->pylon.pPowerTemplate = NULL;
            }
            // Dunno how in-depth this will go -- we may want to show enabled protoss buildings
            //removeFromPsiProviderList(unit); // removeFromPsiProviderList 00493100 00000051 (EAX)
            //*63FF44 = 1;
            return;

        case 0x86: // Nydus Canal
        {
            UnitNode* exit = unit->nydus.exit;
            if (exit == NULL) return;
            unit->nydus.exit = NULL;
            exit->nydus.exit = NULL;
            // We don't want to kill our other nydus !
            //orders_SelfDestructing(exit); // orders_SelfDestructing 00475710 000000E4 (EAX)
            return;
        }
    }
    return;
}

bool Graphics::unknown_004A0080(UnitNode* unit) // Destroy subunits and geyser buildings -- true on destruction
{
    if (chkd.scData.UnitDat(unit->unitType)->SpecialAbilityFlags & 0x10) // Subunit
    { // Destruct the subunit
        /*if (*62842C != NULL) // I don't know what these are, I assume just more empty unit lists.
        {
            if (*59CC98 == *62842C)
            {
                *59CC98 = unit
            }
            unit->prev = *62842C
            unit->next = *62842C->next
            if (*62842C->next != NULL)
            {
                *62842C->next->prev = unit
            }
            *62842C->next = unit
        }
        else
        {
            *59CC98 = unit
            *62842C = unit
        }*/

        // So that's how I will treat them.
        if (unitEmptyNode != NULL)
        {
            unit->prev = unitEmptyNode;
            unit->next = unitEmptyNode->next;
            if (unitEmptyNode->next != NULL)
            {
                unitEmptyNode->next->prev = unit;
            }
            unitEmptyNode->next = unit;
        }
        else
        {
            unitEmptyNode = unit;
        }
        
        // Some player unit list crap
        /*if (unit->previousPlayerUnit != NULL)
        {
            unit->previousPlayerUnit->nextPlayerUnit = unit->nextPlayerUnit;
        }
        if (unit->nextPlayerUnit != NULL)
        {
            unit->nextPlayerUnit->previousPlayerUnit = unit->previousPlayerUnit;
        }
        if (6283F8[unit->playerID] == unit)
        {
            6283F8[unit->playerID] = unit->nextPlayerUnit;
        }
        unit->previousPlayerUnit = NULL;
        unit->nextPlayerUnit = NULL;*/
        //incrementUnitScores(unit, -1);
        unit->subUnit = NULL;
        return true;
    }
    if (unit->sprite->flags & 0x20) return false; // Hidden

    if (unit->unitType == 0x6E || // Refinery
        unit->unitType == 0x95 || // Extractor
        unit->unitType == 0x9D) // assimilator
    {
        if (unit->unitType == 0x95) // Extractor
        {
            // remove creep ?
            //unknown_00414560(unit->sprite->position.x, unit->sprite->position.y, unknown_0049DEF0); // (ECX, EAX, push) // unknown_0049DEF0() { return 0; }
        }
        // Replaces geyser units with geyser. But we aren't doing this. We want it dead.
        /*if (unit->statusFlags & 1) // Completed
        {
            //incrementUnitScoresEx(-1, 0); // (ECX, push)
        }
        unit->statusFlags &= ~1;
        replaceUnitWithType(unit, 0xBC); // replaceUnitWithType 0049FED0 000001A8 (EAX, push) // Vespene Geyser
        unit->orderID = unitsdat.humanInit[unit->unitType];
        unit->orderTargetUnit = NULL;
        unit->orderTarget.x = 0;
        unit->orderTarget.y = 0;
        unit->orderState = 0;
        unit->orderUnitType = 0xE4; // None
        unit->hitPoints = unitsdat.maxHP[unit->unitType]
        unknown_0049EFA0(unit, 0xB, 0); // (ECX, push, push)
        unknown_0049E4E0(unit, 0xB); // (ECX, EDX)
        unit->statusFlags |= 1; // Completed
        //incrementUnitScoresEx(1, 1); // (ECX, push)
        if (*640880 != NULL)
        {
            placebuildingRefineryCheck(unit); // placebuildingRefineryCheck 0048E1E0 0000002F (EAX)
        }
        return true;*/
    }
    return false;
}

bool Graphics::IsUnitCloaking(UnitNode* unit) // More like is unit unburrowing
{
    if (unit->isCloaked == 1)
    {
        return false;
    }
    bool unburrowed = false; // Removed from burrow list ?
    //if (*63FF5C == unit) // First burrowed unit, I presume
    //{
    //  *63FF5C = unit->nextBurrowedUnit;
    //  unburrowed = true;
    //}
    if (unit->previousBurrowedUnit != NULL)
    {
        unit->previousBurrowedUnit->nextBurrowedUnit = unit->nextBurrowedUnit;
        unburrowed = true;
    }
    if (unit->nextBurrowedUnit != NULL)
    {
        unit->nextBurrowedUnit->previousBurrowedUnit = unit->previousBurrowedUnit;
        unburrowed = true;
    }
    unit->statusFlags &= ~0x800; // CloakingForFree
    unit->nextBurrowedUnit = NULL;
    unit->previousBurrowedUnit = NULL;
    if (unburrowed == true)
    {
        // I have no idea what these are.
        //*68C1B0 = 1;
        //*68AC74 = 1;
        //*68C1F8 = 1;
        //*68C1E8 = NULL;
        //*68C1EC = NULL;
    }
    return true;
}

void Graphics::DoCloak(UnitNode* unit, int sound)
{
    if ((unit->statusFlags & 0x10) == 0) // Burrowed
    {
        //BWFXN_PlaySndFromSrc(sound, 1, 0); // (EBX, push, push)
    }
    /*if (*(u16*)6D0F14 == 0)
    {
        if (unit->playerID == g_LocalNationID)
        {
            // Signal relisting burrowed units or something? I still don't know.
            *68C1B0 = 1;
            *(u8*)68AC74 = 1;
            *(u8*)68C1F8 = 1;
            *68C1E8 = NULL;
            *68C1EC = NULL;
        }
    }*/
    if (unit->statusFlags & 0x10) // Burrowed
    {
        updateSpriteFlagOnPalette(unit->sprite); // Set all cloaked images to normal
        unknown_00498170(unit->sprite); // Make images hidden by cloak visible
        unit->statusFlags &= ~0x300; // Cloaked | RequiresDetection
        return;
    }
    UpdateSpriteImagesForCloak(unit->sprite); // UpdateSpriteImagesForCloak 00498A90 0000008A (EDI)
    unit->statusFlags &= ~0x100;
    unit->statusFlags |= 0x200;
    if (unit->subUnit != NULL)
    {
        UpdateSpriteImagesForCloak(unit->subUnit->sprite);
        unit->subUnit->statusFlags &= ~0x100;
        unit->subUnit->statusFlags |= 0x200;
    }
    return;
}

void Graphics::updateSpriteFlagOnPalette(SpriteNode* sprite) // Uncloak sprite
{
    for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
    {
        if (image->paletteType >= DRAW_ENEMY_CLOAK && image->paletteType <= DRAW_DECLOAK) // Cloaked, any of them
        {
            image->paletteType = DRAW_NORMAL;
            image->updateFunction = imgUpdateFxns[DRAW_NORMAL].func;
            if (image->flags & 2) // Flipped
            {
                image->renderFunction = imgRenderFxns[DRAW_NORMAL].flip;
            }
            else {
                image->renderFunction = imgRenderFxns[DRAW_NORMAL].norm;
            }
            image->flags |= 1; // Redraw
        }
    }
}

void Graphics::unknown_00498170(SpriteNode* sprite) // Show images hidden by cloak
{
    for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
    {
        if (chkd.scData.ImageDat(image->imageID)->DrawIfCloaked == 0 &&
            (image->flags & 0x40) == 1) // Hidden/Invisible
        {
            image->flags &= ~40; // Hidden/Invsiible
            image->flags |= 1; // Redraw
        }
    }
    return;
}

void Graphics::UpdateSpriteImagesForCloak(SpriteNode* sprite)
{
    for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
    {
        if (image->paletteType == DRAW_ENEMY_CLOAK ||
            image->paletteType == DRAW_OWN_CLOAK ||
            image->paletteType == DRAW_CLOAK ||
            image->paletteType == DRAW_CLOAKED)
        {
            image->paletteType = image->paletteType < DRAW_ALLY_CLOAK ? DRAW_ALLY_CLOAK : DRAW_DECLOAK;
            image->updateFunction = imgUpdateFxns[image->paletteType].func;
            if (image->flags & 2) // Flipped
            {
                image->renderFunction = imgRenderFxns[image->paletteType].flip;
            }
            else
            {
                image->renderFunction = imgRenderFxns[image->paletteType].norm;
            }
            // Come on, Blizzard, you can do better than that.
            /*if (image->paletteType == DRAW_WARP_FLASH)
            {
                image->coloringData.warpFlash.state = 0x30;
                image->coloringData.warpFlash.timer = 0x02;
            }*/
            image->coloringData.cloak.state = 8;
            image->coloringData.cloak.timer = 3;
            image->flags |= 1; // Redraw
        }
    }

    unknown_00498170(sprite);
}

void Graphics::unknown_0049FD00(UnitNode* unit)
{
    if (unit->statusFlags & 2) // GoundedBuilding
    {
        // No incomplete addons for us
        /*if (unit->secondaryOrderID == 0x25 && // Build Add-on
            unit->currentBuildUnit != NULL &&
            (unit->currentBuildUnit->statusFlags & 1) == 0) // Complete
        {
            Base_CancelStructure( ? ); // Base_CancelStructure 00468280 0000019A
        }*/

        if (unit->building.addon != NULL)
        {
            bldgLiftoff(unit); // bldgLiftoff 00464930 00000079 (EAX)
        }

        // No incomplete techs, researches, or build queues either
        /*if (unit->techType != 0x2C) // none
        {
            Unit_RefundPartialTechnology( ? ); // Unit_RefundPartialTechnology 00453DD0 00000059
        }
        if (unit->upgradeType != 0x3D) // none
        {
            Unit_RefundPartialUpgrade( ? ); // Unit_RefundPartialUpgrade 00454220 0000005D
        }
        if (unit->buildQueue[unit->buildQueueSlot % 5] <= 0x69)
        {
            EAX = unit
                refundAllQueueSlots(unit); // refundAllQueueSlots 00466E80 00000059 (EAX)
        }*/

        // I don't know what this is.
        /*if (*640880 != NULL)
            pracebuildingProc( ? ); // pracebuildingProc 0048E6E0 0000004D
        */
    }
    
    // Huge functions I don't feel like dealing with
    /*if (chkd.scData.UnitDat(unit->unitType)->StarEditGroupFlags & 1) // Zerg
    {
        if (isMorphing(unit)) // It probably is.
        {
            unit->statusFlags |= 1; // complete
        }
        if (unknown_0047DF90(unit))
        {
            unit->statusFlags |= 0x4000; // UNKNOWN2
        }
    }*/
}

void Graphics::unknown_0049A7F0(UnitNode* unit) // Deselect unit for all players
{
    // Deselects unit for all players
    /*for (int i = 0; i < 8; i++)
    {
        unknown_0049A170(unit, i); // Deselect unit for player
    }*/
    if (unit->sprite->flags & 6) // selected ?
    {
        unit->sprite->flags &= ~6;
        for (ImageNode* image = unit->sprite->pImageTail; image != NULL; image = image->prev)
        {
            if (image->imageID < 0x23B) continue;
            if (image->imageID > 0x244) continue;
            //removeSelectionCircleImage(/*?*/ );
        }
    }
    // unknown_004BF8C0(); // Something stupid that's probably stupid
    // if 0059724C is the current player's selected units then it removes current player's unit selection, like the function above
}

void Graphics::unknown_0049E3F0(UnitNode* unit) // Remove from lists
{
    // We don't have a player unit list
    /*if (unit->previousPlayerUnit != NULL)
    {
        unit->previousPlayerUnit->nextPlayerUnit = unit->nextPlayerUnit;
    }
    if (unit->nextPlayerUnit != NULL)
    {
        unit->nextPlayerUnit->previousPlayerUnit = unit->previousPlayerUnit;
    }
    if (*6283F8[unit->playerID] == unit)
    {
        that = unit->nextPlayerUnit;
    }*/
    unit->previousPlayerUnit = NULL;
    unit->nextPlayerUnit = NULL;

    // Too complicated
    /*if (unit->unitType != 0x21 && // Scanner sweep
        unit->unitType != 0x65) // Map revealer
    {
        if (unit->sprite->flags & 0x20) // Hidden
        {
            CListRemoveHiddenUnitEntry();
        }
        else
        {
            CListRemoveUsedUnitEntry();
        }
    }
    else
    {
        if (*6283F4 == unit)
        {
            *6283F4 = unit->next;
        }
        if (*628434 == unit)
        {
            *628434 == unit->prev;
        }
        if (unit->prev != NULL)
        {
            unit->prev->next = unit->next;
        }
        if (unit->next != NULL)
        {
            unit->next->prev = unit->prev;
        }
        unit->prev = NULL;
        unit->next = NULL;
    }
    if (*62842C != NULL)
    {
        if (*59CC98 == *62842C)
        {
            *59CC98 = unit;
        }
        unit->prev = *62842C;
        unit->next = *62842C->next
        if (*62842C->next != NULL)
        {
            unit->next->prev = unit;
        }
        *62842C->next = unit;
        return;
    }
    *59CC98 = unit;
    *62842C = unit;*/
    
    // Remove from list, EZ!
    if (unitLastCreated == unit) unitLastCreated = unit->prev;
    if (unit->next != NULL) unit->next->prev = unit->prev;
    if (unit->prev != NULL) unit->prev->next = unit->next;
    unit->prev = NULL;
    unit->next = NULL;

    // Add to empty list, EZ!
    if (unitEmptyNode != NULL)
    {
        unit->prev = unitEmptyNode;
        unit->next = unitEmptyNode->next;
        if (unitEmptyNode->next != NULL) unitEmptyNode->next->prev = unit;
        unitEmptyNode->next = unit;
    }
    else
    {
        unitEmptyNode = unit;
    }
}

void Graphics::bldgLiftoff(UnitNode* unit)
{

    UnitNode* addon = unit->building.addon;
    GiveUnitToNeutral(addon); // GiveUnitToNeutral 0047CB90 00000071

    // I dno what these are
    //*68C1B0 = 1;
    //*(u8*)68AC74 = 1;
    //*(u8*)68C1F8 = 1;

    unit->building.addon = NULL;
    //*68C1E8 = NULL;
    //*68C1EC = NULL;

    //PlayBuildingLiftoffSound(); // PlayBuildingLiftoffSound 0048F8A0 00000062
    
    UnitNode* tmp = activeIscriptUnit;
    activeIscriptUnit = addon;
    for (ImageNode* image = addon->sprite->pImageHead; image != NULL; image = image->next)
    {
        playImageIscript(image, ANIM_LIFTOFF);
    }
    activeIscriptUnit = tmp;
    return;
}

void Graphics::GiveUnitToNeutral(UnitNode* unit)
{
    /*if (unit->secondaryOrderID == 0x25 && // Build Addon
        unit->statusFlags & 2 && // GoundedBuilding
        unit->currentBuildUnit != NULL &&
        (unit->currentBuildUnit->statusFlags & 1) == 0) // completed
    {
        Base_CancelStructure(? ); // Base_CancelStructure 00468280 0000019A
    }*/
    GiveAllUnitPropertiesToPlayerAndMakeIdle(unit, 0xB); // GiveAllUnitPropertiesToPlayerAndMakeIdle 0047C9F0 00000192
    if ((unit->statusFlags & 0x40000000) == 0) // IsHallucination
    {
        if (unit->secondaryOrderID != 0x17) // Nothing2
        {
            unit->secondaryOrderID = 0x17;
            unit->secondaryOrderPosition.x = 0;
            unit->secondaryOrderPosition.y = 0;
            unit->currentBuildUnit = NULL;
            unit->secondaryOrderState = 0;
        }
    }
}

void Graphics::GiveAllUnitPropertiesToPlayerAndMakeIdle(UnitNode* unit, u8 player)
{
    //researchAllTechsFromUnit(unit, researchTech, player);
    //upgradeAllUpgradesFromUnit(unit, increaseUpgradeLevel, player);
    //unknown_0049A7F0(); // Deselect unit
    //unknown_0049F7A0(); // Update unit selections

    // Refunds
    /*if (unit->statusFlags & 2) // Grounded building
    {
        if (unit->buildQueue[unit->buildQueueSlot % 5] <= 0x69)
        {
            refundAllQueueSlots(unit); // refundAllQueueSlots 00466E80 00000059
            playSpriteIscript(unit->sprite, ANIM_WORKINGTOIDLE, 1); // playSpriteIscript 00499D00 00000036 (ECX, push, push)
        }
        if (unit->techType != 0x2C) // none
        {
            Unit_RefundPartialTechnology(unit); // Unit_RefundPartialTechnology 00453DD0 00000059 (EDX)
        }
        if (unit->upgradeType != 0x3D) // none
        {
            Unit_RefundPartialUpgrade(unit); // Unit_RefundPartialUpgrade 00454220 0000005D (EAX)
        }
    }
    if (unit->unitType == 0x48 || // Carrier
        unit->unitType == 0x52 || // Gantrithor
        unit->unitType == 0x53 || // Reaver
        unit->unitType == 0x51) // Warbringer
    {
        refundAllQueueSlots(unit); // refundAllQueueSlots 00466E80 00000059
    }*/
    //unknown_0049EFA0(unit, player, 1); // (ECX, push, push)
    if (player < 8)
    {
        //unknown_0049E4E0(unit, player); // (ECX, DL)
    }
    /*upgradeAllUpgradesFromUnit(unit, ApplyAllUnitSpeedUpgradesProc, 0); // ApplyAllUnitSpeedUpgradesProc 0047B940 0000001F
    if (unit->secondaryOrderID == 0x25 || // buildAddon
        (unit->statusFlags & 2) || // groundedBuilding
        unit->currentBuildUnit == NULL ||
        (unit->currentBuildUnit->statusFlags & 1)) // complete
    {
        if ((unit->statusFlags & 1) // complete
            (unitabilflags[unit->unitType] & 8) == 0 && // worker
            unit->unitType != 0x49 && // Interceptor
            unit->unitType != 0x55 && // Scarab
            unit->unitType != 0x0E && // Nuclear Missile
            (unit->statusFlags & 0x40) == 0) // In transport
        {
            switch (ActivePlayers[player].Type)
            {
            default: // Human
                orderComputer_cl(unitsdat.humanInit[unit->unitType]); // orderComputer_cl 00475310 00000040 (CL)
                return;

            case 7: // Neutral
                orderComputer_cl(0xA2); // Idle (Neutral)
                return;

            case 3: // Rescuable
                orderComputer_cl(0xA1); // Idle (Rescuable)
                return;

            case 1: // Computer
                orderComputer_cl(unitsdat.AIInit[unit->unitType]); // orderComputer_cl 00475310 00000040 (CL)
                return;
            }
        }
    }*/
}


void Graphics::getImageAttackFrame(point* out, SpriteNode* sprite, int goffs, u8 losrc)
{
    ISCRIPT_UseLOFile(sprite->pImagePrimary, losrc, goffs, out);
}

void Graphics::setSpriteMainImgOffset(SpriteNode* sprite, int unk1, int unk2) // (EAX, push, push)
{
    if (sprite->pImagePrimary->horizontalOffset != unk1 ||
        sprite->pImagePrimary->verticalOffset != unk2)
    {
        sprite->pImagePrimary->flags |= 1; // Redraw
        sprite->pImagePrimary->horizontalOffset = unk1;
        sprite->pImagePrimary->verticalOffset = unk2;
    }
    return;
}

bool Graphics::unknown_004A0320(UnitNode* unit, int unitid, int x, int y, int player)
{
    unit->statusFlags = 0;

    if (unknown_0049ECF0(unit, y, unitid, x, player) == 0)
    { // (EAX, EDX, push, push, push)
        return false;
    }

    //unit->wireframeRandomizer = getRand(); // RandomizeShort(15);

    if (unit->statusFlags & 2) { // GoundedBuilding
        unit->orderID = chkd.scData.UnitDat(unitid)->HumanAIIdle;
    }
    else
    {
        unit->orderID = 0x17; // "Nothing 2 (Normal)"
    }

    unit->orderState = 0;
    if (unit->secondaryOrderID != 0x17) { // "Nothing 2 (Normal)"
        unit->secondaryOrderID = 0x17;
        unit->secondaryOrderState = 0;
    }
    //incrementUnitScores(unit, 1); // (EAX, push) ? 
    if (unit->statusFlags & 2) { // GoundedBuilding
        //updateUnitOrderFinder(? ); // ?
        if (unit->statusFlags & 2) {
            updateBuildingLandFog(unit, unit->sprite->position.x, unit->sprite->position.y); // ? (EAX, push, push)
        }
        //getUnitCollision(unit, ... ? ); // (EAX)
        //if (unit->statusFlags & 4) { // InAir
        //  setRepulseAngle(unit, ... ? ); // (EAX?) ?
        //}
        SetConstructionGraphic(unit, 1); // ? (EDI, push)
        refreshAllVisibleImagesAtScreenPosition(unit->sprite, 0); // unit->sprite, EBX?
        //if (unk_640880 != NULL) {
            //pracebuildingProc(? );
        //}
        //EDI = -1;
    }
    else
    {
        unit->sprite->flags |= 0x20; // Hidden
        refreshAllVisibleImagesAtScreenPosition(unit->sprite, 0); // EBX ?
    }
    unit->visibilityStatus = -1;
    if (chkd.scData.UnitDat(unit->unitType)->SpecialAbilityFlags & 0x10) { // UPROP_SUBUNIT
        unit->sprite->flags |= 0x10; // subunit?
    }
    else
    {
        if (!(unit->sprite->flags & 0x20)) { // Hidden
            refreshUnitVision(unit); // (push) ?
        }
    }
    return true;
}

bool Graphics::unknown_0049ECF0(UnitNode* unit, int y, u16 unitid, int x, u8 player) {
    UnitNode* unittmp = activeIscriptUnit;

    UNITDAT* unitdat = chkd.scData.UnitDat(unitid);

    activeIscriptUnit = unit;
    if (!unknown_00496360(unitdat->Graphics, x, y, unit, player, 0)) // (EAX, ECX, EDX, push, push, push)
    {
        activeIscriptUnit = unittmp;
        return false;
    }
    unit->playerID = player;
    unit->orderID = 0xBC; // 188 - Fatal
    unit->orderState = 0;
    unit->orderSignal = 0;
    unit->orderUnitType = 0xE4; // 228 - None
    memset(&unit->mainOrderTimer, 0, 4);
    unit->orderTargetUnit = NULL;
    unit->orderTarget.x = 0;
    unit->orderTarget.y = 0;
    memset(&unit->building, 0, 16);
    unit->unitType = unitid;
    unit->resourceType = 0;
    unit->secondaryOrderTimer = 0;
    if (unit->sprite != NULL)
    {
        spriteToIscriptLoop(unit->sprite);
        if (unit->sprite->pImageHead == NULL)
        {
            unit->sprite = NULL;
        }
    }
    unit->lastAttackingPlayer = 8;
    activeIscriptUnit = unittmp;
    unit->sprite->elevationLevel = unitdat->ElevationLevel;
    unit->statusFlags ^= ((unitdat->SpecialAbilityFlags << 1) ^ unit->statusFlags) & 2; // Set GroundedBuilding if Building
    unit->statusFlags ^= (unitdat->SpecialAbilityFlags ^ unit->statusFlags) & 4; // Set InAir if UPROP_FLYER
    unit->statusFlags ^= (unitdat->SpecialAbilityFlags >> 12) & 0x10000; // Set IsAUnit // canAttack? if UPROP_AUTOATTACK
                                                                       // UPROP_BATTLEREACTIONS
    unit->statusFlags = ((~(((((unitdat->SpecialAbilityFlags >> 0xA) ^ unit->statusFlags) & 0x20000 ^ unit->statusFlags) >> 2) << 0x14) ^ (((unitdat->SpecialAbilityFlags >> 0xA) ^ unit->statusFlags) & 0x20000 ^ unit->statusFlags)) & 0x100000) ^ (((unitdat->SpecialAbilityFlags >> 0xA) ^ unit->statusFlags) & 0x20000 ^ unit->statusFlags); // What the fuuuuuck
    unit->pathingFlags ^= ((unitdat->ElevationLevel < 12) ^ unit->pathingFlags) & 1;
    if ((unit->unitType >= 194 && unit->unitType <= 199) || unit->unitType == 204) // Beacon or floor hatch
    {
        unit->statusFlags |= 0x200000; // NoCollide
    }
    if (unitdat->SpecialAbilityFlags & 1) // UPROP_BUILDING
    {
        unit->building.techType = 44; // None
        unit->building.upgradeType = 61; // None
    }
    unit->path = 0;
    unit->movementState = 0;
    unit->recentOrderTimer = 0;
    if(unitdat->SpecialAbilityFlags & 0x20000000) { // UPROP_INVINCIBLE
        unit->statusFlags |= 0x4000000; // Invincible
    }
    else
    {
        if (!(chkd.scData.UnitDat(unit->unitType)->SpecialAbilityFlags & 0x20000000)) {// UPROP_INVINCIBLE
            unit->statusFlags &= ~0x4000000; // Invincible
        }
    }
    //unit->buildingOverlayState = unk_5240D0[unit->sprite->pImagePrimary->imageID];
    //UpdateHPGain(? );
    //updateBuildingConstructShieldsAmount(unit); // (EBX)
    //ApplySpeedUpgradeFromUnitType(unit); // (ESI)
    //UpdateUnitSpeed(unit); // (EAX)
    return true;
}

void Graphics::SetConstructionGraphic(UnitNode* unit, int unk1)
{
    u8 cloaked = (unit->statusFlags >> 8) & 3; // RequiresDetection | Cloaked // [EBP - 9]
    u8 EBP5 = 0; // [EBP - 5]
    u8 EBP1 = 0; // [EBP - 1]

    if (cloaked)
    {
        EBP5 = unit->sprite->pImagePrimary->coloringData.cloak.state;
        EBP1 = unit->sprite->pImagePrimary->coloringData.cloak.timer;
    }
    UnitNode* tmpunit = activeIscriptUnit;
    activeIscriptUnit = unit;

    if (unk1 == 0 || chkd.scData.UnitDat(unit->unitType)->ConstructionAnimation == 0)
    {
        ReplaceSpriteOverlayImage(unit->sprite, chkd.scData.SpriteDat(unit->sprite->spriteID)->ImageFile, unit->currentDirection1);
    }
    else
    {
        ReplaceSpriteOverlayImage(unit->sprite, chkd.scData.UnitDat(unit->unitType)->ConstructionAnimation, unit->currentDirection1); // (EAX, push, push) ?
    }
    activeIscriptUnit = tmpunit;
    if (cloaked)
    {
        unknown_00497FD0(unit->sprite, EBP1, cloaked, EBP5); // (ESI, AL, push, push) ?
        if (unit->visibilityStatus & visionFilter)
        {
            makeSpriteVisible(unit->sprite);
        }
    }
    ApplyUnitEffects(unit);
    //unit->buildingOverlayState = unk_5240D0[unit->sprite->pImagePrimary->imageID];
    return;
}

void Graphics::playImageIscript(ImageNode* image, char anim)
{
    if (anim != ANIM_DEATH)
    {
        if (image->anim == ANIM_DEATH) // Is image dying?
        {
            return; // Can't stop it now !
        }
    }
    if (!(image->flags & 0x0010)) // Use Full Iscript
    {
        if (anim != ANIM_INIT && anim != ANIM_DEATH) return; // Init and Die only valid with no full iscript
    }

    if (anim == image->anim)
    {
        if (anim == ANIM_WALKING) return; // Is still walking
        if (anim == ANIM_ISWORKING) return; // Is still working
    }
    if (anim == ANIM_GNDATTKRPT)
    {
        if (anim != image->anim) // Previous animation is not gndattkrpt
        {
            if (image->anim != ANIM_GNDATTKINIT) // Attack been initialized?
            {
                anim = ANIM_GNDATTKINIT; // Can't repeat something if it hasn't been done yet !
            }
        }
    }
    else if (anim == ANIM_AIRATTKRPT) // Same set up as gndattkrpt
    {
        if (anim != image->anim)
        {
            if (image->anim != ANIM_AIRATTKINIT)
            {
                anim = ANIM_AIRATTKINIT;
            }
        }
    }

    image->anim = anim;
    image->iscriptOffset = chkd.scData.iScripts.GetAnimOffset(image->iscriptHeader, anim, true);
    image->iscriptReturn = 0;
    image->sleep = 0;

    iscript_OpcodeCases(image, image->iscriptHeader, 0, 0);
    return;
}

void Graphics::turnUnit(UnitNode* unit, int direction)
{
    unit->velocityDirection1 = direction;
    unit->currentDirection1 = direction;
    unit->currentDirection2 = direction;
    unit->current_speed.x = (arr_512D28[direction << 3].x * unit->current_speed1) >> 8;
    unit->current_speed.y = (arr_512D28[direction << 3].y * unit->current_speed1) >> 8;
    if (unit->sprite->position.x != unit->nextTargetWaypoint.x || unit->sprite->position.y != unit->nextTargetWaypoint.y) {
        unit->nextTargetWaypoint.x = unit->sprite->position.x;
        unit->nextTargetWaypoint.y = unit->sprite->position.y;
    }
    for (ImageNode* image = unit->sprite->pImageHead; image != NULL; image = image->next) {
        setImageDirection(image, direction);
    }
    return;
}

void Graphics::initUnitTrapDoodad(UnitNode* unit)
{
    if (!(unit->sprite->flags & 0x20)) return; // Hidden
    unit->sprite->flags &= ~0x20; // Hidden

    if (unit->subUnit != NULL) {
        if (chkd.scData.UnitDat(unit->subUnit->unitType)->SpecialAbilityFlags & 0x10) { // Sub Unit
            unit->subUnit->sprite->flags &= ~0x20; // Hidden
        }
    }
    refreshUnitVision(unit/*, ?*/ ); // (push)
    UpdateUnitSpriteInfo(unit); // (ESI ?)
    updateUnitOrderFinder(/*?*/ ); // unit?
    if (unit->statusFlags & 2) { // GoundedBuilding
        updateBuildingLandFog(unit, unit->sprite->position.x, unit->sprite->position.y); // (EAX, push, push) ?
    }
    //getUnitCollision(unit/*, ?*/ ); // (EAX) ?
    //if (unit->statusFlags & 4) { // InAir
    //  setRepulseAngle(unit); // (ESI) ?
    //}
    toggleUnitPath(unit); // (ESI) ?
    unit->movementState = 0;
    u8 elevationLevel = unit->sprite->elevationLevel;
    if (unit->sprite->elevationLevel < 0xC) elevationLevel = 1;
    elevationLevel ^= (unit->pathingFlags) & 1;
    unit->pathingFlags = unit->pathingFlags ^ elevationLevel;
    if (unit->subUnit) {
        if (chkd.scData.UnitDat(unit->subUnit->unitType)->SpecialAbilityFlags & 0x10) { // Is SubUnit
            toggleUnitPath(unit->subUnit); // (ESI, ?)
            unit->subUnit->movementState = 0;
            if (unit->subUnit->sprite->elevationLevel < 0xC) elevationLevel = 1;
            elevationLevel ^= unit->subUnit->pathingFlags;
            elevationLevel &= 1;
            unit->subUnit->movementFlags = unit->subUnit->pathingFlags ^ elevationLevel;
        }
    }

    /*CListRemoveHiddenUnitEntry(unit); // (EAX, ?)
    if (firstUnit != NULL) {
        // Check ?
        //if ((*p59CC9C) == firstUnit) { // I don't know what this is
        //  (*p59CC9C) = unit;
        //}
        unit->prev = firstUnit;
        unit->next = firstUnit->next;
        if (firstUnit->prev != NULL)
        {
            firstUnit->prev->prev = unit; // Check ? ->next->prev = unit ?
        }
        firstUnit->next = unit;
    }
    else {
        //(*p59CC9C) = unit;
        firstUnit = unit;
    }*/
    if (unitLastCreated != NULL)
    {
        unitLastCreated->next = unit;
        unit->prev = unitLastCreated;
        unitLastCreated = unit;
    }
    else
    {
        unitLastCreated = unit;
    }
    return;
}

void Graphics::updateBuildingLandFog(UnitNode* unit, int x, int y)
{
    // !!! TO DO !
    // ? (EAX, push, push)
    /*
    updateBuildingLandFog 00469F60 00000076
    00469F60 / $ 55             PUSH EBP
    00469F61 | . 8BEC           MOV EBP, ESP
    00469F63 | . 0FB748 64      MOVZX ECX, WORD PTR DS : [EAX + 64]
    00469F67 | .C1E1 02        SHL ECX, 2
    00469F6A | . 0FBF81 6028660>MOVSX EAX, WORD PTR DS : [ECX + 662860]
    00469F71 | . 99             CDQ
    00469F72 | . 83E2 1F        AND EDX, 1F
    00469F75 | . 03C2           ADD EAX, EDX
    00469F77 | . 56             PUSH ESI
    00469F78 | . 57             PUSH EDI
    00469F79 | . 8BF8           MOV EDI, EAX
    00469F7B | . 0FBF81 6228660>MOVSX EAX, WORD PTR DS : [ECX + 662862]
    00469F82 | . 99             CDQ
    00469F83 | . 83E2 1F        AND EDX, 1F
    00469F86 | . 03C2           ADD EAX, EDX
    00469F88 | . 8BC8           MOV ECX, EAX
    00469F8A | . 0FBF45 0C      MOVSX EAX, WORD PTR SS : [EBP + C]
    00469F8E | . 99             CDQ
    00469F8F | . 83E2 1F        AND EDX, 1F
    00469F92 | . 03C2           ADD EAX, EDX
    00469F94 | .C1F8 05        SAR EAX, 5
    00469F97 | . 0FBFF0         MOVSX ESI, AX
    00469F9A | .C1F9 05        SAR ECX, 5
    00469F9D | . 8BC1           MOV EAX, ECX
    00469F9F | . 99             CDQ
    00469FA0 | . 2BC2           SUB EAX, EDX
    00469FA2 | .D1F8           SAR EAX, 1
    00469FA4 | . 2BF0           SUB ESI, EAX
    00469FA6 | . 0FBF45 08      MOVSX EAX, WORD PTR SS : [EBP + 8]
    00469FAA | . 99             CDQ
    00469FAB | . 83E2 1F        AND EDX, 1F
    00469FAE | . 03C2           ADD EAX, EDX
    00469FB0 | .C1F8 05        SAR EAX, 5
    00469FB3 | . 56             PUSH ESI; / Arg2
    00469FB4 | . 0FBFF0         MOVSX ESI, AX; |
    00469FB7 | .C1FF 05        SAR EDI, 5; |
    00469FBA | . 8BC7           MOV EAX, EDI; |
    00469FBC | . 99             CDQ; |
    00469FBD | . 2BC2           SUB EAX, EDX; |
    00469FBF | .D1F8           SAR EAX, 1; |
    00469FC1 | . 2BF0           SUB ESI, EAX; |
    00469FC3 | . 56             PUSH ESI; | Arg1
    00469FC4 | .BE 00000008    MOV ESI, 8000000; |
    00469FC9 | . 8BD1           MOV EDX, ECX; |
    00469FCB | .E8 E08E0000    CALL StarCraf.00472EB0; \AddFogMask
    00469FD0 | . 5F             POP EDI
    00469FD1 | . 5E             POP ESI
    00469FD2 | . 5D             POP EBP
    00469FD3  \.C2 0800        RETN 8
    */
}

void Graphics::refreshAllVisibleImagesAtScreenPosition(SpriteNode* sprite, u8 newFlags)
{
    if ((sprite->visibilityFlags & visionFilter) == (newFlags & visionFilter))
    {
        sprite->visibilityFlags = newFlags;
        return;
    }
    if ((newFlags & visionFilter) == 0)
    {
        for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
        {
            if (!(unk_6CEFB5 & 1))
            {
                if (image->grpBounds.bottom > 0)
                {
                    if (image->grpBounds.right > 0)
                    {
                        rect coords = { image->screenPosition.x,
                                        image->screenPosition.y,
                                        image->grpBounds.right + image->screenPosition.x,
                                        image->grpBounds.bottom + image->screenPosition.y };
                        refreshImageData(&coords);
                    }
                }
            }
        }
        sprite->visibilityFlags = newFlags;
        return;
    }
    if (sprite->pImageHead == NULL)
    {
        sprite->visibilityFlags = newFlags;
        return;
    }
    for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
    {
        image->flags |= 1; // Redraw;
    }
    sprite->visibilityFlags = newFlags;
    return;
}

void Graphics::refreshUnitVision(UnitNode* unit)
{
    // !!! To Do !
    /*
    004E5F30 / $ 55             PUSH EBP
    004E5F31 | . 8BEC           MOV EBP, ESP
    004E5F33 | . 53             PUSH EBX
    004E5F34 | . 57             PUSH EDI
    004E5F35 | . 8B7D 08        MOV EDI, DWORD PTR SS : [EBP + 8]
    004E5F38 | . 8A5F 4C        MOV BL, BYTE PTR DS : [EDI + 4C]
    004E5F3B | . 80FB 08        CMP BL, 8
    004E5F3E | . 72 0E          JB SHORT StarCraf.004E5F4E
    004E5F40 | . 8A87 21010000  MOV AL, BYTE PTR DS : [EDI + 121]
    004E5F46 | . 84C0           TEST AL, AL
    004E5F48 | . 0F84 BE000000  JE StarCraf.004E600C
    004E5F4E | > 56             PUSH ESI
    004E5F4F | . 66:8B77 64     MOV SI, WORD PTR DS : [EDI + 64]
    004E5F53 | . 66 : 83FE 0E     CMP SI, 0E
    004E5F57 | . 0F84 AE000000  JE StarCraf.004E600B
    004E5F5D | . 8BC7           MOV EAX, EDI
    004E5F5F | .E8 1CDB0000    CALL StarCraf.004F3A80; unknown_004F3A80
    004E5F64 | . 85C0           TEST EAX, EAX
    004E5F66 | . 75 71          JNZ SHORT StarCraf.004E5FD9
    004E5F68 | . 66:81FE D700   CMP SI, 0D7
    004E5F6D | . 75 06          JNZ SHORT StarCraf.004E5F75
    004E5F6F | . 807F 4D 11     CMP BYTE PTR DS : [EDI + 4D], 11
    004E5F73 | . 74 64          JE SHORT StarCraf.004E5FD9
    004E5F75 | > 80FB 08        CMP BL, 8
    004E5F78 | . 0FB697 2101000>MOVZX EDX, BYTE PTR DS : [EDI + 121]
    004E5F7F | . 8BF2           MOV ESI, EDX
    004E5F81 | . 73 0A          JNB SHORT StarCraf.004E5F8D
    004E5F83 | . 0FB6C3         MOVZX EAX, BL
    004E5F86 | . 0B3485 ECF1570>OR ESI, DWORD PTR DS : [EAX * 4 + 57F1EC]
    004E5F8D | > B8 01000000    MOV EAX, 1
    004E5F92 | .B9 F0F15700    MOV ECX, StarCraf.0057F1F0
    004E5F97 | > 85D0 / TEST EAX, EDX
    004E5F99 | . 74 03 | JE SHORT StarCraf.004E5F9E
    004E5F9B | . 0B71 FC | OR ESI, DWORD PTR DS : [ECX - 4]
    004E5F9E | > D1E0 | SHL EAX, 1
    004E5FA0 | . 85D0 | TEST EAX, EDX
    004E5FA2 | . 74 02 | JE SHORT StarCraf.004E5FA6
    004E5FA4 | . 0B31 | OR ESI, DWORD PTR DS : [ECX]
    004E5FA6 | > D1E0 | SHL EAX, 1
    004E5FA8 | . 85D0 | TEST EAX, EDX
    004E5FAA | . 74 03 | JE SHORT StarCraf.004E5FAF
    004E5FAC | . 0B71 04 | OR ESI, DWORD PTR DS : [ECX + 4]
    004E5FAF | > D1E0 | SHL EAX, 1
    004E5FB1 | . 85D0 | TEST EAX, EDX
    004E5FB3 | . 74 03 | JE SHORT StarCraf.004E5FB8
    004E5FB5 | . 0B71 08 | OR ESI, DWORD PTR DS : [ECX + 8]
    004E5FB8 | > D1E0 | SHL EAX, 1
    004E5FBA | . 85D0 | TEST EAX, EDX
    004E5FBC | . 74 03 | JE SHORT StarCraf.004E5FC1
    004E5FBE | . 0B71 0C | OR ESI, DWORD PTR DS : [ECX + C]
    004E5FC1 | > D1E0 | SHL EAX, 1
    004E5FC3 | . 85D0 | TEST EAX, EDX
    004E5FC5 | . 74 03 | JE SHORT StarCraf.004E5FCA
    004E5FC7 | . 0B71 10 | OR ESI, DWORD PTR DS : [ECX + 10]
    004E5FCA | > 83C1 18 | ADD ECX, 18
    004E5FCD | .D1E0 | SHL EAX, 1
    004E5FCF | . 81F9 20F25700 | CMP ECX, StarCraf.0057F220
    004E5FD5 | . ^ 7C C0          \JL SHORT StarCraf.004E5F97
    004E5FD7 | .EB 05          JMP SHORT StarCraf.004E5FDE
    004E5FD9 | > BE FF000000    MOV ESI, 0FF
    004E5FDE | > 8B5D 08        MOV EBX, DWORD PTR SS : [EBP + 8]
    004E5FE1 | . 8B7F 0C        MOV EDI, DWORD PTR DS : [EDI + C]
    004E5FE4 | . 6A 00          PUSH 0; / Arg1 = 00000000
    004E5FE6 | . 8BD3           MOV EDX, EBX; |
    004E5FE8 | .E8 53FBFFFF    CALL StarCraf.004E5B40; \getUpdatedSightRange
    004E5FED | . 8B8B DC000000  MOV ECX, DWORD PTR DS : [EBX + DC]
    004E5FF3 | . 0FBF57 16      MOVSX EDX, WORD PTR DS : [EDI + 16]
    004E5FF7 | .C1E9 02        SHR ECX, 2
    004E5FFA | . 83E1 01        AND ECX, 1
    004E5FFD | . 51             PUSH ECX; / Arg3
    004E5FFE | . 0FBF4F 14      MOVSX ECX, WORD PTR DS : [EDI + 14]; |
    004E6002 | . 52             PUSH EDX; | Arg2
    004E6003 | . 51             PUSH ECX; | Arg1
    004E6004 | . 8BCE           MOV ECX, ESI; |
    004E6006 | .E8 E5A6F9FF    CALL StarCraf.004806F0; \revealSightAtLocation
    004E600B | > 5E             POP ESI
    004E600C | > 5F             POP EDI
    004E600D | . 5B             POP EBX
    004E600E | . 5D             POP EBP
    004E600F  \.C2 0400        RETN 4
    */
}

// initFlingySprite ?
bool Graphics::unknown_00496360(int flingyid, s16 x, s16 y, UnitNode* flingy, int player, u8 direction)
{
    FLINGYDAT* flingydat = chkd.scData.FlingyDat(flingyid);
    flingy->flingyID = flingyid;
    flingy->movementFlags = 0;
    flingy->current_speed2 = 0;
    flingy->flingyTopSpeed = flingydat->TopSpeed;
    flingy->flingyAcceleration = flingydat->Acceleration;
    flingy->flingyTurnRadius = flingydat->TurnRadius;
    flingy->flingyMovementType = flingydat->MoveControl;
    flingy->position.x = x;
    flingy->halt.x = x << 8;
    flingy->position.y = y;
    flingy->halt.y = y << 8;
    if (x != flingy->moveTarget.x || y != flingy->moveTarget.y)
    {
        flingy->moveTarget.x = x;
        flingy->moveTarget.y = y;
        flingy->moveTargetUnit = NULL;
        flingy->movementFlags = 1;
        flingy->nextMovementWaypoint = flingy->moveTarget;
    }
    if (x != flingy->nextTargetWaypoint.x || y != flingy->nextTargetWaypoint.y)
    {
        flingy->nextTargetWaypoint.x = x;
        flingy->nextTargetWaypoint.y = y;
    }
    flingy->currentDirection1 = direction;
    flingy->velocityDirection1 = direction;
    flingy->hitPoints = 1;
    flingy->sprite = createSprite(chkd.scData.FlingyDat(flingyid)->Sprite, x, y, player);
    if (flingy->sprite == NULL) {
        return false;
    }
    for (ImageNode* img = flingy->sprite->pImageHead; img != NULL; img = img->next)
    {
        setImageDirection(img, flingy->currentDirection1);
    }
    return true;
}

void Graphics::spriteToIscriptLoop(SpriteNode* sprite)
{
    ImageNode* image = sprite->pImageHead;
    ImageNode* nextimg = NULL;
    while (image != NULL)
    {
        nextimg = image->next;
        (this->*image->updateFunction)(image);
        iscript_OpcodeCases(image, image->iscriptHeader, 0, 0);
        /* // Add sprite to draw sort list
        if (sprite->pImageHead == NULL)
        {
            int ytile = sprite->position.y / 32;
            if (ytile < 0)
            {
                ytile = 0;
            }
            else {
                if (ytile >= mapHeight)
                {
                    ytile = mapHeight - 1;
                }
            }
            if (unk_sprite_629688[ytile] == sprite)
            {
                unk_sprite_629688[ytile] = sprite->next;
            }
            if (unk_sprite_629288[ytile] == sprite)
            {
                unk_sprite_629288[ytile] = sprite->prev;
            }
            if (sprite->prev != NULL)
            {
                sprite->prev->next = sprite->next;
            }
            if (sprite->next != NULL)
            {
                sprite->next->prev = sprite->prev;
            }
            sprite->prev = NULL;
            sprite->next = NULL;
            if (unk_sprite_63FE30 != NULL)
            {
                if (unk_sprite_63FE34 == unk_sprite_63FE30)
                {
                    unk_sprite_63FE34 = sprite;
                }
                sprite->prev = unk_sprite_63FE30;
                sprite->next = unk_sprite_63FE30->next;
                if (unk_sprite_63FE30->next != NULL)
                {
                    unk_sprite_63FE30->next->prev = sprite;
                }
                unk_sprite_63FE30->next = sprite;
            }
            else
            {
                unk_sprite_63FE34 = sprite;
                unk_sprite_63FE30 = sprite;
            }
            
        }
        */
        image = nextimg;
    }
    return;
}

void Graphics::ReplaceSpriteOverlayImage(SpriteNode* sprite, int imageid, int direction)
{
    u8 EAX = (sprite->flags >> 1) & 3; // unk_04 | unk_02
    u8 EBX = (sprite->flags >> 3) & 1; // Selected
    u8 ECX = sprite->flags & 1; // Draw selection circle
    u8 EBP10 = sprite->flags & 1; // Draw selection circle
    u8 EBP14 = (sprite->flags >> 1) & 3; // unk_04 | unk_02
    if (sprite->flags & 0x08) { // Selected
        removeSelectionCircleAndHPBar(sprite /*?*/ ); // ?
    }
    else {
        removeSelectionCircle(sprite/*, ?*/ ); // (ECX) ?
    }
    //EAX = ?
    //[EBP - 4] = AL
    u8 EBP8;
    if (!(sprite->flags & 6)) // unk_04 | unk_02
    {
        EBP8 = 0;
    }
    else
    {
        sprite->flags &= ~6;
        EBP8 = removeSelectionCircleFromSprite(sprite); // (EAX, ? )
    }
    ImageNode* tmp = sprite->pImageHead;
    ImageNode* tmpnext;
    while (tmp != NULL) {
        tmpnext = tmp->next;
        ImageDestructor(tmp);
        tmp = tmpnext;
    }
    CreateImageOverlay(sprite, imageid, 0, 0, 0);
    if (EBP10) // Draw selection circle
    {
        if (EBX) // Selected
        {
            if (!(sprite->flags & 8)) // Selected
            {
                sprite->flags |= 8;
                //CreateHealthBar(sprite, ? ); // (EDX, ?)
                //if (!(sprite->flags & 1)) { // Draw selection circle
                //  if (CreateSelectionCircle(sprite, [EBP - 4], 0x231)) { // (EAX, push, push) - 561 - Selection Circle (22px)
                //      sprite->flags |= 1; // Draw selection circle
                //  }
                //}
            }
        }
        else
        {
            //if (!(sprite->flags & 1)) { // Draw selection circle
            //  if (CreateSelectionCircle(sprite, [EBP - 4], 0x231)) {
            //      sprite->flags |= 1;
            //  }
            //}
        }
    }
    if (EBP14) // unk_04 | unk_02
    {
        //ESI = [EBP - 8]
        imageid = EBP14;
        //do {
            //BL = sprite->flags
            //illegalTeamCheck(? )
            //AL = ?
            //BL = ? (sprite->flags ? )
            //ECX = (BL >> 1) & 3
            //EAX = AL
            //EAX--
            //if (EAX != ECX) {
            //  AL = ((((BL & ~1) + 2) ^ BL) & 6) ^ BL
            //  sprite->flags = ((((BL & ~1) + 2) ^ BL) & 6) ^ BL;
            //  AL = sprite->flags & 6
            //  if ((sprite->flags & 6) <= 2) {
            //      if (!CreateSelectionCircle(sprite, [EBP - 8], 0x23B)) { // 571 - Selection Circle Dashed (22px)
            //          sprite->flags &= ~6;
            //      }
            //  }
            //}
            //[EBP - 8]--;
        //} while ([EBP - 8] != 0);
    }
    tmp = sprite->pImageHead;
    while (tmp != NULL) {
        setImageDirection(tmp, direction);
        tmp = tmp->next;
    }
    return;
}

void Graphics::unknown_00497FD0(SpriteNode* sprite, u8 unk1, u8 cloaked, int EBP5)
{
    refreshAllVisibleSprites(sprite);

    if (cloaked == 3)
    {
        for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
        {
            if (image->paletteType == DRAW_NORMAL ||
                image->paletteType == DRAW_ENEMY_CLOAK ||
                image->paletteType == DRAW_ALLY_CLOAK)
            {
                image->paletteType = DRAW_OWN_CLOAK;
                image->updateFunction = imgUpdateFxns[DRAW_OWN_CLOAK].func; // 51252C
                if (image->flags & 2) // flipped
                {
                    image->renderFunction = imgRenderFxns[DRAW_OWN_CLOAK].flip; // 5125CC
                }
                else {
                    image->renderFunction = imgRenderFxns[DRAW_OWN_CLOAK].norm; // 5125C8
                }
                image->flags |= 1; // Redraw
            }
        }
    }
    else if (cloaked == 2)
    {
        for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
        {
            if (image->paletteType == DRAW_NORMAL)
            {
                image->paletteType = DRAW_ALLY_CLOAK;
                image->updateFunction = imgUpdateFxns[DRAW_ALLY_CLOAK].func; // 512534
                if (image->flags & 2) // Flipped
                {
                    image->renderFunction = imgRenderFxns[DRAW_ALLY_CLOAK].flip; // 5125D8
                }
                else {
                    image->renderFunction = imgRenderFxns[DRAW_ALLY_CLOAK].norm; // 5125D4
                }
                image->flags |= 1; // Redraw
                image->coloringData.cloak.state = EBP5;
                image->coloringData.cloak.timer = unk1;
            }
        }
    }
    else if (cloaked == 1)
    {
        for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
        {
            if (image->paletteType == DRAW_NORMAL)
            {
                image->paletteType = DRAW_ENEMY_CLOAK;
                image->updateFunction = imgUpdateFxns[DRAW_ENEMY_CLOAK].func; // 512524
                if (image->flags & 2) // Flipped
                {
                    image->renderFunction = imgRenderFxns[DRAW_ENEMY_CLOAK].flip; // 5125C0
                }
                else {
                    image->renderFunction = imgRenderFxns[DRAW_ENEMY_CLOAK].norm;
                }
                image->flags |= 1; // Redraw
                image->coloringData.cloak.state = EBP5;
                image->coloringData.cloak.timer = unk1;
            }
        }
    }
    return;
}

void Graphics::refreshAllVisibleSprites(SpriteNode* sprite)
{
    for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
    {
        if (chkd.scData.ImageDat(image->imageID)->DrawIfCloaked == 0 &&
            image->flags & 0x40) // Hidden
        {
            if (//*(u8*)6CEFB5 == 1 && // What is it?
                image->grpBounds.bottom > 0 &&
                image->grpBounds.right > 0)
            {

                rect unk = {
                    image->screenPosition.x,
                    image->screenPosition.y,
                    image->grpBounds.right + image->screenPosition.x,
                    image->grpBounds.bottom + image->screenPosition.y
                };
                refreshImageData(&unk);
            }
            image->flags |= 0x40;
        }
    }
}

void Graphics::iscript_OpcodeCases(ImageNode *image, u16 hdr, int unk_arg2, int unk_arg3) {
    if (image->sleep != 0) {
        image->sleep--;
        return;
    }

    int opcode;
    u16 isptr = image->iscriptOffset;
    point goffs;
    
    while (1) {
        opcode = is_get(u8, isptr);
        isptr++;
        switch (opcode) {
            default:
                break; // Do nothing !

            case 0x00: // PLAYFRAM
                isptr += 2;
                if (unk_arg2) break;
                ISCRIPT_PlayFrame(image, is_get(u16, isptr-2));
                break;

            case 0x40: // WARPOVERLAY
                isptr += 2;
                if (unk_arg2) break;
                image->flags |= 1; // Redraw
                image->coloringData.warpFlash.state = is_get(u8, isptr - 2);
                image->coloringData.warpFlash.timer = is_get(u8, isptr - 1);
                break;

            case 0x01: // PLAYFRAMTILE
            {
                isptr += 2;
                if (unk_arg2) break;
                u16 frame = is_get(u16, isptr - 2) + map.getTileset();
                if (frame >= (image->GRPFile->numFrames() & 0x7FFF)) break;
                ISCRIPT_PlayFrame(image, frame);
                break;
            }

            case 0x02: // SETHORPOS
                isptr++;
                if (unk_arg2) break;
                if (image->horizontalOffset == is_get(s8, isptr-1)) break;
                image->flags |= 1; // Redraw
                image->horizontalOffset = is_get(s8, isptr - 1);
                break;

            case 0x03: // SETVERTPOS
                isptr++;
                if (unk_arg2) break;
                if (activeIscriptUnit)
                {
                    if (activeIscriptUnit->statusFlags & (USTATUS_REQUIRESDETECTION | USTATUS_CLOAKED)) break;
                }
                if (image->verticalOffset == is_get(s8, isptr - 1)) break;
                image->flags |= 1; // Redraw
                image->verticalOffset = is_get(s8, isptr - 1);
                break;

            case 0x04: // SETPOS
                isptr += 2;
                if (unk_arg2) break;
                ISCRIPT_setPosition(image, is_get(s8, isptr - 2), is_get(s8, isptr - 1)); // (EAX, DL, CL)
                break;

            case 0x06: // WAITRAND
                isptr += 2;
                if (unk_arg2) break;
                image->iscriptOffset = isptr;
                image->sleep = ((getRand() & 0xFF) % (is_get(u8, isptr - 1) - is_get(u8, isptr - 2) + 1)) + is_get(u8, isptr - 2) - 1;
                return;

            case 0x07: // GOTO
                isptr = is_get(u16, isptr); // s16?
                break;

            case 0x08: // IMGOL
                isptr += 4;
                if (unk_arg2) break;
                ISCRIPT_CreateImage(image, is_get(u16, isptr - 4), image->horizontalOffset + is_get(s8, isptr - 2), image->verticalOffset + is_get(s8, isptr - 1), 1);
                break;

            case 0x09: // IMGUL
                isptr += 4;
                if (unk_arg2) break;
                ISCRIPT_CreateImage(image, is_get(u16, isptr - 4), image->horizontalOffset + is_get(s8, isptr - 2), image->verticalOffset + is_get(s8, isptr - 1), 0);
                break;

            case 0x3D: // IMGULNEXTID
                isptr += 2;
                if (unk_arg2) break;
                ISCRIPT_CreateImage(image, image->imageID + 1, image->horizontalOffset + is_get(s8, isptr - 2), image->verticalOffset + is_get(s8, isptr - 1), 0);
                break;

            case 0x0A: // IMGOLORIG
            {
                isptr += 2;
                if (unk_arg2) break;
                ImageNode* ret = ISCRIPT_CreateImage(image, is_get(u16, isptr - 2), 0, 0, 1);
                if (ret == NULL || (ret->flags & 0x80))
                {
                    break;
                }
                ret->flags |= 0x80;
                updateImagePositionOffset(ret);
                break;
            }

            case 0x0B: // SWITCHUL
            {
                isptr += 2;
                if (unk_arg2) break;
                ImageNode* ret = ISCRIPT_CreateImage(image, is_get(u16, isptr - 2), 0, 0, 0);
                if (ret == NULL || (ret->flags & 0x80))
                {
                    break;
                }
                ret->flags |= 0x80;
                updateImagePositionOffset(ret);
                break;
            }

            case 0x0D: // IMGOLUSELO
                isptr += 4;
                if (unk_arg2) break;
                ISCRIPT_UseLOFile(image, is_get(u8, isptr - 2), is_get(u8, isptr - 1), &goffs);
                ISCRIPT_CreateImage(image, is_get(u16, isptr - 4), image->horizontalOffset + goffs.x, image->verticalOffset + goffs.y, 1);
                break;

            case 0x0E: // IMGULUSELO
                isptr += 4;
                if (unk_arg2) break;
                ISCRIPT_UseLOFile(image, is_get(u8, isptr - 2), is_get(u8, isptr - 1), &goffs);
                ISCRIPT_CreateImage(image, is_get(u16, isptr - 4), image->horizontalOffset + goffs.x, image->verticalOffset + goffs.y, 0);
                break;

            case 0x42: // GRDSPROL
                isptr += 4;
                if (unk_arg2) break;
                if (!canUnitTypeFitAt(0, // EBX - Unit ID
                    /* EAX - X */ image->spriteOwner->position.x + image->horizontalOffset + is_get(s8, isptr - 2),
                    /* EDI - Y */ image->spriteOwner->position.y + image->verticalOffset + is_get(s8, isptr - 1)))
                {
                    break;
                }
                ISCRIPT_CreateSprite(image, is_get(u16, isptr - 4), is_get(s8, isptr - 2), is_get(s8, isptr - 1), image->spriteOwner->elevationLevel + 1);
                break;

            case 0xF: // SPROL
            {
                isptr += 4;
                if (unk_arg2) break;
                /*if (unk_unit_6D11F8 != NULL) // Is a bullet -- I don't have CBullet class
                {
                    if (unk_unit_6D11F8->sourceUnit != NULL) // Bullet has a parent
                    {
                        if (unitIsGoliath(unk_unit_6D11F8->sourceUnit)) // Parent is a goliath
                        {
                            u8 upgd = 0;
                            if ( (chkd.maps.curr->getUpgradePlayerStartLevel(54, unk_unit_6D11F8->sourceUnit->srcPlayer, &upgd) && upgd) || // Charon Boosters upgraded
                                 ((chkd.scData.UnitDat(unk_unit_6D11F8->sourceUnit->unitType)->SpecialAbilityFlags & 0x40) && chkd.maps.curr->isExpansion()) ) // Or Hero && Expansion
                            {
                                ISCript_CreateSprite(image, 0x1F9, is_get(s8, isptr - 2), is_get(s8, isptr - 1), image->spriteOwner->elevationLevel + 1); // Charon Booster overlay
                                break;
                            }
                        }
                    }
                }*/
                ISCRIPT_CreateSprite(image, is_get(u16, isptr - 4), is_get(s8, isptr - 2), is_get(s8, isptr - 1), image->spriteOwner->elevationLevel + 1);
                break;
            }

            case 0x12: // UFLUNSTABLE
                isptr += 2;
                if (unk_arg2) break;
                //CFlingy* ret // CBullet?
                //  = ISCRIPT_CreateFlingy(image->spriteOwner->position.x + image->horizontalOffset, image->spriteOwner->position.y + image->verticalOffset, 0, is_get(u16, isptr-2)); // (ECX, EDX, EAX, push)
                //if (ret == NULL) {
                //  break;
                //}
                //uflunstableRandomize(ret);
                //unknown_004878F0(ret);
                break;

            case 0x15: // SPROLUSELO
            {
                isptr += 3;
                if (unk_arg2) break;
                ISCRIPT_UseLOFile(image, is_get(u8, isptr - 1), 0, &goffs);
                SpriteNode* ret = ISCRIPT_CreateSprite(image, is_get(u16, isptr - 3), goffs.x, goffs.y, image->spriteOwner->elevationLevel + 1); // CThingy?
                if (ret == NULL)
                {
                    break;
                }
                if (image->flags & 2) {
                    setAllOverlayDirectionsGeneric(ret, 0x20 - image->direction);
                }
                else {
                    setAllOverlayDirectionsGeneric(ret, image->direction);
                }
                break;
            }

            case 0x10: // HIGHSPROL
                isptr += 4;
                if (unk_arg2) break;
                ISCRIPT_CreateSprite(image, is_get(u16, isptr - 4), is_get(s8, isptr - 2), is_get(s8, isptr - 1), image->spriteOwner->elevationLevel - 1);
                break;

            case 0x11: // LOWSPRUL
                isptr += 4;
                if (unk_arg2) break;
                ISCRIPT_CreateSprite(image, is_get(u16, isptr - 4), is_get(s8, isptr - 2), is_get(s8, isptr - 1), 1);
                break;

            case 0x13: // SPRULUSELO
            {
                isptr += 4;
                if (unk_arg2) break;
                if (activeIscriptUnit != NULL)
                {
                    if (activeIscriptUnit->statusFlags & 3) // (Requires Detection | Cloaked)
                    {
                        if (chkd.scData.ImageDat(chkd.scData.SpriteDat(is_get(u16, isptr - 4))->ImageFile)->DrawIfCloaked == 0) break;
                    }
                }
                SpriteNode* ret = ISCRIPT_CreateSprite(image, is_get(u16, isptr - 4), is_get(s8, isptr - 2), is_get(s8, isptr - 1), image->spriteOwner->elevationLevel); // CThingy ?
                if (ret == NULL)
                {
                    break;
                }
                if (image->flags & 2) // flipped
                {
                    setAllOverlayDirectionsGeneric(ret, 0x20 - image->direction);
                }
                else
                {
                    setAllOverlayDirectionsGeneric(ret, image->direction);
                }
                break;
            }

            case 0x14: // SPRUL
            {
                isptr += 4;
                if (unk_arg2) break;
                if (activeIscriptUnit != NULL && (activeIscriptUnit->statusFlags & 3)) // (RequiresDetection | Cloaked)
                {
                    if (chkd.scData.ImageDat(chkd.scData.SpriteDat(is_get(u16, isptr - 4))->ImageFile)->DrawIfCloaked == 0) break;
                }
                SpriteNode* ret = ISCRIPT_CreateSprite(image, is_get(u16, isptr - 4), is_get(s8, isptr - 2), is_get(s8, isptr - 1), image->spriteOwner->elevationLevel - 1); // CThingy ?
                if (ret == NULL)
                {
                    break;
                }
                if (image->flags & 2) // flipped
                {
                    setAllOverlayDirectionsGeneric(ret, 0x20 - image->direction);
                }
                else
                {
                    setAllOverlayDirectionsGeneric(ret, image->direction);
                }
                break;
            }

            case 0x38: // CREATEGASOVERLAYS
            {
                isptr++;
                if (unk_arg2) break;
                ISCRIPT_UseLOFile(image, 2, is_get(s8, isptr - 1), &goffs); // 2 = "Special" Overlay
                ImageNode* newImage = unknown_004D4E30(); // GetNextUnusedImage ( ? )
                if (newImage == NULL)
                {
                    break;
                }
                image_Insert(image, newImage, &image->spriteOwner->pImageHead); // (ECX, EAX, EDX)
                // SI = -activeIscriptUnit->resource.resourceCount // unit + 0xD0
                // other unit + 0xD0 values:
                //   resource -> resourceCount
                //   nydus -> CUnit* exit
                //   ghost -> CSprite* nukeDot
                //   Pylon -> CSprite* pPowerTemplate
                //   silo -> CUnit* pNuke
                //   hatchery -> rect harvestValue
                //   powerup -> POINTS origin
                //   gatherer -> CUnit* harvestTarget
                somePlayImageCrapThatCrashes(newImage, image->spriteOwner,
                    image->horizontalOffset + goffs.x,
                    image->verticalOffset + goffs.y,
                    (((activeIscriptUnit->resource.resourceCount != 0) - 1) & 0xFFFFFFFB) + 435 + is_get(s8, isptr - 1)); // 435 = Vespene Geyser Smoke1 Overlay, -5 if depleted
                break;
            }

            case 0x39: // PWRUPCONDJMP
                isptr += 2;
                if (image->spriteOwner == NULL) break;
                if (image->spriteOwner->pImagePrimary == image) break;
                isptr = is_get(u16, isptr - 2);
                break;

            case 0x16: // END
                if (unk_arg2) break;
                ImageDestructor(image); // EBP-C -- ImageDestructor(ESI)
                image->iscriptOffset = isptr;
                return;

            case 0x17: // SETFLIPSTATE
                isptr++;
                if (unk_arg2) break;
                flipImage(image, is_get(u8, isptr - 1));
                break;

            case 0x18: // PLAYSND
                isptr += 2;
                if (unk_arg2) break;
                ISCRIPT_PlaySnd(image, is_get(u16, isptr - 2));
                break;

            case 0x1C: // ATTACKMELEE
                if (!unk_arg2)
                {
                    ISCRIPT_AttackMelee(activeIscriptUnit);
                }
            case 0x19: // PLAYSNDRAND
            {
                isptr++;
                if (unk_arg2)
                {
                    isptr += is_get(u8, isptr - 1) * 2;
                    break;
                }
                u16 count = is_get(u8, isptr - 1);
                u16 result = getRand() % count;
                isptr += (result & 0xFF) * 2 + 2;
                ISCRIPT_PlaySnd(image, is_get(u16, isptr - 2)); // (EAX, EBX)
                isptr += ((count - result - 1) * 2);
                break;
            }

            case 0x1A: // PLAYSNDBTWN
                isptr += 4;
                if (unk_arg2) break;
                ISCRIPT_PlaySnd(image, getRand() % (is_get(u16, isptr - 2) - is_get(u16, isptr - 4) + 1) + is_get(u16, isptr - 4));
                break;

            case 0x1D: // FOLLOWMAINGRAPHIC
                if (unk_arg2) break;
                if (image->spriteOwner->pImagePrimary == NULL) break;
                if (image->frameIndex == image->spriteOwner->pImagePrimary->frameIndex)
                {
                    if (!((image->spriteOwner->pImagePrimary->flags ^ image->flags) & 0x02)) break; // Flipped/Mirrored
                }
                image->frameSet = image->spriteOwner->pImagePrimary->frameSet;
                image->direction = image->spriteOwner->pImagePrimary->direction;
                image->flags ^= (image->spriteOwner->pImagePrimary->flags ^ image->flags) & 2;
                setImagePaletteType(image, image->paletteType);
                updateImageFrameIndex(image);
                break;

            case 0x2B: // ENGFRAME
                isptr++;
                if (unk_arg2) break;
                image->frameSet = is_get(u8, isptr - 1);
                image->direction = image->spriteOwner->pImagePrimary->direction;
                image->flags ^= (image->spriteOwner->pImagePrimary->flags ^ image->flags) & 2;
                setImagePaletteType(image, image->paletteType);
                updateImageFrameIndex(image); //
                break;

            case 0x2C: // ENGSET
                isptr++;
                if (unk_arg2) break;
                image->direction = image->spriteOwner->pImagePrimary->direction;
                image->frameSet = (image->spriteOwner->pImagePrimary->GRPFile->numFrames() & 0x7FFF) * is_get(u8, isptr - 1) + image->spriteOwner->pImagePrimary->frameSet;
                image->flags ^= (image->spriteOwner->pImagePrimary->flags ^ image->flags) & 2; // Flipped
                setImagePaletteType(image, image->paletteType);
                updateImageFrameIndex(image);
                break;

            case 0x2D: // __2D
                if (unk_arg2) break;
                //unk_652920 = 0; // Hides target marker
                break;

            case 0x33: // TMPRMGRAPHICEND
                if (unk_arg2) break;
                showImage(image); // (ECX)
                break;

            case 0x32: // TMPRMGRAPHICSTART
                if (unk_arg2) break;
                hideImage(image); // (ESI)
                break;

            case 0x24: // SIGORDER
                isptr++;
                if (unk_arg2) break;
                unk_unit_6D11F4->orderSignal |= is_get(u8, isptr - 1);
                break;

            case 0x41: // ORDERDONE
                isptr++;
                if (unk_arg2) break;
                unk_unit_6D11F4->orderSignal &= ~(is_get(u8, isptr - 1));
                break;

            case 0x1B: // DOMISSILEDMG
                if (unk_arg2) break;
                //CBullet_Damage(unk_unit_6D11F8); // ?
                break;

            case 0x44: // DOGRDDAMAGE
                if (unk_arg2) break;
                //CBullet_Damage(unk_unit_6D11F8); // ?
                break;

            case 0x1F: // TURNCCWISE
                isptr++;
                if (unk_arg2) break;
                _turnGraphic(activeIscriptUnit, is_get(u8, isptr - 1));
                break;

            case 0x20: // TURNCWISE
                isptr++;
                if (unk_arg2) break;
                turnUnit(activeIscriptUnit, activeIscriptUnit->currentDirection1 + is_get(u8, isptr - 1) * 8);
                break;

            case 0x21: // TURN1CWISE
                if (unk_arg2) break;
                if (activeIscriptUnit->orderTargetUnit != NULL) break;
                turnUnit(activeIscriptUnit, activeIscriptUnit->currentDirection1 + 8);
                break;

            case 0x22: // TURNRAND
                isptr++;
                if (unk_arg2) break;
                if ((getRand() & 3) == 1)
                {
                    _turnGraphic(activeIscriptUnit, is_get(u8, isptr - 1)); // (EAX, CL)
                }
                else
                {
                    turnUnit(activeIscriptUnit, activeIscriptUnit->currentDirection1 + is_get(u8, isptr - 1) * 8); // (EAX, EBX)
                }
                break;

            case 0x23: // SETSPAWNFRAME
                isptr++;
                if (unk_arg2) break;
                unitSetRetreatPoint(activeIscriptUnit, is_get(u8, isptr - 1));
                break;

            case 0x1E: // RANDCONDJMP
                isptr += 3;
                if ((getRand() & 0xFF) > is_get(u8, isptr - 3)) break;
                isptr = is_get(u16, isptr - 2);
                break;

            case 0x3A: // TRGTRANGECONDJMP
                isptr += 4;
                if (unk_arg2) break;
                if (activeIscriptUnit->orderTargetUnit == NULL) break;
                unknown_004762C0(activeIscriptUnit, &goffs.x, &goffs.y); // (unit, outx, outy)
                if (isDistanceGreaterThanHaltDistance(activeIscriptUnit, is_get(u16, isptr - 4), goffs.x, goffs.y))
                {
                    isptr = is_get(u16, isptr - 2);
                }
                break;

            case 0x3B: // TRGTARCCONDJMP
                isptr += 6;
                if (unk_arg2) break;
                if (activeIscriptUnit->orderTargetUnit == NULL) break;
                if (is_get(u16, isptr - 4) >= unknown_00494BD0(is_get(u16, isptr - 6),
                    getDirectionFromPoints(activeIscriptUnit->orderTargetUnit->sprite->position.x,
                        activeIscriptUnit->orderTargetUnit->sprite->position.y,
                        activeIscriptUnit->sprite->position.x,
                        activeIscriptUnit->sprite->position.y))) break;
                isptr = is_get(u16, isptr - 2);
                break;

            case 0x3C: // CURDIRECTCONDJMP
                isptr += 6;
                if (unk_arg2) break;
                if (is_get(u16, isptr - 4) >= unknown_00494BD0(is_get(u16, isptr - 6), activeIscriptUnit->currentDirection1)) break;
                isptr = is_get(u16, isptr - 2);
                break;

            case 0x25: // ATTACKWITH
                isptr++;
                if (unk_arg2) break;
                ISCRIPT_AttackWith(activeIscriptUnit, is_get(u8, isptr - 1)); // (EAX, ECX)
                break;

            case 0x26: // ATTACK
                if (unk_arg2) break;
                if (activeIscriptUnit->orderTargetUnit != NULL && (activeIscriptUnit->orderTargetUnit->statusFlags & 4)) // InAir
                {
                    // EDX = unitsdat.airweapon[activeIscriptUnit->unitType]
                    // PUSH unitsdat.airweapon[activeIscriptUnit->unitType]
                    // ESI = activeIscriptUnit
                    // EBX = activeIscriptUnit
                    //FireUnitWeapon(? , unitsdat.airweapon[activeIscriptUnit->unitType]);
                    // EAX = [EBP-10]
                    //if (weaponsdat.damageFactor[unitsdat.airweapon[activeIscriptUnit->unitType]] == 2) {
                        // ECX = [EBP-10]
                        //FireUnitWeapon(? , unitsdat.airweapon[activeIscriptUnit->unitType]);
                    //}
                    break;
                }
                ISCRIPT_AttackWith(activeIscriptUnit, 1);  // (EAX, ECX)
                break;

            case 0x27: // CASTSPELL
                if (unk_arg2) break;
                //if (ordersdat.targeting[activeIscriptUnit->orderID] >= 0x82) break; // "None" // CHKD doesn't have orders.dat xD
                // EDX = activeIscriptUnit
                // EAX = activeIscriptUnit->orderID
                // EDI = activeIscriptUnit
                if (canCastSpell_0(activeIscriptUnit)) // (EDI) ?
                {
                    break;
                }
                // EAX = EDI // = activeIscriptUnit ?
                // ECX = activeIscriptUnit->orderID
                // BL = ordersdat.targeting[activeIscriptUnit->orderID]
                ISCRIPT_CastSpell(activeIscriptUnit, 130 /* orders.dat->targeting[activeIscriptUnit->orderID] */); // No orders.dat :(
                break;

            case 0x28: // USEWEAPON
                isptr++;
                if (unk_arg2) break;
                ISCRIPT_UseWeapon(activeIscriptUnit, is_get(u8, isptr - 1)); // (ESI, EDX)
                break;

            case 0x31: // ATTKSHIFTPROJ
                isptr++;
                if (unk_arg2) break;
                //EAX ? = activeIscriptUnit->getGrndWeapon(); // Unit::getGrndWeapon(ECX)
                //weaponsdat.xOffset[EAX] = is_get(u8, isptr - 1); // Weapons.dat - Graphics Offset X
                ISCRIPT_AttackWith(activeIscriptUnit, 1); // (EAX, push) ?
                break;

            case 0x2A: // GOTOREPEATATTK
                if (unk_arg2) break;
                if (activeIscriptUnit == NULL) break;
                activeIscriptUnit->movementFlags &= 0xF7; // ~StartingAttack
                break;

            case 0x29: // MOVE
                //EAX = unk_arg3
                isptr++;
                if (unk_arg3)
                {
                    //EDX = activeIscriptUnit;
                    //EAX = is_get(u8, isptr - 1);
                    //EAX = GetModifiedUnitSpeed(? ); // (EDX, EAX)
                    //ECX = unk_arg3
                    //*ECX = EAX
                }
                if (unk_arg2) break;
                //EAX = is_get(u8, isptr - 1);
                //EBX = activeIscriptUnit;
                //EDX = EBX
                //EAX = GetModifiedUnitSpeed(/*?*/ )
                //EDX = EAX
                //EAX = EBX
                //SetUnitMovementSpeed(/*?*/ ) // (EAX, EDX)
                break;

            case 0x30: // IGNOREREST
                if (unk_arg2) break;
                if (activeIscriptUnit->orderTargetUnit != NULL)
                {
                    image->iscriptOffset = isptr - 1;
                    image->sleep = 10;
                    return;
                }
                IgnoreAllScriptAndGotoIdle(activeIscriptUnit);
                break;

            case 0x2E: // NOBRKCODESTART
                if (unk_arg2) break;
                activeIscriptUnit->statusFlags |= 0x2000; // NoBrkCodeStart
                activeIscriptUnit->sprite->flags |= 0x80; // Iscript unbreakable code section.
                break;

            case 0x2F: // NOBRKCODEEND
                if (unk_arg2) break;
                if (activeIscriptUnit == NULL) break;
                ISCRIPT_NoBrkCodeEnd(activeIscriptUnit);
                break;

            case 0x3F: // LIFTOFFCONDJMP
                isptr += 2;
                if (unk_arg2) break;
                if (!(activeIscriptUnit->statusFlags & 0x00000004)) break; // InAir
                isptr = is_get(u16, isptr - 2); // s16?
                break;

            case 0x34: // SETFLDIRECT
                isptr++;
                if (unk_arg2) break;
                turnUnit(activeIscriptUnit, is_get(u8, isptr - 1) << 3);
                break;

            case 0x35: // CALL
                isptr += 2;
                image->iscriptReturn = isptr;
                isptr = is_get(u16, isptr - 2);
                break;

            case 0x36: // RETURN
                isptr = image->iscriptReturn;
                break;

            case 0x37: // SETFLSPEED
                isptr += 2;
                if (unk_arg2) break;
                activeIscriptUnit->flingyTopSpeed = is_get(s16, isptr - 2);
                break;

            case 0x05: // WAIT
                image->sleep = is_get(u8, isptr) - 1;
                isptr++;
                image->iscriptOffset = isptr;
                return;
        }
    }
}

void Graphics::setImageDirection(ImageNode* image, int direction)
{
    int flipped, dcomp;
    if (image->flags & 0x80) // ? Update Image Position Offset
    {
        updateImagePositionOffset(image); // ??
    }
    if (!(image->flags & 8)) return; // If doesn't have rotation frames, no need to rotate
    dcomp = (direction + 4) >> 3;
    if (dcomp > 0x10)
    {
        dcomp = 0x20 - dcomp;
        flipped = 1;
    }
    else
    {
        flipped = 0;
    }
    if (image->direction == dcomp)
    {
        if (((image->flags >> 1) & 1) == flipped) return; // Flipped/Mirrored
    }
    image->direction = dcomp;
    image->flags &= ~2;
    image->flags |= (flipped & 1) << 1; // Flipped/Mirrored
    setImagePaletteType(image, image->paletteType);
    if (image->frameIndex != (image->frameSet + image->direction))
    {
        image->flags |= 1; // redraw
        image->frameIndex = image->frameSet + image->direction;
    }
    return;
}

void Graphics::refreshImageData(rect* rect)
{

    rect->left = (rect->left + ((rect->left < 0)*-1) & 0xF) / 16;
    rect->top = (rect->top + ((rect->top  < 0)*-1) & 0xF) / 16;
    rect->right = (rect->right + 0xF + (((rect->right + 0xF) < 0)*-1) & 0xF) / 16;
    rect->bottom = (rect->bottom + 0xF + (((rect->bottom + 0xF) < 0)*-1) & 0xF) / 16;

    // I don't know what all this does, but it has a potential to crash due to lots of pointer accessing & writing
    /*
    int a, i;
    char *dst;
    char *tmp;
    dst = &unk_6DEFF8 + rect->left + (rect->top * 5) * 8; // Dunno what 6DEFF8 is
    a = rect->right - rect->left + 1;
    while (rect->top <= rect->bottom)
    {
        rect->top += 1;
        tmp = dst;
        for (i = a >> 2; i != 0; i--)
        {
            *(int*)(tmp) = 0x01010101;
            tmp += 4;
        }
        for (i = a & 3; i != 0; i--)
        {
            *tmp = 0x01;
            tmp += 1;
        }
        dst += 28;
    }
    */
    rect->top += 1;
    unk_6CEFB5 |= 2;
    return;
}

SpriteNode* Graphics::createSprite(int id, int x, int y, char player)
{
    SpriteNode* tmp = spriteEmptyNode;
    if (tmp == NULL) // No empty nodes?
    {
        // Created one!
        spriteNodes.push_back(std::shared_ptr<SpriteNode>(new SpriteNode()));
        tmp = spriteNodes.back().get(); // Get new node
    }
    spriteEmptyNode = tmp->next;
    //if (unk_sprite_63FE34 == tmp) // Last empty node -- I don't have one.
    //{
    //  unk_sprite_63FE34 = tmp->prev;
    //}
    if (tmp->prev != NULL)
    {
        tmp->prev->next = tmp->next;
    }
    if (tmp->next != NULL)
    {
        tmp->next->prev = tmp->prev;
    }
    tmp->prev = NULL;
    tmp->next = NULL;
    if (!initSpriteData(tmp, id, x, y, player))
    {
        // Failed !
        if (spriteEmptyNode != NULL)
        {
            // Re-insert back in to list
            //if (unk_sprite_63FE34 == spriteEmptyNode) // I don't have one
            //{
            //  unk_sprite_63FE34 = tmp;
            //}
            tmp->prev = spriteEmptyNode;
            tmp->next = spriteEmptyNode->next;
            if (spriteEmptyNode->next != NULL)
            {
                spriteEmptyNode->next->prev = tmp;
            }
            spriteEmptyNode->next = tmp;
            return NULL;
        }
        //unk_sprite_63FE34 = tmp; I don't have one
        spriteEmptyNode = tmp;
        return NULL;
    }

    // y-based tile sorting
    /*
    int tmpid = (tmp->position.y + (((tmp->position.y < 0)*-1) & 0x1F)) / 32;
    if (tmpid < 0)
    {
        tmpid = 0
    }
    else
    {
        if (tmpid >= mapHeight)
        {
            tmpid = mapHeight - 1;
        }
    }

    if (unk_sprite_629688[tmpid] != NULL)
    {
        if (unk_sprite_629288[tmpid] == unk_sprite_629688[tmpid])
        {
            unk_sprite_629288[tmpid] = tmp;
        }
        tmp->prev = unk_sprite_629688[tmpid];
        tmp->next = unk_sprite_629688[tmpid]->next;
        if (unk_sprite_629688[tmpid]->next != NULL)
        {
            unk_sprite_629688[tmpid]->next->prev = tmp;
        }
        unk_sprite_629688[tmpid]->next = tmp;
        return tmp;
    }
    unk_sprite_629288[tmpid] = tmp;
    unk_sprite_629688[tmpid] = tmp;
    */
    return tmp;
}

void Graphics::ImageDestructor(ImageNode* image)
{
    if (unk_6CEFB5 & 1) // I dunno what this is
    {
        refreshImage(image); // (EAX)
    }

    if (image->spriteOwner->pImageHead == image) // Is first?
    {
        image->spriteOwner->pImageHead = image->next; // Next one is now first
    }
    if (image->spriteOwner->pImageTail == image) // Is last?
    {
        image->spriteOwner->pImageTail = image->prev; // Previous one is now last
    }

    // Remove from spriteOwner's image list
    if (image->prev != NULL)
    {
        image->prev->next = image->next;
    }
    if (image->next != NULL)
    {
        image->next->prev = image->prev;
    }
    image->prev = NULL;
    image->next = NULL;
    image->GRPFile = NULL;

    // Re-insert in to empty node list
    if (imageEmptyNode == NULL)
    {
        imageEmptyNode = image;
        //unk_img_57EB70 = image; // LastEmpty -- I don't have one
        return;
    }

    //if (unk_img_57EB70 == imageEmptyNode) // LastEmpty -- I don't have one
    //{
    //  unk_img_57EB70 = image;
    //}
    image->prev = imageEmptyNode;
    image->next = imageEmptyNode->next;
    if (imageEmptyNode->next != NULL)
    {
        imageEmptyNode->next->prev = image;
    }
    imageEmptyNode->next = image;
    return;
}

ImageNode* Graphics::CreateImageOverlay(SpriteNode* sprite, int imageid, int x, int y, int direction)
{
    ImageNode* tmp = unknown_004D4E30(); // GetNextUnusedImage ( ? )
    if (tmp == NULL) return NULL;
    if (sprite->pImageHead != NULL)
    {
        if (sprite->pImageHead == sprite->pImagePrimary)
        {
            sprite->pImageHead = tmp;
        }
        tmp->prev = sprite->pImagePrimary->prev;
        tmp->next = sprite->pImagePrimary;
        if (sprite->pImagePrimary->prev != NULL)
        {
            sprite->pImagePrimary->prev->next = tmp;
        }
        sprite->pImagePrimary->prev = tmp;
    }
    else
    {
        sprite->pImagePrimary = tmp;
        if (sprite->pImageHead != NULL)
        {
            if (sprite->pImageTail == sprite->pImageHead)
            {
                sprite->pImageTail = tmp;
            }
            tmp->prev = sprite->pImageHead;
            tmp->next = sprite->pImageHead->next;
            if (sprite->pImageHead->next != NULL)
            {
                sprite->pImageHead->next->prev = tmp;
            }
            sprite->pImageHead = tmp;
        }
        else
        {
            sprite->pImageTail = tmp;
            sprite->pImageHead = tmp;
        }
    }
    InitializeImageData(tmp, sprite, imageid, x, y);
    unknown_004D66B0(tmp, sprite, imageid);
    updateImageDirection(tmp, direction);
    return tmp;
}

void Graphics::ISCRIPT_PlayFrame(ImageNode* image, int frame)
{
    if (frame == image->frameSet) return;
    image->frameSet = frame;
    if (image->frameIndex != (image->direction + frame))
    {
        image->flags |= 1; // Redraw
        image->frameIndex = image->direction + frame;
    }
    return;
}

void Graphics::ISCRIPT_setPosition(ImageNode* image, s8 x, s8 y)
{
    if (image->horizontalOffset == x && image->verticalOffset == y) return;
    image->flags |= 1; // Redraw
    image->horizontalOffset = x;
    image->verticalOffset = y;
    return;
}

ImageNode* Graphics::ISCRIPT_CreateImage(ImageNode* parent, int id, int horizontalOffset, int verticalOffset, int isOverlay) {
    ImageNode* newImage = unknown_004D4E30(); // GetNextUnusedImage ( ? )
    if (newImage == NULL) return NULL;
    if (isOverlay)
    {
        if (parent->spriteOwner->pImageHead == parent)
        {
            parent->spriteOwner->pImageHead = newImage;
        }
        newImage->prev = parent->prev;
        newImage->next = parent;
        if (parent->prev != NULL)
        {
            parent->prev->next = newImage;
        }
        parent->prev = newImage;
    }
    else
    {
        if (parent->spriteOwner->pImageTail == parent)
        {
            parent->spriteOwner->pImageTail = newImage;
        }
        newImage->prev = parent;
        newImage->next = parent->next;
        if (parent->next != NULL)
        {
            parent->next->prev = newImage;
        }
        parent->next = newImage;
    }
    InitializeImageData(newImage, parent->spriteOwner, id, horizontalOffset, verticalOffset);
    unknown_004D66B0(newImage, parent->spriteOwner, id);
    if (newImage->paletteType == DRAW_NORMAL)
    {
        if (activeIscriptUnit != NULL)
        {
            if (activeIscriptUnit->statusFlags & 0x40000000) // IsHallucination
            {
                if (unitIsOwnerByCurrentPlayer(activeIscriptUnit) == 0)
                {
                    //if (unk_6D0F14) // ?
                    //{
                        newImage->paletteType = DRAW_HALLUCINATION;
                        newImage->updateFunction = imgUpdateFxns[DRAW_HALLUCINATION].func; // Image Update Functions[0x10][1]
                        newImage->renderFunction = (newImage->flags & 2) ? imgRenderFxns[DRAW_HALLUCINATION].flip : imgRenderFxns[DRAW_HALLUCINATION].norm;
                        newImage->flags |= 1; // Redraw
                        newImage->coloringData.raw32 = 0;
                    //}
                }
            }
        }
    }
    if (newImage->flags & 8) // Has rotation frames
    {
        if (newImage->flags & 2) // Flipped/Mirrored
        {
            updateImageDirection(newImage, 0x20 - parent->direction);
        }
        else
        {
            updateImageDirection(newImage, parent->direction);
        }
    }
    if (newImage->frameIndex != (newImage->frameSet + newImage->direction))
    {
        newImage->flags |= 1; // Redraw
        newImage->frameIndex = newImage->frameSet + newImage->direction;
    }

    if (activeIscriptUnit)
    {
        if (activeIscriptUnit->statusFlags & 0x300) // RequiresDetection | Cloaked
        {
            if (chkd.scData.ImageDat(id)->DrawIfCloaked == 0)
            {
                
                hideImage(newImage);
                return newImage;
            }
            if (newImage->paletteType == DRAW_NORMAL)
            {
                CopyImagePaletteType(newImage, parent->spriteOwner->pImageHead); // (dst, src)
            }
        }
    }
    return newImage;
}

void Graphics::updateImagePositionOffset(ImageNode* image)
{
    GRP& lo = chkd.scData.grps[chkd.scData.ImageDat(image->spriteOwner->pImagePrimary->imageID)->SpecialOverlay - 1];
    LODATA* lodata = lo.LoGetOffset(image->spriteOwner->pImagePrimary->frameIndex, 0);
    if (image->spriteOwner->pImagePrimary->flags & 2) // Flipped
    {
        lodata->xOffset = -lodata->xOffset;
    }
    if (image->horizontalOffset != lodata->xOffset || image->verticalOffset != lodata->yOffset)
    {
        image->flags |= 1; // Redraw
        image->horizontalOffset = lodata->xOffset;
        image->verticalOffset = lodata->yOffset;
    }
    return;
}

void Graphics::ISCRIPT_UseLOFile(ImageNode* image, u32 losrc, u32 goffs, point* out) {
    u32 loID;
    switch (losrc) // CHKD doesn't have a happy LO? array for easy indexing like SC does
    {
        case IMAGES_ATTACK_OVERLAY:
            loID = chkd.scData.ImageDat(image->imageID)->AttackOverlay;
            break;
        case IMAGES_DAMAGE_OVERLAY:
            loID = chkd.scData.ImageDat(image->imageID)->DamageOverlay;
            break;
        case IMAGES_SPECIAL_OVERLAY:
            loID = chkd.scData.ImageDat(image->imageID)->SpecialOverlay;
            break;
        case IMAGES_LANDING_OVERLAY:
            loID = chkd.scData.ImageDat(image->imageID)->LandingDustOverlay;
            break;
        case IMAGES_LIFTING_OVERLAY:
            loID = chkd.scData.ImageDat(image->imageID)->LiftOffOverlay;
            break;
        default:
            loID = 0; // Error ?
    }
    // Error checking ? -- SC doesn't, but it might be nice.

    GRP& lo = chkd.scData.grps[loID - 1];
    LODATA* lodata = lo.LoGetOffset(image->frameIndex, goffs);
    out->x = lodata->xOffset;
    out->y = lodata->yOffset;
    if (image->flags & 2) // Flipped/Mirrored
    {
        out->x = -out->x;
    }
    return;
}

bool Graphics::canUnitTypeFitAt(u16 unitID, s16 x, s16 y) {
    /*if (x < chkd.scData.UnitDat(unitID)->UnitSizeLeft) return false;
    if ((chkd.scData.UnitDat(unitID)->UnitSizeRight + x) >= (u16)628450) return false;
    if (y < chkd.scData.UnitDat(unitID)->UnitSizeUp) return false;
    if ((chkd.scData.UnitDat(unitID)->UnitSizeDown + y) >= (u16)6284B4) return false;

    EAX = (s16)chkd.scData.UnitDat(unitID)->UnitSizeDown + y
    ECX = y
    EDX = (u16)6284B4
    ESI = x
    EDI = y
    if (!isVisible(? )) return false;
    EAX = *6D5BFC + 0x97A1C
    ECX = unitID
    setContourSearchVariablesFromUnitType(? )
    ECX = EAX(? )
    return isContourSpaceAvailable(ECX, x ? , y ? ) != 0; // ECX, push ESI, push EDI*/
    return true;
}

bool Graphics::unitIsGoliath(UnitNode* unit)
{
    if (unit->unitType == 3) return true; // Terran Goliath
    if (unit->unitType == 0x11) return true; // Alan Schezar
    return false;
}

SpriteNode* Graphics::ISCRIPT_CreateSprite(ImageNode* image, int id, int x, int y, int elevation) // CThingy ?
{
    SpriteNode* ret = CreateThingy(id, image->spriteOwner->position.x + image->horizontalOffset + x, image->spriteOwner->position.y + image->verticalOffset + y, 0); // CThingy
    if (ret == NULL) return NULL;
    ret/*->sprite*/->elevationLevel = elevation;
    unknown_004878F0(ret);
    return ret;
}

void Graphics::flipImage(ImageNode* image, u8 flip)
{
    if (((image->flags >> 1) & 1) == flip) // is flip == current flip state?
    {
        return; // Do nothing
    }
    image->flags &= ~2; // Clear flag
    image->flags |= (flip & 1) << 1; // Set new flip state
    setImagePaletteType(image, image->paletteType);
    if (image->flags & 0x80) // Update Lo? Offset?
    {
        updateImagePositionOffset(image);
    }
    return;
}

void Graphics::ISCRIPT_PlaySnd(ImageNode* image, u16 sound)
{
    // lol
    return;
}

void Graphics::ISCRIPT_AttackMelee(UnitNode* unit)
{
    // lol
    return;
}

void Graphics::setImagePaletteType(ImageNode* image, char paletteType)
{
    image->paletteType = paletteType;

    image->updateFunction = imgUpdateFxns[paletteType].func; // Image Update Functions + 4
    if (image->flags & 2) // Flipped/Mirrored
    {
        image->renderFunction = imgRenderFxns[paletteType].flip; // Image Drawing Functions + 8
    }
    else
    {
        image->renderFunction = imgRenderFxns[paletteType].norm; // Image Drawing Functions + 4
    }
    if (paletteType == DRAW_WARP_FLASH)
    {
        image->coloringData.warpFlash.state = 0x30;
        image->coloringData.warpFlash.timer = 0x02;
    }
    image->flags |= 1; // Redraw
    return;
}

void Graphics::updateImageFrameIndex(ImageNode* image)
{
    if (image->frameIndex != (image->frameSet + image->direction)) // Do nothing if no change
    {
        image->flags |= 1; // Redraw
        image->frameIndex = image->frameSet + image->direction;
    }
    return;
}


void Graphics::showImage(ImageNode* image)
{
    if (image->flags & 0x40) // Hidden
    {
        image->flags &= ~0x40;
        image->flags |= 0x01; // Redraw
    }
    return;
}


void Graphics::hideImage(ImageNode* image)
{
    if (image->flags & 0x40) return; // Hidden -- Do nothing
    if (!(unk_6CEFB5 & 1))
    {
        refreshImage(image);
    }
    image->flags |= 0x40; // Hidden
    return;
}

void Graphics::CBullet_Damage(UnitNode* unit)
{
    // lol
    return;
}

void Graphics::_turnGraphic(UnitNode* unit, int direction)
{
    turnUnit(unit, unit->currentDirection1 - (direction << 3));
}

void Graphics::unitSetRetreatPoint(UnitNode* unit, u8 direction)
{
    int x, y;
    x = ((arr_512D28[direction << 3].x << 0xC) / 256 + unit->halt.x) / 256;
    y = ((arr_512D28[direction << 3].y << 0xC) / 256 + unit->halt.y) / 256;
    if (x != unit->nextTargetWaypoint.x ||
        y != unit->nextTargetWaypoint.y) {
        unit->nextTargetWaypoint.x = x;
        unit->nextTargetWaypoint.y = y;
    }
    return;
}

void Graphics::unknown_004762C0(UnitNode* unit, int* unk_x, int* unk_y)
{
    //s16 v[6]; // 0 = [EBP-C], 1 = [EBP-A],  2 = [EBP-8], 3 = [EBP-6], 4 = [EBP-4], 5 = [EBP-2]
    rect tgt_rect; // v[0] - v[3]
    points tgt_center; // v[4] - v[5]
    points pos; // [EBP+8] ?
    rect d; // rect

    pos.x = unit->sprite->position.x;
    pos.y = unit->sprite->position.y;

    d.left = unit->orderTargetUnit->sprite->position.x - chkd.scData.UnitDat(unit->orderTargetUnit->unitType)->UnitSizeLeft;
    d.top = unit->orderTargetUnit->sprite->position.y - chkd.scData.UnitDat(unit->orderTargetUnit->unitType)->UnitSizeUp;
    d.right = unit->orderTargetUnit->sprite->position.x + chkd.scData.UnitDat(unit->orderTargetUnit->unitType)->UnitSizeRight;
    d.bottom = unit->orderTargetUnit->sprite->position.y + chkd.scData.UnitDat(unit->orderTargetUnit->unitType)->UnitSizeDown;

    
    tgt_center.x = (d.right - d.left) / 4;
    tgt_center.y = (d.bottom - d.top) / 4;
    tgt_rect.left = d.left + tgt_center.x;  // Left
    tgt_rect.top = d.top + tgt_center.y;    // Top
    tgt_rect.right = d.right - tgt_center.x; // Right
    tgt_rect.bottom = d.bottom - tgt_center.y;  // Bottom
    tgt_center.x = tgt_rect.right - tgt_rect.left;
    tgt_center.x = (tgt_center.x + (tgt_center.x < 0)) / 2 + tgt_rect.left; // Center X
    tgt_center.y = tgt_rect.bottom - tgt_rect.top;
    tgt_center.y = (tgt_center.y + (tgt_center.y < 0)) / 2 + tgt_rect.top; // Center Y

    if (unknown_0042A5C0(&tgt_center, &pos, &tgt_rect))
    {
        *unk_x = pos.x;
        *unk_y = pos.y;
    }
    else
    {
        *unk_x = tgt_center.x;
        *unk_y = tgt_center.y;
    }
    return;
}

bool Graphics::unknown_0042A5C0(points* tgt_center, points* pos, rect* tgt_rect)
{
    points newCenter = *tgt_center;
    points tmp;
    points newPos = *pos;
    int l = tgt_rect->left;
    int t = tgt_rect->top;
    int r = tgt_rect->right;
    int d = tgt_rect->bottom;
    int ebp1c;

    int cflags = (newCenter.x < tgt_rect->left) |
                 (((newCenter.x <= tgt_rect->right) - 1) & 2) |
                 (((newCenter.y >= tgt_rect->top) - 1) & 8) |
                 (((newCenter.y <= d) - 1) & 4);

    int pflags = (newPos.x < tgt_rect->left) |
                 (((newPos.x <= r) - 1) & 2) |
                 (((newPos.y >= t) - 1) & 8) |
                 (((newPos.y <= d) - 1) & 4);

    int tflags;

    while (pflags | cflags)
    {
        if (pflags & cflags) return false;
        if (cflags)
        {
            tflags = cflags;
        }
        else
        {
            tflags = pflags;
        }

        if (tflags & 8)
        {
            ebp1c = tgt_rect->top;
            tmp.x = ((newPos.x - newCenter.x) * (ebp1c - newCenter.y)) / (newPos.y - newCenter.y) + newCenter.x;
            tmp.y = ebp1c;
        }
        else if (tflags & 4)
        {
            ebp1c = tgt_rect->bottom;
            tmp.x = ((newPos.x - newCenter.x) * (ebp1c - newCenter.y)) / (newPos.y - newCenter.y) + newCenter.x;
            tmp.y = ebp1c;
        }
        else if (tflags & 2)
        {
            tmp.x = tgt_rect->right;
            tmp.y = ((newPos.y - newCenter.y) * (tgt_rect->right - newCenter.x)) / (newPos.x - newCenter.x) + newCenter.y;
        }
        else
        {
            tmp.x = tgt_rect->left;
            tmp.y = ((newPos.y - newCenter.y) * (tgt_rect->left - newCenter.x)) / (newPos.x - newCenter.x) + newCenter.y;
        }

        if (tflags == cflags)
        {
            newCenter.x = tmp.x;
            newCenter.y = tmp.y;
            cflags = (tmp.x < tgt_rect->left) |
                     (((tmp.x <= r) - 1) & 2) |
                     (((tmp.y >= t) - 1) & 8) |
                     (((tmp.y <= d) - 1) & 4);
        }
        else
        {
            newPos.x = tmp.x;
            newPos.y = tmp.y;
            pflags = (tmp.x < tgt_rect->left) |
                     (((tmp.x <= r) - 1) & 2) |
                     (((tmp.y >= t) - 1) & 8) |
                     (((tmp.y <= d) - 1) & 4);
        }
    }
    *tgt_center = newCenter;
    *pos = newPos;
    return true;
}

int Graphics::isDistanceGreaterThanHaltDistance(UnitNode* unit, int distance, int x, int y)
{
    return (distance >= (getDistanceFast(unit->halt.x, x << 8, unit->halt.y, y << 8) >> 8));
}

int Graphics::getDistanceFast(int x1, int x2, int y1, int y2)
{
    x1 = abs(x1 - x2);
    y1 = abs(y1 - y2);
    if (x1<y1)
    {
        x2 = x1;
        x1 = y1;
        y1 = x2;
    }
    x2 = x1;
    x1 >>= 2;
    if (x1 >= y1) return x2;
    x1 >>= 2;
    y2 = (y1 * 3) >> 3;
    return ((((x2 - x1) + y2) - (x1 >> 2)) + (y2 >> 5));
}

int Graphics::unknown_00494BD0(int a, int b) // angleDistance
{
    a -= b;
    a &= 0xFF;
    if (a <= 0x80) return a;
    return (0x100 - a);
}

int Graphics::getDirectionFromPoints(int x1, int y1, int x2, int y2)
{
    x1 -= x2;
    y1 -= y2;
    if (x1 == 0)
    {
        return ((y1 <= 0) - 1) & 0x80;
    }
    y1 = (y1 << 8) / x1;
    if (x1 >= 0)
    {
        return GetFastestDistance(y1) + 0x40;
    }
    y1 = GetFastestDistance(y1) + 0xC0;
    return (y1 & ((y1 == 0x100) - 1));
}

int Graphics::GetFastestDistance(int x)
{
    int neg;
    if (x < 0)
    {
        neg = 1;
        x = -x;
    }
    else
    {
        neg = 0;
    }
    int a = 0x20;
    int c = 0x40;
    int d = 0;
    do
    {
        if (x > arr_513528[a])
        {
            d = a + 1;
        }
        else
        {
            c = a;
        }
        a = (d + c) / 2;
    } while (c != d);
    if (neg) return -a;
    return a;
}

void Graphics::ISCRIPT_AttackWith(UnitNode* unit, u8 arg)
{
    // lol
    return;
}

bool Graphics::canCastSpell_0(UnitNode* unit)
{
    // lol
    return true;
}

void Graphics::ISCRIPT_UseWeapon(UnitNode* unit, u8 weapon)
{
    // lol
    return;
}

void Graphics::IgnoreAllScriptAndGotoIdle(UnitNode* unit)
{
    unit->statusFlags &= ~0x2000;
    unit->sprite->flags &= ~0x80;
    UnitNode* tmp_6D11F4 = unk_unit_6D11F4;
    UnitNode* tmp_6D11FC = activeIscriptUnit;
    unk_unit_6D11F4 = unit;
    activeIscriptUnit = unit;
    playIdleIscript(unit->sprite);
    unk_unit_6D11F4 = tmp_6D11F4;
    activeIscriptUnit = tmp_6D11FC;
    unit->movementFlags &= ~0x08; // StartingAttack
    return;
}

void Graphics::ISCRIPT_NoBrkCodeEnd(UnitNode* unit)
{
    unit->statusFlags &= ~0x2000; // NoBrkCodeStart
    unit->sprite->flags &= ~0x80; // Iscript unbreakable code section.
    //if (unit->orderQueueHead == NULL) return;
    if (unit->userActionFlags & 1) {
        IgnoreAllScriptAndGotoIdle(unit);
        orders_Nothing2(unit);
    }
    return;
}

bool Graphics::initSpriteData(SpriteNode* sprite, short id, short x, short y, u8 player)
{
    //if (x >= (*p628450)) return false;
    //if (y >= (*p6284B4)) return false; // I don't know what these are
    sprite->playerID = player;
    sprite->spriteID = id;
    sprite->flags = 0;
    sprite->position.x = x;
    sprite->position.y = y;
    sprite->visibilityFlags = 0xFF;
    sprite->elevationLevel = 4;
    sprite->selectionTimer = 0;
    
    if (chkd.scData.SpriteDat(id)->IsVisible == 0)
    {
        sprite->flags = 0x20; // Hidden
        refreshAllVisibleImagesAtScreenPosition(sprite, 0);
    }
    if (CreateImageOverlay(sprite, chkd.scData.SpriteDat(id)->ImageFile, 0, 0, 0) == NULL) return false;
    sprite->unkflags_12 = 0xFF;
    if (sprite->pImagePrimary->GRPFile->GrpWidth() <= 0xFF)
    {
        sprite->unkflags_12 = (u8)sprite->pImagePrimary->GRPFile->GrpWidth();
    }
    if (sprite->pImagePrimary->GRPFile->GrpHeight() >= 0xFF)
    {
        sprite->unkflags_13 = 0xFF;
    }
    else
    {
        sprite->unkflags_13 = (u8)sprite->pImagePrimary->GRPFile->GrpHeight();
    }
    return true;
}

void Graphics::refreshImage(ImageNode* image)
{
    rect r;
    if (image->grpBounds.bottom > 0) {
        if (image->grpBounds.right > 0) {
            r.left = image->screenPosition.x;
            r.top = image->screenPosition.y;
            r.right = image->grpBounds.right + image->screenPosition.x;
            r.bottom = image->grpBounds.bottom + image->screenPosition.y;
            refreshImageData(&r);
        }
    }
    return;
}

ImageNode* Graphics::unknown_004D4E30() // GetNextUnusedImage
{
    ImageNode* tmp = imageEmptyNode;
    if (imageEmptyNode == NULL) // Is empty?
    {
        // Create one!
        imageNodes.push_back(std::shared_ptr<ImageNode>(new ImageNode()));
        tmp = imageNodes.back().get(); // Get new node
        //return NULL;
    }
    imageEmptyNode = tmp->next;
    //if (unk_img_57EB70 == tmp) // Last empty -- we don't have one
    //{
    //  unk_img_57EB70 = tmp->prev;
    //}
    if (tmp->prev != NULL)
    {
        tmp->prev->next = tmp->next;
    }
    if (tmp->next != NULL)
    {
        tmp->next->prev = tmp->prev;
    }
    tmp->prev = NULL;
    tmp->next = NULL;
    return tmp;
}

void Graphics::InitializeImageData(ImageNode* image, SpriteNode* sprite, int imageID, int x, int y)
{
    IMAGEDAT* imagesdat = chkd.scData.ImageDat(imageID);
    image->imageID = imageID;
    image->GRPFile = &chkd.scData.grps[imagesdat->GRPFile - 1];
    image->flags |= (imagesdat->GraphicTurns & 1) << 3; // 0x0008 - Has rotation frames
    image->flags &= ~0x20; // Clickable
    image->frameSet = 0;
    image->direction = 0;
    image->anim = 0;
    image->spriteOwner = sprite;
    image->flags |= (imagesdat->Clickable & 1) << 5; // 0x0020 - Clickable
    image->horizontalOffset = x;
    image->verticalOffset = y;
    image->grpBounds.left = 0;
    image->grpBounds.top = 0;
    image->grpBounds.right = 0;
    image->grpBounds.bottom = 0;
    image->coloringData.raw32 = 0;
    image->iscriptHeader = 0;
    image->iscriptOffset = 0;
    image->iscriptReturn = 0;
    image->anim = 0;
    image->sleep = 0;
    if (imagesdat->DrawFunction == DRAW_PLAYER_SIDE)
    {
        image->coloringData.playerColor = image->spriteOwner->playerID;
    }
    if (imagesdat->DrawFunction == DRAW_EFFECT)
    {
        if (imagesdat->Remapping == 0) // This normally reads from a table, like imgRender/UpdateFxns, with the 0 entry being NULL
        {
            image->coloringData.effect = NULL;
        }
        else // More error checking is probably good
        {
            image->coloringData.effect = &chkd.scData.tilesets.set[map.getTileset()].remap[imagesdat->Remapping - 1]; // No 0 entry, so -1
        }
    }
    return;
}

void Graphics::unknown_004D66B0(ImageNode* image, SpriteNode* sprite, int imageid)
{
    image->paletteType = chkd.scData.ImageDat(image->imageID)->DrawFunction;
    image->updateFunction = imgUpdateFxns[image->paletteType].func;
    if (image->flags & 2) // Flipped
    {
        image->renderFunction = imgRenderFxns[image->paletteType].flip;
    }
    else
    {
        image->renderFunction = imgRenderFxns[image->paletteType].norm;
    }
    if (image->paletteType == DRAW_WARP_FLASH)
    {
        image->coloringData.warpFlash.state = 0x30;
        image->coloringData.warpFlash.timer = 0x02;
    }
    image->flags |= 1; // Redraw
    image->flags &= ~0x10;
    image->flags |= (chkd.scData.ImageDat(image->imageID)->UseFullIscript & 1) << 4; // 0x10 - (Use Full Iscript)
    image->iscriptHeader = chkd.scData.iScripts.GetHeaderOffset(chkd.scData.ImageDat(image->imageID)->IscriptID);
    playImageIscript(image, ANIM_INIT); // Init
    CImage__updateGraphicData(image);
}

void Graphics::updateImageDirection(ImageNode* image, int direction)
{
    int flipped;
    if (!(image->flags & 0x08)) return; // Has rotation frames

    if (direction > 0x10)
    {
        direction = 0x20 - direction;
        flipped = 1;
    }
    else
    {
        flipped = 0;
    }

    if (((image->flags >> 1) & 1) == flipped) // Flipped/Mirrored
    {
        if (image->direction == direction) return;
    }

    image->direction = direction;
    image->flags &= ~2; // Flipped/Mirrored
    image->flags |= flipped << 1;

    setImagePaletteType(image, image->paletteType);

    if (image->frameIndex != (image->frameSet + image->direction))
    {
        image->flags |= 1; // Redraw
    }
    if (!(image->flags & 0x80)) return;
    updateImagePositionOffset(image);
    return;
}

bool Graphics::unitIsOwnerByCurrentPlayer(UnitNode* unit)
{
    //if (unk_6D0F14) return false; -- Is current player?
    //if (unit->playerID == g_LocalNationID) return true;
    //return false;
    return true; // Why not?
}

void Graphics::CopyImagePaletteType(ImageNode* dst, ImageNode* src)
{
    if (src->paletteType != DRAW_ENEMY_CLOAK && src->paletteType != DRAW_DECLOAK) // Cloaking, decloaking
    {
        setImagePaletteType(dst, src->paletteType);
        dst->coloringData.raw16 = src->coloringData.raw16;
    }
    return;
}

SpriteNode* Graphics::CreateThingy(int id, int x, int y, int unk) // CThingy
{
    // I'm not using Thingies (I don't know what they are for), so just create & return a sprite.
    SpriteNode* /*
    CThingy* tmp = unk_thingy_654878;
    if (tmp == NULL) return NULL;
    unk_thingy_654878 = tmp->next;
    // Remove thingy from empty list
    if (unk_thingy_65487C == tmp)
    {
        unk_thingy_65487C = tmp->prev;
    }
    if (unk_thingy_654878->prev != NULL)
    {
        unk_thingy_654878->prev->next = tmp->next;
    }
    if (tmp->next != NULL)
    {
        tmp->next->prev = unk_thingy_654878->prev;
    }
    tmp->prev = NULL;
    tmp->next = NULL;
    tmp->param = 1;
    tmp->*/sprite = createSprite(id, x, y, unk);
    if (/*tmp->*/sprite == NULL)
    {
        // Destroy & add Thingy back to empty list -- if there was one
        /*if (unk_thingy_654878 != NULL)
        {
            if (unk_thingy_65487C == unk_thingy_654878)
            {
                unk_thingy_65487C == tmp;
            }
            tmp->prev = unk_thingy_654878;
            tmp->next = unk_thingy_654878->next;
            if (unk_thingy_654878->next != NULL)
            {
                unk_thingy_654878->next->prev = tmp;
            }
            unk_thingy_654878->next = tmp;
            return NULL;
        }
        unk_thingy_65487C = tmp;
        unk_thingy_654878 = tmp;*/
        return NULL;
    }
    // Add to thingy used list
    /*if (unk_thingy_654874 != NULL)
    {
        if (unk_thingy_65291C == unk_thingy_654874)
        {
            unk_thingy_65291C = tmp;
        }
        tmp->prev = unk_thingy_654874;
        tmp->next = unk_thingy_654874->next;
        if (unk_thingy_654874->next != NULL)
        {
            unk_thingy_654874->next->prev = tmp;
        }
        unk_thingy_654874->next = tmp;
        return tmp;
    }
    unk_thingy_65291C = tmp;
    unk_thingy_654874 = tmp;*/
    // thingyLastCreated = tmp;
    return sprite;
}

bool Graphics::unknown_004878F0(SpriteNode* thingy) // CFlingy / Thingy
{
    //if (thingy->sprite == NULL) return false;
    if (thingy/*->sprite*/->flags & 0x20) return false; // Unbreakable Code Section
    // I don't know what this array is
    int EBP5 = 0; //~unk_arr_6D1260[(thingy/*->sprite*/->position.y + (((thingy/*->sprite*/->position.y < 0) * -1) & 0x1F)) / 32 * mapWidth + (thingy/*->sprite*/->position.x + ((thingy/*->sprite*/->position.x < 0) * -1) & 0x1F) / 32];
    if (thingy/*->sprite*/->visibilityFlags == EBP5) return false;
    refreshAllVisibleImagesAtScreenPosition(thingy/*->sprite*/, EBP5);
    if (!(thingy/*->sprite*/->visibilityFlags & visionFilter)) return false;
    return true;
}

void Graphics::playIdleIscript(SpriteNode* sprite)
{
    u8 newAnim;
    switch (sprite->pImagePrimary->anim)
    {
    case 3:
    case 6:
        newAnim = 9;
        break;

    case 15:
        if (sprite->spriteID != 0xEF &&
            sprite->spriteID != 0x8C &&
            sprite->spriteID != 0xC2) return;
    case 2:
    case 5:
        newAnim = 8;
        break;

    case 13:
        if (sprite->spriteID != 0x1E9) return;
    case 7:
        newAnim = 0xC;
        break;

    default:
        return;
    }

    for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
    {
        playImageIscript(image, newAnim);
    }
    return;
}

void Graphics::orders_Nothing2(UnitNode* unit)
{
    /*if (unit->pAI != NULL &&
        unit->orderQueueHead != NULL &&
        unit->statusFlags & 0x10) != 0 &&
        unit->orderQueueHead->orderID != 0 &&
        unit->orderQueueHead->orderID != 0x76 &&
        unit->orderQueueHead->orderID != 0x75 &&
        unit->orderQueueHead->orderID != 0x9C &&
        unit->unitType != 0xD &&
        unit->unitType == 0x67 ||
        (unit->orderQueueHead->orderID != 2 &&
            unit->orderQueueHead->orderID != 0xB) )
    {
        EDX = unit->sprite->position.y
        ECX = unit->sprite->position.x
        EDI = unit->orderQueueHead
        unknown_00474790(unit->orderQueueHead, 0x76, unit->sprite->position.x, unit->sprite->position.y); // (EDI, BL, push, push)

        EAX = unit->statusFlags & ~0x1000
        ECX = unit
        unit->statusFlags &= ~0x1000;
        toIdle(unit); // ECX
        return
    }

    ECX = unit->statusFlags
    if (((unit->statusFlags & 0x3000) == 0 ||
        (unit->orderQueueHead != NULL &&
            unit->orderQueueHead->orderID == 0) &&
        unit->orderQueueHead != NULL))
    {
        ECX = unit
        removeOrderFromUnitQueue(unit, ?  ); // (ECX, ?)

        BL = unit->orderQueueHead->orderID
        EDI = unit->orderQueueHead->target.x
        EAX = unit->orderQueueHead->target.y
        [EBP - 8] = unit->orderQueueHead->unitType;
        [EBP - C] = unit->orderQueueHead->orderID
        u8[EBP - 4] = unit->orderQueueHead->orderID
        [EBP - 14] = unit->orderQueueHead->target.x
        [EBP - 10] = unit->orderQueueHead->target.y

        unit->orderQueueTimer = 0;
        unit->recentOrderTimer = 0;
        EDX = ((ordersdat.Can Be Interrupted[unit->orderQueueHead->orderID] == 0) & 1) << 0xC
        ECX = (unit->statusFlags & 0xF7FFEFF7) | ((ordersdat.Can Be Interrupted[unit->orderQueueHead->orderID] == 0) & 1) << 0xC ?
        unit->userActionFlags &= 0xFE;

        unit->statusFlags = (unit->statusFlags & 0xF7FFEFF7) | ((ordersdat.Can Be Interrupted[unit->orderQueueHead->orderID] == 0) & 1) << 0xC;
        ECX = 0
        if (unit->orderQueueHead->target.y != 0)
        {
            ECX = unit->orderQueueHead->target.y
            DL = unit->orderQueueHead->orderID
            EAX = unit
            unitOrderMoveToTargetUnitResetOrderState( ? );

            ECX = 0;
        }
        else
        {
            EAX = unit->orderQueueHead->unitType
            unit->orderState = 0;
            unit->orderTarget.x = unit->orderQueueHead->target.x;
            unit->orderTargetUnit = NULL;
            unit->orderID = 0;
            if (unit->orderQueueHead->unitType != 0xE4) // None
            {
                DX = unit->orderQueueHead->target.y
                unit->orderUnitType = unit->orderQueueHead->unitType;
                unit->orderTarget.y = unit->orderQueueHead->target.y;
            }
            else
            {
                AX = unit->orderQueueHead->target.y
                unit->orderTarget.y = unit->orderQueueHead->target.y;
                unit->orderUnitType = 0xE4;
            }
        }
    }
    if (unit->pAI == NULL)
    {
        unit->autoTargetUnit = NULL;
    }
    IgnoreAllScriptAndGotoIdle( ? );

    EAX = unit->unitType
    */
    /*
        0047516D | .F60485 8040660>TEST BYTE PTR DS : [EAX * 4 + 664080], 10
        00475175 | . 0F85 9E000000  JNZ StarCraf.00475219
        0047517B | . 8B76 70        MOV ESI, DWORD PTR DS : [ESI + 70]
        0047517E | . 85F6           TEST ESI, ESI
        00475180 | . 0F84 93000000  JE StarCraf.00475219
        00475186 | . 0FB74E 64      MOVZX ECX, WORD PTR DS : [ESI + 64]
        0047518A | .F6048D 8040660>TEST BYTE PTR DS : [ECX * 4 + 664080], 10
        00475192 | . 0F84 81000000  JE StarCraf.00475219
        00475198 | . 3A98 98486600  CMP BL, BYTE PTR DS : [EAX + 664898]
        0047519E | . 75 0F          JNZ SHORT StarCraf.004751AF
        004751A0 | . 0FB756 64      MOVZX EDX, WORD PTR DS : [ESI + 64]
        004751A4 | . 8A82 98486600  MOV AL, BYTE PTR DS : [EDX + 664898]
        004751AA | . 8845 FC        MOV BYTE PTR SS : [EBP - 4], AL
        004751AD | .EB 3B          JMP SHORT StarCraf.004751EA
        004751AF | > 3A98 20336600  CMP BL, BYTE PTR DS : [EAX + 663320]
        004751B5 | . 75 0F          JNZ SHORT StarCraf.004751C6
        004751B7 | . 0FB74E 64      MOVZX ECX, WORD PTR DS : [ESI + 64]
        004751BB | . 8A91 20336600  MOV DL, BYTE PTR DS : [ECX + 663320]
        004751C1 | . 8855 FC        MOV BYTE PTR SS : [EBP - 4], DL
        004751C4 | .EB 24          JMP SHORT StarCraf.004751EA
        004751C6 | > 3A98 503A6600  CMP BL, BYTE PTR DS : [EAX + 663A50]
        004751CC | . 75 0F          JNZ SHORT StarCraf.004751DD
        004751CE | . 0FB746 64      MOVZX EAX, WORD PTR DS : [ESI + 64]
        004751D2 | . 8A88 503A6600  MOV CL, BYTE PTR DS : [EAX + 663A50]
        004751D8 | . 884D FC        MOV BYTE PTR SS : [EBP - 4], CL
        004751DB | .EB 0D          JMP SHORT StarCraf.004751EA
        004751DD | > 8B55 F4        MOV EDX, DWORD PTR SS : [EBP - C]
        004751E0 | . 8A82 404A6600  MOV AL, BYTE PTR DS : [EDX + 664A40]
        004751E6 | . 84C0           TEST AL, AL
        004751E8 | . 74 2F          JE SHORT StarCraf.00475219
        004751EA | > 8B45 F0        MOV EAX, DWORD PTR SS : [EBP - 10]
        004751ED | . 85C0           TEST EAX, EAX
        004751EF | . 74 12          JE SHORT StarCraf.00475203
        004751F1 | . 50             PUSH EAX; / Arg3
        004751F2 | . 8B45 FC        MOV EAX, DWORD PTR SS : [EBP - 4]; |
        004751F5 | . 57             PUSH EDI; | Arg2
        004751F6 | . 50             PUSH EAX; | Arg1
        004751F7 | .E8 34FAFFFF    CALL StarCraf.00474C30; \orderInterrupt
        004751FC | . 5F             POP EDI
        004751FD | . 5E             POP ESI
        004751FE | . 5B             POP EBX
        004751FF | . 8BE5           MOV ESP, EBP
        00475201 | . 5D             POP EBP
        00475202 | .C3             RETN
        00475203 | > 0FBF4D EE      MOVSX ECX, WORD PTR SS : [EBP - 12]
        00475207 | . 8B45 F8        MOV EAX, DWORD PTR SS : [EBP - 8]
        0047520A | . 51             PUSH ECX; / Arg4
        0047520B | . 8B4D FC        MOV ECX, DWORD PTR SS : [EBP - 4]; |
        0047520E | . 0FBFD7         MOVSX EDX, DI; |
        00475211 | . 52             PUSH EDX; | Arg3
        00475212 | . 50             PUSH EAX; | Arg2
        00475213 | . 51             PUSH ECX; | Arg1
        00475214 | .E8 97FDFFFF    CALL StarCraf.00474FB0; \StarCraf.00474FB0; ? ? ?

        00475219 | > 5F             POP EDI
        0047521A | . 5E             POP ESI
        0047521B | . 5B             POP EBX
        0047521C | . 8BE5           MOV ESP, EBP
        0047521E | . 5D             POP EBP
        0047521F  \.C3             RETN
        */
}


void Graphics::makeSpriteVisible(SpriteNode* sprite)
{
    if (sprite->flags & 0x40) // Burrowed
    {
        //EAX = sprite
        // JMP StarCraf.00497E80
        return;
    }
    for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
    {
        if (image->paletteType >= DRAW_ENEMY_CLOAK && image->paletteType <= DRAW_ALLY_CLOAK)
        {
            image->paletteType += 3;
            image->updateFunction = imgUpdateFxns[image->paletteType].func;
            if (image->flags & 2)
            {
                image->renderFunction = imgRenderFxns[image->paletteType].flip;
            }
            else
            {
                image->renderFunction = imgRenderFxns[image->paletteType].norm;
            }
            if (image->paletteType == DRAW_WARP_FLASH) // wat, must be a carryover from an inline function or macro ?
            {
                image->coloringData.warpFlash.state = 0x30;
                image->coloringData.warpFlash.timer = 0x02;
            }
            image->flags |= 1; // Redraw
        }
        else if (image->paletteType == DRAW_NORMAL)
        {
            image->paletteType = DRAW_CLOAKED;
            image->updateFunction = imgUpdateFxns[DRAW_CLOAKED].func; // 512544
            if (image->flags & 2) // Flipped
            {
                image->renderFunction = imgRenderFxns[DRAW_CLOAKED].flip; // 5125F0
            }
            else
            {
                image->renderFunction = imgRenderFxns[DRAW_CLOAKED].norm; // 5125EC
            }
            image->flags |= 1; // Redraw
        }
    }
    return;
}

void Graphics::ApplyUnitEffects(UnitNode* unit)
{
    //EDI = unit
    if (unit->status.defenseMatrixTimer != 0)
    {
        //AL = unit->status.defenseMatrixTimer
        //EAX = unit
//      ApplyDefensiveMatrix(unit); // EAX
    }

    if (unit->status.lockdownTimer != 0)
    {
        //AL = unit->status.lockdownTimer
        //EAX = AL
        u32 timer = unit->status.lockdownTimer;
        unit->status.lockdownTimer = 0;
//      LockdownHit(unit, timer); // EDI ?, push EAX
    }

    if (unit->status.maelstromTimer != 0)
    {
        //AL = unit->status.maelstromTimer
        u32 timer = unit->status.maelstromTimer;
        unit->status.maelstromTimer = 0;
//      SetMaelstromTimer(unit, timer); // EDI ?, push EAX
    }

    if (unit->status.irradiateTimer != 0)
    {
        //AL = unit->status.irradiateTimer
        //EAX = unit
//      ApplyIrradiate(unit); // EAX
    }

    if (unit->status.ensnareTimer != 0)
    {
        //BL = unit->status.ensnareTimer
        u8 timer = unit->status.ensnareTimer;
        unit->status.ensnareTimer = 0;
//      CreateEnsnareOverlay(unit); // EDI ?
        unit->status.ensnareTimer = timer;
    }

    if (unit->status.stasisTimer != 0)
    {
        //AL = unit->status.stasisTimer
        u32 timer = unit->status.stasisTimer; // EDX
        unit->status.stasisTimer = 0;
//      StasisFieldOverlay(unit, timer); // EDI ?, push EDX
    }

    if (unit->status.plagueTimer != 0)
    {
        //BL = unit->status.plagueTimer
        u8 timer = unit->status.plagueTimer;
        unit->status.plagueTimer = 0;
//      CreatePlagueOverlay(unit); // EDI ?
        unit->status.plagueTimer = timer;
    }

    if (unit->status.acidSporeCount != 0)
    {
        //AL = unit->status.acidSporeCount
        //ECX = unit
        //GetAcidSporeImage(unit); // ECX ?
        //ECX = unit->subUnit
        if (unit->subUnit == NULL)
        {
            //ECX = unit
        }
        //ESI = EAX // GetAcidSporeImage retval or acidSporeCount ?
        //EAX = ECX->sprite
        //CreateOverlay(ECX->sprite, ESI, 0, 0, 0); // (EAX, ESI, push, push, push)
    }
    return;
}

void Graphics::UpdateUnitSpriteInfo(UnitNode* unit)
{
    //ESI = unit
    //EAX = unitsdat.bldgDim[unit->unitType].height
    //ECX = unitsdat.bldgDim[unit->unitType].width
    //EDI = EAX

    //EAX = unit->subUnit->sprite
    if (unit->subUnit != NULL &&
        !(unit->subUnit->sprite->flags & 0x20)) // Hidden
    {
        //EDX = unit->sprite
        //ECX = unit->sprite->visibilityFlags
        //EBX = unit->subUnit->sprite
        refreshAllVisibleImagesAtScreenPosition(unit->subUnit->sprite, unit->sprite->visibilityFlags);
    }

    //if (isThingyOnMap(unit)) // isThingyOnMap 004877B0 0000013E (push)
    //{
        //EDX = unit->sprite
        if (unit->sprite->flags & 8) // Selected
        {
            //EDX = unit
            //updateBuildingLandUnitSelection(unit); // EDX ?
        }
        //ECX = unit->sprite
        removeSelectionCircle(unit->sprite); // ECX ?
        //EAX = unit->sprite
        //ECX = unit->sprite->flags
        if (unit->sprite->flags & 6)
        {
            //ECX &= 0xF9
            unit->sprite->flags &= ~6;
            removeSelectionCircleFromSprite(unit->sprite);
        }
        if (((unit->statusFlags & 2) || (unit->unitType >= 0xCB && unit->unitType <= 0xD5)) &&
            unit->sprite != NULL &&
            !(unit->orderID == 0 && unit->orderState == 1)) // Moving/Following Order
        {
            //unknown_00488410(unit->unitType, unit->sprite); // push, push
        }
    //}
    return;
}

void Graphics::updateUnitOrderFinder(/* ? */)
{
    // !!! To do ?
    return;
}

void Graphics::toggleUnitPath(UnitNode* unit) // ESI unit
{
    // !!! To do ?
    return;
}

void Graphics::removeSelectionCircleAndHPBar(SpriteNode* sprite)
{
    // !!! To do ?
    return;
}

void Graphics::removeSelectionCircle(SpriteNode* sprite)
{
    // !!! To do ?
    return;
}

u8 Graphics::removeSelectionCircleFromSprite(SpriteNode* sprite)
{
    // !!! To do ?
    return 0;
}

void Graphics::ISCRIPT_CastSpell(UnitNode* unit, u8 weapon)
{
    //EAX = unit
    //ESI = unit
//  FireUnitWeapon(unit, weapon); // (ESI, push)
    if (chkd.scData.WeaponDat(weapon)->DamageFactor == 2)
    {
//      FireUnitWeapon(unit, weapon); // (ESI, push)
    }
    return;
}


void Graphics::editUnitFlags(UnitNode* unit, ChkUnit* chkUnit)
{
    // These don't change anything for us; more functions we don't need.
    // setUnitHP( stuff );
    // unit->shieldPoints = stuff;
    // setUnitEnergyEx(unit, cuwp->validflags, cuwp->energy);

    if (chkUnit->validFlags & 0x100000) // Has Resources
    {
        if (chkd.scData.UnitDat(unit->unitType)->SpecialAbilityFlags & 0x2000) // Resource Container
        {
            unit->resource.resourceCount = chkUnit->resources;
            if (unit->unitType >= 176 && unit->unitType <= 178) // Mineral Field 1, 2, 3
            {
                setResourceCount(unit);
            }
        }
    }

    //setHangerCount(unit, cuwp->validflags, cuwp->hangar); // (EDI, EAX, arg1)

    if(chkUnit->validFlags & chkUnit->stateFlags & 1) // Cloak
    {
    //  thing = getCloakingTech( stuff );
    //  if (TechUseAllowed( stuff, thing ))
    //  {
            AI_CloakUnit(unit);
    //  }
    }

    CHK_UNIT_ApplyBurrowFlag(unit, chkUnit->validFlags, chkUnit->stateFlags & 2); // Burrow

    CHK_UNIT_ApplyOtherFlags(unit, chkUnit->validFlags, chkUnit->stateFlags & 4); // In Transit

    if (chkUnit->validFlags & chkUnit->stateFlags & 8) { // Hallucination
            CHK_UNIT_FinalCreateStep(unit);
    }

    // These last two don't change much for us

    if (chkUnit->validFlags & chkUnit->stateFlags & 0x10) { // Invincible
        unit->statusFlags |= 0x4000000; // Invincible
    }

    if (unit->unitType == 40) { // Broodling
        u16 newTimer = 1800 + ((unit->statusFlags & 0x40000000) ? -450 : 0); // IsHallucination
        if (unit->status.removeTimer == 0 || unit->status.removeTimer < newTimer) {
            unit->status.removeTimer = newTimer;
        }
    }
    return;
}

void Graphics::setResourceCount(UnitNode* unit)
{
    u8 anim;
    if (unit->resource.resourceCount >= 750)
    {
        anim = ANIM_WORKINGTOIDLE;
    }
    else if (unit->resource.resourceCount >= 500)
    {
        anim = ANIM_ALMOSTBUILT;
    }
    else if (unit->resource.resourceCount >= 250)
    {
        anim = ANIM_SPECIALSTATE2;
    }
    else
    {
        anim = ANIM_SPECIALSTATE1;
    }
    if (anim != unit->resource.resourceIscript)
    {
        unit->resource.resourceIscript = anim;
        for (ImageNode* image = unit->sprite->pImageHead; image != NULL; image = image->next)
        {
            playImageIscript(image, anim);
        }
    }
    return;
}


void Graphics::AI_CloakUnit(UnitNode* unit)
{
    /*
    0049B2B0 / $ 8B87 DC000000  MOV EAX, DWORD PTR DS : [EDI + DC]
    0049B2B6 | .F6C4 01        TEST AH, 1
    0049B2B9 | . 75 62          JNZ SHORT StarCraf.0049B31D
    0049B2BB | . 53             PUSH EBX
    0049B2BC | . 56             PUSH ESI
    0049B2BD | . 8BC7           MOV EAX, EDI
    0049B2BF | .E8 CC64FFFF    CALL StarCraf.00491790; secondaryOrd_Cloak 00491790 00000071
    0049B2C4 | . 8B77 0C        MOV ESI, DWORD PTR DS : [EDI + C]
    0049B2C7 | . 33DB           XOR EBX, EBX
    0049B2C9 | . 53             PUSH EBX; / Arg2 = > 00000000
    0049B2CA | . 6A 03          PUSH 3; | Arg1 = 00000003
    0049B2CC | . 32C0           XOR AL, AL; |
    0049B2CE | .E8 FDCCFFFF    CALL StarCraf.00497FD0; \unknown_00497FD0
    0049B2D3 | . 8B97 DC000000  MOV EDX, DWORD PTR DS : [EDI + DC]
    0049B2D9 | . 8A8F A6000000  MOV CL, BYTE PTR DS : [EDI + A6]
    0049B2DF | . 81CA 00030000  OR EDX, 300
    0049B2E5 | .B0 6D          MOV AL, 6D
    0049B2E7 | . 3AC8           CMP CL, AL
    0049B2E9 | . 8997 DC000000  MOV DWORD PTR DS : [EDI + DC], EDX
    0049B2EF | . 74 20          JE SHORT StarCraf.0049B311
    0049B2F1 | . 8887 A6000000  MOV BYTE PTR DS : [EDI + A6], AL
    0049B2F7 | . 66:899F EA0000>MOV WORD PTR DS : [EDI + EA], BX
    0049B2FE | . 66:899F E80000>MOV WORD PTR DS : [EDI + E8], BX
    0049B305 | . 899F EC000000  MOV DWORD PTR DS : [EDI + EC], EBX
    0049B30B | . 889F E2000000  MOV BYTE PTR DS : [EDI + E2], BL
    0049B311 | > 5E             POP ESI
    0049B312 | .C787 E4000000 >MOV DWORD PTR DS : [EDI + E4], 80000000
    0049B31C | . 5B             POP EBX
    0049B31D  \> C3             RETN
    */
}


void Graphics::CHK_UNIT_ApplyBurrowFlag(UnitNode* unit, int validFlags, int applyFlags)
{
    if ((validFlags & 2) == 0) return; // Burrow
    if (applyFlags == 0) return; // Nothing to apply

    //if (TechUseAllowed(unit, 0xB, unit->playerID) == 1) // (EAX, EDI, push)
    //{
        for (ImageNode* image = unit->sprite->pImageHead; image != NULL; image = image->next)
        {
            playImageIscript(image, ANIM_SPECIALSTATE2);
        }
        //unknown_004E9A30(unit); // (EAX, ?)

        AI_CloakUnit(unit); // (EDI, ?)
        unit->statusFlags |= 0x200000; // NoCollide
        //orderComputer_cl(? , 0x75); // (?, CL)
    //}
    return;
}

void Graphics::CHK_UNIT_ApplyOtherFlags(UnitNode* unit, int validFlags, int applyFlags) {
    if ((validFlags & 4) == 0) return; // In Transit
    if (applyFlags == 0) return; // Nothing to apply

    /*(*p66FF60) = 0;
    if (((u16)*p665610) == 0xFFFF) {
        parseOrdersDatReqs(? ); // (EBX) ?
    }
    if (EBX != unit->playerID) {
        (*p66FF60) = 1;
        return;
    }

    if ((unit->statusFlags & 1) == 0) { // Completed
        (*p66FF60) = 0x14;
        return;
    }
    if ((unit->statusFlags & 0x400) || // DoodadStatesThing
        unit->status.lockdownTimer ||
        unit->status.stasisTimer ||
        unit->status.maelstromTimer) {
        (*p66FF60) = 0xA;
        return;
    }
    if ((unit->statusFlags & 0x10) && // Burrowed
        unit->unitType != 103) { // Lurker
        (*p66FF60) = 0xB;
        return;
    }
    if ((unit->unitType == 7 && unit->orderID == 33) || // SCV && isBuilding
        (unitIsGhost(unit) && unit->orderID == 129) || // Ghost && Nuke Track
        (unit->unitType == 68 && unit->orderID == 106) || // Archon && Completing Archon Summon
        (unit->unitType == 63 && unit->orderID == 106))   // Dark Archon && Completing Archon Summon
    {
        (*p66FF60) = 8;
        return;
    }

    if (!((u16)*p665610)) {
        (*p66FF60) = 0x17;
        return;
    }*/

    //if (parseRequirementOpcodes(0x48, unit->playerID, 0x00514CF8) == 1) { // (?, push, push, push)
        if (unit->sprite->pImageTail->paletteType == DRAW_SHADOW) {
            if (unit->sprite->pImageTail->verticalOffset != 0x2A) {
                unit->sprite->pImageTail->flags |= 1; // Redraw
                unit->sprite->pImageTail->verticalOffset = 0x2A;
            }
        }
        for (ImageNode* image = unit->sprite->pImageHead; image != NULL; image = image->next)
        {
            playImageIscript(image, ANIM_SPECIALSTATE1);
        }
        //updateUnitSorting(unit); // (EAX ?)
        //setPathingFromUnitElevation(unit); // (EAX ?)
                                           // EDX = unit->sprite
        unit->sprite->elevationLevel = 0xC;
        //removeUnitAirPathing(unit, 1); // (ECX, AL) ?
        //Unknown_0046A5A0(unit); // (EAX ?) -- update stuff
    //}
    return;
}

void Graphics::CHK_UNIT_FinalCreateStep(UnitNode* unit) {
    int i;
    //incrementUnitScores(unit, -1); // (EAX, push)
    //incrementUnitScoresEx(-1, 1); // (?, ECX, push)
    if (((unit->statusFlags & 0x400) && // DoodadStatesThing
        unit->status.lockdownTimer == 0 &&
        unit->status.stasisTimer == 0 &&
        unit->status.maelstromTimer == 0) ||
        (chkd.scData.UnitDat(unit->unitType)->SpecialAbilityFlags & 1)) { // Building
        unit->currentButtonSet = 0xF4;
    }
    unit->statusFlags |= 0x40000000; // IsHallucination
    //incrementUnitScores(unit, 1); // (EAX, push)
    //incrementUnitScoresEx(1, 0); // (?, ECX, push)
    //ECX = 1
    //EDX = unit->playerID
    //if ((*p6D0F14) != NULL /* EBX ? */ || unit->playerID == (*p512684)) {
    //  EAX = unit->sprite
    //  BL = 0
        setSpriteColoringData(unit->sprite, 0, true); // (EAX, BL)
    //}

    if (chkd.scData.UnitDat(unit->unitType)->SpecialAbilityFlags & 0x10) return; // Is SubUnit

    if (unit->statusFlags & 0x40000000)
    {
        i = 1350;
    }
    else
    {
        i = (unit->unitType == 40) ? 1800 : 0;
    }
    if (unit->status.removeTimer == 0 || i < unit->status.removeTimer)
    {
        unit->status.removeTimer = i;
    }
    //unit->airStrength = getUnitStrength(unit, 0); // (EAX, push)
    //unit->groundStrength = getUnitStrength(unit, 1); // (EAX, push)
    if (unit->secondaryOrderID != 149) { // Script (Hallucination)
        unit->secondaryOrderID = 149; // Script (Hallucination)
        unit->secondaryOrderPosition.y = 0;
        unit->secondaryOrderPosition.x = 0;
        unit->currentBuildUnit = 0;
        unit->secondaryOrderState = 0;
    }
    if (unit->subUnit != NULL) {
        CHK_UNIT_FinalCreateStep(unit->subUnit);
    }
    if (unit->unitType == 0x48 || // Carrier
        unit->unitType == 0x52 || // Gantrithor
        unit->unitType == 0x53 || // Reaver
        unit->unitType == 0x51)   // Warbringer
    {
        if (unit->unitType == 0x53 || // Reaver
            unit->unitType == 0x51)   // Warbringer
        {
            UnitNode* subunit = unit->carrier.pInHanger;
            for (i = unit->carrier.inHangerCount; i != 0; i--)
            {
                CHK_UNIT_FinalCreateStep(subunit); // (ECX)
                subunit = unit->carrier.pOutHanger;
            }
        }
        else
        {
            //killAllHangerUnits(unit); // (ESI)
        }
    }
    return;
}

void Graphics::CImage__updateGraphicData(ImageNode* image)
{
    if (image->frameIndex >= image->GRPFile->numFrames())
        image->frameIndex = 0;

    if (image->flags & 2)
    {
        image->mapPosition.x = image->spriteOwner->position.x + image->horizontalOffset + (image->GRPFile->GrpWidth() / 2) - image->GRPFile->frameWidth(image->frameIndex) - image->GRPFile->xOffset(image->frameIndex);
    }
    else
    {
        image->mapPosition.x = image->spriteOwner->position.x + image->horizontalOffset + (image->GRPFile->xOffset(image->frameIndex) - (image->GRPFile->GrpWidth() / 2));
    }
    if (!(image->flags & 4)) // 0x04 - Don't Calc Y?
    {
        image->mapPosition.y = image->GRPFile->yOffset(image->frameIndex) - (image->GRPFile->GrpHeight() / 2) + image->verticalOffset + image->spriteOwner->position.y;
    }

    image->grpBounds.left = 0;
    image->grpBounds.right = image->GRPFile->frameWidth(image->frameIndex);
    image->screenPosition.x = image->mapPosition.x - screenLeft;

    if (image->screenPosition.x < 0)
    {
        image->grpBounds.left = -image->screenPosition.x;
        image->grpBounds.right = image->GRPFile->frameWidth(image->frameIndex) + image->screenPosition.x;
        image->screenPosition.x = 0;
    }
    if (image->grpBounds.right >= ((s32)screenWidth - image->screenPosition.x)) // 640 = screen width
    {
        image->grpBounds.right = screenWidth - image->screenPosition.x;
    }

    image->grpBounds.top = 0;
    image->grpBounds.bottom = image->GRPFile->frameHeight(image->frameIndex);
    image->screenPosition.y = image->mapPosition.y - screenTop;
    if (image->screenPosition.y < 0)
    {
        image->grpBounds.top = -image->screenPosition.y;
        image->grpBounds.bottom = image->GRPFile->frameHeight(image->frameIndex) + image->screenPosition.y;
        image->screenPosition.y = 0;
    }
    if (image->grpBounds.bottom >= ((s32)screenHeight - image->screenPosition.y)) // 400 = screen height
    {
        image->grpBounds.bottom = screenHeight - image->screenPosition.y;
    }

    image->flags |= 1; // test

    return;
}

void Graphics::setAllOverlayDirectionsGeneric(SpriteNode* thingy, int direction) // CThingy
{
    for (ImageNode* image = thingy/*->sprite*/->pImageHead; image != NULL; image = image->next)
    {
        updateImageDirection(image, direction);
    }
    return;
}

void Graphics::image_Insert(ImageNode* image, ImageNode* newImage, ImageNode** pImageHead)
{
    if (*pImageHead == image)
    {
        *pImageHead = newImage;
    }
    newImage->prev = image->prev;
    newImage->next = image;
    if (image->prev != NULL)
    {
        image->prev->next = newImage;
    }
    image->prev = newImage;
    return;
}

void Graphics::somePlayImageCrapThatCrashes(ImageNode* image, SpriteNode* sprite, int x, int y, int imageid)
{
    InitializeImageData(image, sprite, imageid, x, y);
    unknown_004D66B0(image, sprite, imageid);
    return;
}

void Graphics::thg2SpecialDIsableUnit(UnitNode* unit)
{
    if (unit->statusFlags & USTATUS_DOODADSTATESTHING) return;
    if (unit->status.lockdownTimer) return;
    if (unit->status.stasisTimer) return;
    if (unit->status.maelstromTimer) return;
    unit->statusFlags |= USTATUS_DOODADSTATESTHING;
    for (ImageNode* image = unit->sprite->pImageHead; image != NULL; image = image->next) {
        playImageIscript(image, ANIM_ALMOSTBUILT);
    }
    unit->statusFlags |= USTATUS_NOCOLLIDE;
    if (unit->unitType == 203 || // Floor Missile Trap
        unit->unitType == 209 || // Floor Gun Trap
        unit->unitType == 205 || // Left Upper Level Door
        unit->unitType == 206 || // Right Upper Level Door
        unit->unitType == 207 || // Left Pit Door
        unit->unitType == 208) {  // Right Pit Door
        unit->sprite->elevationLevel = 1;
    }
    if (unit->unitType == 203 || // Floor Missile Trap
        unit->unitType == 209 || // Floor Gun Trap
        unit->unitType == 210 || // Left Wall Missile Trap
        unit->unitType == 211 || // Left Wall Flame Trap
        unit->unitType == 212 || // Right Wall Missile Trap
        unit->unitType == 213) {  // Right Wall Flame Trap
        unit->statusFlags |= USTATUS_REQUIRESDETECTION | USTATUS_CLOAKED;
        unit->visibilityStatus = 0x80000000;
        unit->secondaryOrderTimer = 0;
    }
}

void Graphics::SpriteDestructor(SpriteNode* sprite)
{
    ImageNode* next;
    for (ImageNode* image = sprite->pImageHead; image != NULL; image = next)
    {
        next = image->next;
        ImageDestructor(image);
    }

    // Remove sprite from sort tables or whatever
    // Remove from used sprite list
    /*
    00497B5F | > 0FBF47 16      MOVSX EAX, WORD PTR DS : [EDI + 16]
    00497B63 | . 99             CDQ
    00497B64 | . 83E2 1F        AND EDX, 1F
    00497B67 | . 03C2           ADD EAX, EDX
    00497B69 | .C1F8 05        SAR EAX, 5
    00497B6C | . 85C0           TEST EAX, EAX
    00497B6E | . 5E             POP ESI
    00497B6F | . 7D 04          JGE SHORT StarCraf.00497B75
    00497B71 | . 33C0           XOR EAX, EAX
    00497B73 | .EB 0E          JMP SHORT StarCraf.00497B83
    00497B75 | > 0FB70D D6F1570>MOVZX ECX, WORD PTR DS : [57F1D6]
    00497B7C | . 3BC1           CMP EAX, ECX
    00497B7E | . 7C 03          JL SHORT StarCraf.00497B83
    00497B80 | . 8D41 FF        LEA EAX, DWORD PTR DS : [ECX - 1]
    00497B83 | > 393C85 8896620>CMP DWORD PTR DS : [EAX * 4 + 629688], EDI
    00497B8A | . 75 0A          JNZ SHORT StarCraf.00497B96
    00497B8C | . 8B4F 04        MOV ECX, DWORD PTR DS : [EDI + 4]
    00497B8F | . 890C85 8896620>MOV DWORD PTR DS : [EAX * 4 + 629688], ECX
    00497B96 | > 393C85 8892620>CMP DWORD PTR DS : [EAX * 4 + 629288], EDI
    00497B9D | . 75 09          JNZ SHORT StarCraf.00497BA8
    00497B9F | . 8B17           MOV EDX, DWORD PTR DS : [EDI]
    00497BA1 | . 891485 8892620>MOV DWORD PTR DS : [EAX * 4 + 629288], EDX
    00497BA8 | > 8B07           MOV EAX, DWORD PTR DS : [EDI]
    00497BAA | . 85C0           TEST EAX, EAX
    00497BAC | . 74 06          JE SHORT StarCraf.00497BB4
    00497BAE | . 8B4F 04        MOV ECX, DWORD PTR DS : [EDI + 4]
    00497BB1 | . 8948 04        MOV DWORD PTR DS : [EAX + 4], ECX
    00497BB4 | > 8B47 04        MOV EAX, DWORD PTR DS : [EDI + 4]
    00497BB7 | . 85C0           TEST EAX, EAX
    00497BB9 | . 74 04          JE SHORT StarCraf.00497BBF
    00497BBB | . 8B17           MOV EDX, DWORD PTR DS : [EDI]
    00497BBD | . 8910           MOV DWORD PTR DS : [EAX], EDX
    00497BBF | > C707 00000000  MOV DWORD PTR DS : [EDI], 0
    00497BC5 | .C747 04 000000>MOV DWORD PTR DS : [EDI + 4], 0
    00497BCC | .A1 30FE6300    MOV EAX, DWORD PTR DS : [63FE30]
    00497BD1 | . 85C0           TEST EAX, EAX
    00497BD3 | . 74 23          JE SHORT StarCraf.00497BF8
    00497BD5 | . 3905 34FE6300  CMP DWORD PTR DS : [63FE34], EAX
    00497BDB | . 75 06          JNZ SHORT StarCraf.00497BE3
    00497BDD | . 893D 34FE6300  MOV DWORD PTR DS : [63FE34], EDI
    00497BE3 | > 8907           MOV DWORD PTR DS : [EDI], EAX
    00497BE5 | . 8B48 04        MOV ECX, DWORD PTR DS : [EAX + 4]
    00497BE8 | . 894F 04        MOV DWORD PTR DS : [EDI + 4], ECX
    00497BEB | . 8B48 04        MOV ECX, DWORD PTR DS : [EAX + 4]
    00497BEE | . 85C9           TEST ECX, ECX
    00497BF0 | . 74 02          JE SHORT StarCraf.00497BF4
    00497BF2 | . 8939           MOV DWORD PTR DS : [ECX], EDI
    00497BF4 | > 8978 04        MOV DWORD PTR DS : [EAX + 4], EDI
    00497BF7 | .C3             RETN
    00497BF8 | > 893D 34FE6300  MOV DWORD PTR DS : [63FE34], EDI
    00497BFE | . 893D 30FE6300  MOV DWORD PTR DS : [63FE30], EDI
    00497C04  \.C3             RETN
    */

    // Remove from used list
    if (sprite->prev != NULL) sprite->prev->next = sprite->next;
    if (sprite->next != NULL) sprite->next->prev = sprite->prev;
    if (sprite == spriteLastCreated) spriteLastCreated = sprite->prev;

    // Add to empty list
    sprite->prev = NULL;
    sprite->next = spriteEmptyNode;
    if (spriteEmptyNode != NULL) spriteEmptyNode->prev = sprite;
    spriteEmptyNode = sprite;
}

void Graphics::setSpriteColoringData(SpriteNode* sprite, u8 colordata, bool hallucination)
{
    for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
    {
        if (image->paletteType == DRAW_NORMAL || image->paletteType == DRAW_HALLUCINATION) // == Normal, == Shift
        {
            image->paletteType = hallucination ? DRAW_HALLUCINATION : DRAW_NORMAL;
            image->updateFunction = imgUpdateFxns[image->paletteType].func;
            if (image->flags & 2) // Flipped
            {
                image->renderFunction = imgRenderFxns[image->paletteType].flip;
            }
            else
            {
                image->renderFunction = imgRenderFxns[image->paletteType].norm;
            }
            image->flags |= 1; // Redraw
            image->coloringData.shiftRow = colordata;
        }
    }
    return;
}



void Graphics::updateAndDrawThingy(ChkdBitmap& bitmap, SpriteNode* sprite) // Thingy
{
    for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
    {
        CImage__updateGraphicData(image);
    }
    drawSprite(bitmap, sprite);
    for (ImageNode* image = sprite->pImageHead; image != NULL; image = image->next)
    {
        image->flags |= 1; // Redraw
    }
    return;
}

void Graphics::drawSprite(ChkdBitmap& bitmap, SpriteNode* sprite)
{
    activePlayerColor = sprite->playerID;
    
    for (int i = 0; i < 8; i ++) grpReindexing[i + 8] = chkd.scData.tunit.pcxDat.get<u8>(activePlayerColor * 8 + i);
    if (sprite->pImagePrimary != NULL)
    {
        updateTrans50PlayerColors(sprite->pImagePrimary->paletteType, sprite->playerID);
    }
    for (ImageNode* image = sprite->pImageTail; image != NULL; image = image->prev)
    {
        drawImage(bitmap, image);
    }
}

void Graphics::updateTrans50PlayerColors(u8 paletteType, u8 playerID) // unknownColorShiftSomething
{
    if (paletteType >= DRAW_CLOAK && paletteType <= DRAW_DECLOAK) // Is Cloaked ?
    {
        // Get pointer so we don't have this whole line, every time
        u8* trans50 = (u8*)chkd.scData.tilesets.set[map.getTileset()].remap[REMAP_CLOAK].pcxDat.getPtr(0);

        for (int i = 0; i < 8; i++) // For each color in the player color
        {
            memmove(&trans50[0x800 + 0x100 * i], // Store to player color row
                    &trans50[chkd.scData.tunit.pcxDat.get<u8>(playerID * 8 + i) * 0x100], // From tunit color's row
                    0x100);
        }
    }
}

void Graphics::drawImage(ChkdBitmap& bitmap, ImageNode* image)
{
    if ((image->flags & 0x40) == 0 && // Hidden/Invisible
        image->grpBounds.bottom > 0 &&
        image->grpBounds.right > 0 &&
        ((image->flags & 1) /*|| isImageRefreshable(image)*/)) // Is refreshable? Probably.
    {
        RECT grpBounds = { image->grpBounds.left, image->grpBounds.top, image->grpBounds.right, image->grpBounds.bottom };
        (this->*image->renderFunction)(bitmap, &chkd.scData.tilesets.set[map.getTileset()].wpe,
            image->screenPosition.x, image->screenPosition.y, image->GRPFile, image->frameIndex,
            &grpBounds, image->coloringData);
    }
    image->flags &= ~1; // Clear Redraw
    return;
}


u16 Graphics::getRand()
{
    randSeed *= 0x15A4E35;
    randSeed++;
    return (randSeed >> 0x10) & 0x7FFF;
}

void Graphics::updateCloaking(ImageNode* image)
{
    if (image->coloringData.cloak.timer) {
        image->coloringData.cloak.timer--;
        return;
    }
    image->coloringData.cloak.timer = 3;
    image->coloringData.cloak.state++;
    if (image->coloringData.cloak.state >= 8)
    {
        setImagePaletteType(image, image->paletteType + 1); // Go to next palette type, DRAW_OWN_CLOAK or DRAW_CLOAKED
        activeIscriptUnit->statusFlags |= USTATUS_REQUIRESDETECTION | USTATUS_CLOAKED;
    }
    image->flags |= 1; // Redraw
    return;
}




void Graphics::updateDecloaking(ImageNode* image) {
    if (image->coloringData.cloak.timer) {
        image->coloringData.cloak.timer--;
        return;
    }
    image->coloringData.cloak.timer = 3;
    image->coloringData.cloak.state--;
    if (image->coloringData.cloak.state == 0)
    {
        image->paletteType = DRAW_NORMAL;
        image->updateFunction = imgUpdateFxns[DRAW_NORMAL].func;
        if (image->flags & 2) // Flipped
        {
            image->renderFunction = imgRenderFxns[DRAW_NORMAL].flip; // RenderFunc[0].flip;
        }
        else {
            image->renderFunction = imgRenderFxns[DRAW_NORMAL].norm; // RenderFunc[1].norm;
        }
        image->flags |= 1; // Redraw
        activeIscriptUnit->statusFlags &= ~(USTATUS_REQUIRESDETECTION | USTATUS_CLOAKED);
    }
    image->flags |= 1; // Redraw
    return;
}

void Graphics::updateWarpFlash(ImageNode* image)
{
    if (image->coloringData.warpFlash.timer != 0)
    {
        image->coloringData.warpFlash.timer--;
        return;
    }
    image->coloringData.warpFlash.timer = 2;
    if (image->coloringData.warpFlash.state < 0x3F)
    {
        image->coloringData.warpFlash.state++;
        image->flags |= 1; // Redraw
        return;
    }
    if (image->anim != ANIM_DEATH)
    {
        image->anim = ANIM_DEATH;
        image->sleep = 0;
        image->iscriptReturn = 0;
        image->iscriptOffset = chkd.scData.iScripts.GetAnimOffset(image->iscriptHeader, ANIM_DEATH, false);
        iscript_OpcodeCases(image, image->iscriptHeader, 0, 0);
    }
    unk_unit_6D11F4->orderSignal |= 1;
    image->flags |= 1; // Redraw
    return;
}

void Graphics::imageRenderFxn0_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Normal draw -- Writes the decompressed bytes through grpReindexing[], set with the active player color, to the bitmap
{
    u32 bitmapIndex = screenWidth * y + x;

    u8* lineDat;
    u8 compSect;

    u32 lineOffs = screenWidth - grpRect->right;
    s32 linesLeft = grpRect->bottom - 1;
    s32 drawLeft, drawRight;
    u32 line = grpRect->top;

    while (linesLeft > 0)
    {
        lineDat = grp->data(frame, line);
        line++;
        drawLeft = grpRect->left;
        drawRight = grpRect->right;
        while (drawLeft != 0) // Bytes to skip -- left of drawing rect
        {
            compSect = *lineDat;
            lineDat++;
            if (compSect & 0x80)
            {
                compSect &= ~0x80;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                bitmapIndex -= drawLeft;
                drawRight += drawLeft;
                break; // Bytes remaining to draw -- go to loop below
            }
            else if (compSect & 0x40)
            {
                compSect &= ~0x40;
                lineDat++;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                drawLeft = -drawLeft;
                lineDat--;
                compSect = drawLeft;
                // jump down
                drawRight -= compSect;
                if (drawRight < 0)
                {
                    compSect += drawRight;
                }
                do
                {
                    bitmap[bitmapIndex] = palette->get<u32>(grpReindexing[*lineDat] * 4);
                    bitmapIndex++;
                    compSect--;
                } while (compSect > 0);
                lineDat++;
                break; // Bytes remaining to draw -- go to loop below
            }
            else
            {
                lineDat += compSect;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                drawLeft = -drawLeft;
                lineDat -= drawLeft;
                compSect = drawLeft;
                // jump down
                drawRight -= compSect;
                if (drawRight < 0)
                {
                    compSect += drawRight;
                }
                do
                {
                    bitmap[bitmapIndex] = palette->get<u32>(grpReindexing[*lineDat] * 4);
                    lineDat++;
                    bitmapIndex++;
                    compSect--;
                } while (compSect > 0);
                break; // Bytes remaining to draw -- go to loop below
            }
        }
        if (drawRight <= 0) { // Bytes remaining to skip
            bitmapIndex += drawRight;
        }
        else
        {
            while (drawRight > 0)
            {
                compSect = *lineDat;
                lineDat++;
                if (compSect & 0x80)
                {
                    compSect &= ~0x80;
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    bitmapIndex += compSect;
                }
                else if (compSect & 0x40)
                {
                    compSect &= ~0x40;
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    do
                    {
                        bitmap[bitmapIndex] = palette->get<u32>(grpReindexing[*lineDat] * 4);
                        bitmapIndex++;
                        compSect--;
                    } while (compSect > 0);
                    lineDat++;
                }
                else
                {
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    do
                    {
                        bitmap[bitmapIndex] = palette->get<u32>(grpReindexing[*lineDat] * 4);
                        lineDat++;
                        bitmapIndex++;
                        compSect--;
                    } while (compSect > 0);
                }
            }
        }
        bitmapIndex += lineOffs;
        linesLeft--;
    }
}

void Graphics::imageRenderFxn2_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Unvisioned Cloaking / Decloaking Draw -- Draws unvisioned cloak effect based on GRP, and then parts of the normal GRP based on the GRP through cloakingTable
{
    imageRenderFxn3_0(bitmap, palette, x, y, grp, frame, grpRect, (u32)0);
    imageRenderFxn5_2__0_common(bitmap, palette, x, y, grp, frame, grpRect, colorData);
}

void Graphics::imageRenderFxn2_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Flipped Unvisioned Cloaking / Decloaking Draw
{
    //imageRenderFxn3_1(bitmap, palette, x, y, grp, frame, grpRect, (u32)0);
    //imageRenderFxn5_2__1_common(bitmap, palette, x, y, grp, frame, grpRect, colorData);
}

void Graphics::imageRenderFxn3_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Unvisioned Cloaked Effect -- Copies drawbuffer values some pixels away based on the cloakingTable values
{
    u32 bitmapIndex = screenWidth * y + x;
    u32 maxIndex = screenWidth * screenHeight;
    u32 effect;

    u8* lineDat;
    u8 compSect;

    u32 lineOffs = screenWidth - grpRect->right;
    s32 linesLeft = grpRect->bottom - 1;
    s32 drawLeft, drawRight;
    u32 line = grpRect->top;

    while (linesLeft > 0)
    {
        lineDat = grp->data(frame, line);
        line++;
        drawLeft = grpRect->left;
        drawRight = grpRect->right;
        while (drawLeft != 0) // Bytes to skip -- left of drawing rect
        {
            compSect = *lineDat;
            lineDat++;
            if (compSect & 0x80)
            {
                compSect &= ~0x80;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                bitmapIndex -= drawLeft;
                drawRight += drawLeft;
                break; // Bytes remaining to draw -- go to loop below
            }
            else if (compSect & 0x40)
            {
                compSect &= ~0x40;
                lineDat++;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                drawLeft = -drawLeft;
                lineDat--;
                compSect = drawLeft;
                // jump down
                drawRight -= compSect;
                if (drawRight < 0)
                {
                    compSect += drawRight;
                }
                effect = bitmapIndex + cloakingTable[*lineDat];
                lineDat++;
                do
                {
                    if (effect >= maxIndex)
                    {
                        effect -= maxIndex;
                    }
                    bitmap[bitmapIndex] = bitmap[effect];
                    effect++;
                    bitmapIndex++;
                    compSect--;
                } while (compSect > 0);
                break; // Bytes remaining to draw -- go to loop below
            }
            else
            {
                lineDat += compSect;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                drawLeft = -drawLeft;
                lineDat -= drawLeft;
                compSect = drawLeft;
                // jump down
                drawRight -= compSect;
                if (drawRight < 0)
                {
                    compSect += drawRight;
                }
                do
                {
                    effect = bitmapIndex + cloakingTable[*lineDat];
                    lineDat++;
                    if (effect >= maxIndex)
                    {
                        effect -= maxIndex;
                    }
                    bitmap[bitmapIndex] = bitmap[effect];
                    bitmapIndex--;
                    compSect--;
                } while (compSect > 0);
                break; // Bytes remaining to draw -- go to loop below
            }
        }
        if (drawRight <= 0) { // Bytes remaining to skip
            bitmapIndex += drawRight;
        }
        else
        {
            while (drawRight > 0)
            {
                compSect = *lineDat;
                lineDat++;
                if (compSect & 0x80)
                {
                    compSect &= ~0x80;
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    bitmapIndex += compSect;
                }
                else if (compSect & 0x40)
                {
                    compSect &= ~0x40;
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    effect = bitmapIndex + cloakingTable[*lineDat];
                    lineDat++;
                    do
                    {
                        if (effect >= maxIndex)
                        {
                            effect -= maxIndex;
                        }
                        bitmap[bitmapIndex] = bitmap[effect];
                        effect++;
                        bitmapIndex++;
                        compSect--;
                    } while (compSect > 0);
                }
                else
                {
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    do
                    {
                        effect = bitmapIndex + cloakingTable[*lineDat];
                        lineDat++;
                        if (effect >= maxIndex)
                        {
                            effect -= maxIndex;
                        }
                        bitmap[bitmapIndex] = bitmap[effect];
                        bitmapIndex--;
                        compSect--;
                    } while (compSect > 0);
                }
            }
        }
        bitmapIndex += lineOffs;
        linesLeft--;
    }
}

void Graphics::imageRenderFxn5_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Visioned Cloaking/Decloaking Draw -- Draws trans50 effect with GRP, then parts of GRP based on cloakingTable
{
    imageRenderFxn9_0(bitmap, palette, x, y, grp, frame, grpRect, &chkd.scData.tilesets.set[map.getTileset()].remap[REMAP_CLOAK - 1]);
    imageRenderFxn5_2__0_common(bitmap, palette, x, y, grp, frame, grpRect, colorData);
}


void Graphics::imageRenderFxn5_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Flipped Visioned Cloaking/Decloaking Draw
{
    //imageRenderFxn9_1(bitmap, palette, x, y, grp, frame, grpRect, &chkd.scData.tilesets.set[chk.getTileset()].remap[REMAP_CLOAK - 1]);
    //imageRenderFxn5_2__1_common(bitmap, palette, x, y, grp, frame, grpRect, colorData);
}

void Graphics::imageRenderFxn6_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Visioned Cloaked Draw -- Draws GRP with trans50 remapping table
{
    imageRenderFxn9_0(bitmap, palette, x, y, grp, frame, grpRect, &chkd.scData.tilesets.set[map.getTileset()].remap[REMAP_CLOAK - 1]);
}

void Graphics::imageRenderFxn6_1(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// FLipped Visioned Cloaked Draw
{
    //imageRenderFxn9_1(bitmap, palette, x, y, grp, frame, grpRect, &chkd.scData.tilesets.set[chk.getTileset()].remap[REMAP_CLOAK - 1]);
}

void Graphics::imageRenderFxn8_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// EMP effect -- Copies the color some pixels away based on the decompressed value
{
    u32 bitmapIndex = screenWidth * y + x;
    u32 maxIndex = screenWidth * screenHeight;
    u32 effect;

    u8* lineDat;
    u8 compSect;

    u32 lineOffs = screenWidth - grpRect->right;
    s32 linesLeft = grpRect->bottom - 1;
    s32 drawLeft, drawRight;
    u32 line = grpRect->top;

    while (linesLeft > 0)
    {
        lineDat = grp->data(frame, line);
        line++;
        drawLeft = grpRect->left;
        drawRight = grpRect->right;
        while (drawLeft != 0) // Bytes to skip -- left of drawing rect
        {
            compSect = *lineDat;
            lineDat++;
            if (compSect & 0x80)
            {
                compSect &= ~0x80;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                bitmapIndex -= drawLeft;
                drawRight += drawLeft;
                break; // Bytes remaining to draw -- go to loop below
            }
            else if (compSect & 0x40)
            {
                compSect &= ~0x40;
                lineDat++;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                drawLeft = -drawLeft;
                lineDat--;
                compSect = drawLeft;
                // jump down
                drawRight -= compSect;
                if (drawRight < 0)
                {
                    compSect += drawRight;
                }
                effect = bitmapIndex + *lineDat;
                lineDat++;
                do
                {
                    if (effect >= maxIndex)
                    {
                        effect -= maxIndex;
                    }
                    bitmap[bitmapIndex] = bitmap[effect];
                    effect++;
                    bitmapIndex++;
                    compSect--;
                } while (compSect > 0);
                break; // Bytes remaining to draw -- go to loop below
            }
            else
            {
                lineDat += compSect;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                drawLeft = -drawLeft;
                lineDat -= drawLeft;
                compSect = drawLeft;
                // jump down
                drawRight -= compSect;
                if (drawRight < 0)
                {
                    compSect += drawRight;
                }
                do
                {
                    effect = bitmapIndex + *lineDat;
                    lineDat++;
                    if (effect >= maxIndex)
                    {
                        effect -= maxIndex;
                    }
                    compSect--;
                    bitmap[bitmapIndex] = bitmap[effect];
                    bitmapIndex++;
                } while (compSect > 0);
                break; // Bytes remaining to draw -- go to loop below
            }
        }
        if (drawRight <= 0) { // Bytes remaining to skip
            bitmapIndex += drawRight;
        }
        else
        {
            while (drawRight > 0)
            {
                compSect = *lineDat;
                lineDat++;
                if (compSect & 0x80)
                {
                    compSect &= ~0x80;
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    bitmapIndex += compSect;
                }
                else if (compSect & 0x40)
                {
                    compSect &= ~0x40;
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    effect = bitmapIndex + *lineDat;
                    lineDat++;
                    do
                    {
                        if (effect >= maxIndex)
                        {
                            effect -= maxIndex;
                        }
                        bitmap[bitmapIndex] = bitmap[effect];
                        effect++;
                        bitmapIndex++;
                        compSect--;
                    } while (compSect > 0);
                }
                else
                {
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    do
                    {
                        effect = bitmapIndex + *lineDat;
                        lineDat++;
                        if (effect >= maxIndex)
                        {
                            effect -= maxIndex;
                        }
                        compSect--;
                        bitmap[bitmapIndex] = bitmap[effect];
                        bitmapIndex++;
                    } while (compSect > 0);
                }
            }
        }
        bitmapIndex += lineOffs;
        linesLeft--;
    }
}

void Graphics::imageRenderFxn9_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Remapping function -- Looks up a new color based on the palette index of the bitmap coordinate being written to and the decompressed value
{
    u32 bitmapIndex = screenWidth * y + x;
    //u32 maxIndex = screenWidth * screenHeight;
    u32 effect;

    u8* lineDat;
    u8 compSect;

    u32 lineOffs = screenWidth - grpRect->right;
    s32 linesLeft = grpRect->bottom - 1;
    s32 drawLeft, drawRight;
    u32 line = grpRect->top;

    while (linesLeft > 0)
    {
        lineDat = grp->data(frame, line);
        line++;
        drawLeft = grpRect->left;
        drawRight = grpRect->right;
        while (drawLeft != 0) // Bytes to skip -- left of drawing rect
        {
            compSect = *lineDat;
            lineDat++;
            if (compSect & 0x80)
            {
                compSect &= ~0x80;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                bitmapIndex -= drawLeft;
                drawRight += drawLeft;
                break; // Bytes remaining to draw -- go to loop below
            }
            else if (compSect & 0x40)
            {
                compSect &= ~0x40;
                lineDat++;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                drawLeft = -drawLeft;
                lineDat--;
                compSect = drawLeft;
                // jump down
                drawRight -= compSect;
                if (drawRight < 0)
                {
                    compSect += drawRight;
                }
                do
                {
                    effect = ((*lineDat - 1) << 8) | ((bitmap[bitmapIndex] & 0xFF000000) >> 24);
                    bitmap[bitmapIndex] = palette->get<u32>(colorData.effect->pcxDat.get<u8>(effect) * 4);
                    bitmapIndex++;
                    compSect--;
                } while (compSect > 0);
                lineDat++;
                break; // Bytes remaining to draw -- go to loop below
            }
            else
            {
                lineDat += compSect;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                drawLeft = -drawLeft;
                lineDat -= drawLeft;
                compSect = drawLeft;
                // jump down
                drawRight -= compSect;
                if (drawRight < 0)
                {
                    compSect += drawRight;
                }
                do
                {
                    effect = ((*lineDat - 1) << 8) | ((bitmap[bitmapIndex] & 0xFF000000) >> 24);
                    bitmap[bitmapIndex] = palette->get<u32>(colorData.effect->pcxDat.get<u8>(effect) * 4);
                    lineDat++;
                    bitmapIndex++;
                    compSect--;
                } while (compSect > 0);
                break; // Bytes remaining to draw -- go to loop below
            }
        }
        if (drawRight <= 0) { // Bytes remaining to skip
            bitmapIndex += drawRight;
        }
        else
        {
            while (drawRight > 0)
            {
                compSect = *lineDat;
                lineDat++;
                if (compSect & 0x80)
                {
                    compSect &= ~0x80;
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    bitmapIndex += compSect;
                }
                else if (compSect & 0x40)
                {
                    compSect &= ~0x40;
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    do
                    {
                        effect = ((*lineDat - 1) << 8) | ((bitmap[bitmapIndex] & 0xFF000000) >> 24);
                        bitmap[bitmapIndex] = palette->get<u32>(colorData.effect->pcxDat.get<u8>(effect) * 4);
                        bitmapIndex++;
                        compSect--;
                    } while (compSect > 0);
                    lineDat++;
                }
                else
                {
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    do
                    {
                        effect = ((*lineDat - 1) << 8) | ((bitmap[bitmapIndex] & 0xFF000000) >> 24);
                        bitmap[bitmapIndex] = palette->get<u32>(colorData.effect->pcxDat.get<u8>(effect) * 4);
                        lineDat++;
                        bitmapIndex++;
                        compSect--;
                    } while (compSect > 0);
                }
            }
        }
        bitmapIndex += lineOffs;
        linesLeft--;
    }
}

void Graphics::imageRenderFxn10_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Shadow -- Draws a shaded color based on the existing color index of the pixel being written to
{
    u32 bitmapIndex = screenWidth * y + x;
    u32 effect;

    buffer& dark = chkd.scData.tilesets.set[map.getTileset()].dark.pcxDat;

    u8* lineDat;
    u8 compSect;

    u32 lineOffs = screenWidth - grpRect->right;
    s32 linesLeft = grpRect->bottom - 1;
    s32 drawLeft, drawRight;
    u32 line = grpRect->top;

    while (linesLeft > 0)
    {
        lineDat = grp->data(frame, line);
        line++;
        drawLeft = grpRect->left;
        drawRight = grpRect->right;
        while (drawLeft != 0) // Bytes to skip -- left of drawing rect
        {
            compSect = *lineDat;
            lineDat++;
            if (compSect & 0x80)
            {
                compSect &= ~0x80;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                bitmapIndex -= drawLeft;
                drawRight += drawLeft;
                break; // Bytes remaining to draw -- go to loop below
            }
            else if (compSect & 0x40)
            {
                compSect &= ~0x40;
                lineDat++;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                drawLeft = -drawLeft;
                lineDat--;
                compSect = drawLeft;
                // jump down
                drawRight -= compSect;
                if (drawRight < 0)
                {
                    compSect += drawRight;
                }
                do
                {
                    effect = (18 << 8) | ((bitmap[bitmapIndex] & 0xFF000000) >> 24);
                    bitmap[bitmapIndex] = palette->get<u32>(dark.get<u8>(effect) * 4);
                    bitmapIndex++;
                    compSect--;
                } while (compSect > 0);
                lineDat++;
                break; // Bytes remaining to draw -- go to loop below
            }
            else
            {
                lineDat += compSect;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                drawLeft = -drawLeft;
                lineDat -= drawLeft;
                compSect = drawLeft;
                // jump down
                drawRight -= compSect;
                if (drawRight < 0)
                {
                    compSect += drawRight;
                }
                do
                {
                    effect = (18 << 8) | ((bitmap[bitmapIndex] & 0xFF000000) >> 24);
                    bitmap[bitmapIndex] = palette->get<u32>(dark.get<u8>(effect) * 4);
                    lineDat++;
                    bitmapIndex++;
                    compSect--;
                } while (compSect > 0);
                break; // Bytes remaining to draw -- go to loop below
            }
        }
        if (drawRight <= 0) { // Bytes remaining to skip
            bitmapIndex += drawRight;
        }
        else
        {
            while (drawRight > 0)
            {
                compSect = *lineDat;
                lineDat++;
                if (compSect & 0x80)
                {
                    compSect &= ~0x80;
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    bitmapIndex += compSect;
                }
                else if (compSect & 0x40)
                {
                    compSect &= ~0x40;
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    do
                    {
                        effect = (18 << 8) | ((bitmap[bitmapIndex] & 0xFF000000) >> 24);
                        bitmap[bitmapIndex] = palette->get<u32>(dark.get<u8>(effect) * 4);
                        bitmapIndex++;
                        compSect--;
                    } while (compSect > 0);
                    lineDat++;
                }
                else
                {
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    do
                    {
                        effect = (18 << 8) | ((bitmap[bitmapIndex] & 0xFF000000) >> 24);
                        bitmap[bitmapIndex] = palette->get<u32>(dark.get<u8>(effect) * 4);
                        lineDat++;
                        bitmapIndex++;
                        compSect--;
                    } while (compSect > 0);
                }
            }
        }
        bitmapIndex += lineOffs;
        linesLeft--;
    }
}

void Graphics::imageRenderFxn13_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Selection Circle -- calls Normal Draw, but with a selection circle color reindexing applied
{
    u8 tselectPrev[8];
    for (int i = 0; i < 8; i++)
    {
        tselectPrev[i] = grpReindexing[i + 1]; // Back up old tselect reindexing values
        grpReindexing[i + 1] = chkd.scData.tselect.pcxDat.get<u8>(colorData.selectColor * 8 + i); // Load tselect reindex value
    }
    imageRenderFxn0_0(bitmap, palette, x, y, grp, frame, grpRect, (u32)0);
    for (int i = 0; i < 8; i++)
    {
        grpReindexing[i + 1] = tselectPrev[i]; // Restore saved tselect reindexing values
    }
}

void Graphics::imageRenderFxn14_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Draw with Player Color -- calls Normal Draw, but with a specified player color applied
{
    u8 prevColor = activePlayerColor;

    // Load player color reindexing values
    activePlayerColor = colorData.playerColor; // Normally calls a function, but it just changes the player color based on minimap state
    for (int i = 0; i < 8; i++)
    {
        grpReindexing[i + 1] = chkd.scData.tunit.pcxDat.get<u8>(activePlayerColor * 8 + i); // Load tunit reindex value
    }

    imageRenderFxn0_0(bitmap, palette, x, y, grp, frame, grpRect, (u32)0);

    // Load previous player color values
    activePlayerColor = prevColor;
    for (int i = 0; i < 8; i++)
    {
        grpReindexing[i + 1] = chkd.scData.tunit.pcxDat.get<u8>(activePlayerColor * 8 + i); // Load tunit reindex value
    }
}

void Graphics::imageRenderFxn16_0(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Hallucination Draw -- Fills grpReindexing table with shift.pcx data, then calls Normal Draw
{
    u8 tmp[0x100];
    PCX* shift = &chkd.scData.tilesets.set[map.getTileset()].shift;
    for (int i = 0; i < 0x100; i++)
    {
        tmp[i] = grpReindexing[i];
        grpReindexing[i] = shift->pcxDat.get<u8>(colorData.shiftRow << 8 + i); // shift.pcx only has one row, and this doesn't ever seem to be set > 0 -- But maybe mods can change it.
    }
    imageRenderFxn0_0(bitmap, palette, x, y, grp, frame, grpRect, (u32)0);
    for (int i = 0; i < 0x100; i++) grpReindexing[i] = tmp[i];
}


void Graphics::imageRenderFxn5_2__0_common(ChkdBitmap& bitmap, buffer* palette, s32 x, s32 y, GRP *grp, u16 frame, RECT *grpRect, ColoringData colorData)
// Unvisioned cloaking/decloaking common -- Draws the normal-colored components of GRP based on cloakingTable, making parts transparent
{
    u32 bitmapIndex = screenWidth * y + x;

    u8* lineDat;
    u8 compSect;

    u32 lineOffs = screenWidth - grpRect->right;
    s32 linesLeft = grpRect->bottom - 1;
    s32 drawLeft, drawRight;
    u32 line = grpRect->top;

    while (linesLeft > 0)
    {
        lineDat = grp->data(frame, line);
        line++;
        drawLeft = grpRect->left;
        drawRight = grpRect->right;
        while (drawLeft != 0) // Bytes to skip -- left of drawing rect
        {
            compSect = *lineDat;
            lineDat++;
            if (compSect & 0x80)
            {
                compSect &= ~0x80;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                bitmapIndex -= drawLeft;
                drawRight += drawLeft;
                break; // Bytes remaining to draw -- go to loop below
            }
            else if (compSect & 0x40)
            {
                compSect &= ~0x40;
                lineDat++;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                drawLeft = -drawLeft;
                lineDat--;
                compSect = drawLeft;
                // jump down
                drawRight -= compSect;
                if (drawRight < 0)
                {
                    compSect += drawRight;
                }
                if (cloakingTable[grpReindexing[*lineDat]] <= colorData.cloak.state)
                {
                    bitmapIndex += compSect;
                }
                else
                {
                    do
                    {
                        bitmap[bitmapIndex] = grpReindexing[*lineDat];
                        bitmapIndex++;
                        compSect--;
                    } while (compSect > 0);
                }
                lineDat++;
                break; // Bytes remaining to draw -- go to loop below
            }
            else
            {
                lineDat += compSect;
                drawLeft -= compSect;
                if (drawLeft == 0) break;
                if (drawLeft >= 0) continue;
                drawLeft = -drawLeft;
                lineDat -= drawLeft;
                compSect = drawLeft;
                // jump down
                drawRight -= compSect;
                if (drawRight < 0)
                {
                    compSect += drawRight;
                }
                do
                {
                    if (cloakingTable[grpReindexing[*lineDat]] > colorData.cloak.state)
                    {
                        bitmap[bitmapIndex] = grpReindexing[*lineDat];
                    }
                    lineDat++;
                    bitmapIndex++;
                    compSect--;
                } while (compSect > 0);
                    break; // Bytes remaining to draw -- go to loop below
            }
        }
        if (drawRight <= 0) { // Bytes remaining to skip
            bitmapIndex += drawRight;
        }
        else
        {
            while (drawRight > 0)
            {
                compSect = *lineDat;
                lineDat++;
                if (compSect & 0x80)
                {
                    compSect &= ~0x80;
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }
                    bitmapIndex += compSect;
                }
                else if (compSect & 0x40)
                {
                    compSect &= ~0x40;
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }

                    if (cloakingTable[grpReindexing[*lineDat]] <= colorData.cloak.state)
                    {
                        bitmapIndex += compSect;
                    }
                    else
                    {
                        do
                        {
                            bitmap[bitmapIndex] = grpReindexing[*lineDat];
                            bitmapIndex++;
                            compSect--;
                        } while (compSect > 0);
                    }

                    lineDat++;
                }
                else
                {
                    drawRight -= compSect;
                    if (drawRight < 0)
                    {
                        compSect += drawRight;
                    }

                    do
                    {
                        if (cloakingTable[grpReindexing[*lineDat]] > colorData.cloak.state)
                        {
                            bitmap[bitmapIndex] = grpReindexing[*lineDat];
                        }
                        lineDat++;
                        bitmapIndex++;
                        compSect--;
                    } while (compSect > 0);

                }
            }
        }
        bitmapIndex += lineOffs;
        linesLeft--;
    }
}
