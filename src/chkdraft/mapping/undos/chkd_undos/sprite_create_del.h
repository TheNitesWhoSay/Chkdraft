#ifndef SPRITECREATEDEL_H
#define SPRITECREATEDEL_H
#include "mapping/undos/reversibles.h"
#include "undo_types.h"

class SpriteCreateDel : public ReversibleAction
{
    public:
        virtual ~SpriteCreateDel();
        static std::shared_ptr<SpriteCreateDel> Make(size_t index); // Undo Creation
        static std::shared_ptr<SpriteCreateDel> Make(size_t index, const Chk::Sprite & sprite); // Undo Deletion
        virtual void Reverse(void *guiMap);
        virtual int32_t GetType();
    
    protected:
        SpriteCreateDel(size_t index); // Undo Creation
        SpriteCreateDel(size_t index, const Chk::Sprite & sprite); // Undo Deletion

    private:
        size_t index;
        std::unique_ptr<Chk::Sprite> sprite; // If creation, is a nullptr
};

#endif