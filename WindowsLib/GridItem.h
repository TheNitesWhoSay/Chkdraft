#ifndef GRIDITEM_H
#define GRIDITEM_H
#include "DataTypes.h"
#include <string>

namespace WinLib {

    class GridControlItem
    {
        public:
            GridControlItem();
            virtual ~GridControlItem();
            void SetText(const std::string & text);
            void SetText(int text);
            void SetData(u32 data);
            void SetSelected(bool selected);
            void SetDisabled(bool disabled);
            void Clear();

            bool getText(std::string & text);
            size_t getTextLength();
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

}

#endif