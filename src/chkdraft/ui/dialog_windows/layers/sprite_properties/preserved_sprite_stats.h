#ifndef PRESERVEDSPRITESTATS_H
#define PRESERVEDSPRITESTATS_H
#include <common_files/common_files.h>
#include <mapping_core/mapping_core.h>
#include "mapping/selections.h"

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