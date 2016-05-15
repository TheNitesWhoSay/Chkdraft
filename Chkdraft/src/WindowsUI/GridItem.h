#ifndef GRIDITEM_H
#define GRIDITEM_H
#include "DataTypes.h"

class GridControlItem
{
    public:
        GridControlItem();
        void SetText(const char* text);
        void SetText(int text);
        void SetData(u32 data);
        void SetSelected(bool selected);
        void SetDisabled(bool disabled);
        void Clear();

        bool getText(std::string &text);
        int getTextLength();
        int getTextNum();
        u32 getData();
        bool isSelected();
        bool isDisabled();

    protected:

    private:
        std::string text;
        bool disabled;
        bool selected;
        u32 data;
};

#endif