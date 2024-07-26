#ifndef PRESERVEDSPRITESTATS_H
#define PRESERVEDSPRITESTATS_H
#include "../../../../CommonFiles/CommonFiles.h"
#include "../../../../../MappingCoreLib/MappingCore.h"
#include "../../../../Mapping/Selections.h"

class PreservedSpriteStats
{
    public:
        PreservedSpriteStats();
        virtual ~PreservedSpriteStats();
        void Clear();
        void AddStats(Selections & sel, Chk::Sprite::Field statField);
        void convertToUndo();

    private:
        Chk::Sprite::Field field;
        std::vector<u32> preservedValues;
};

#endif