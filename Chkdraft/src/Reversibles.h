#ifndef REVERSIBLES_H
#define REVERSIBLES_H
#include "CommonFiles/CommonFiles.h"
#include <vector>

class Reversible
{
    public:
        virtual void Reverse(void *obj) = 0;
        virtual int32_t GetType() = 0;
        virtual int32_t Count() = 0;
};

using ReversiblePtr = std::shared_ptr<Reversible>;

class ReversibleAction : public Reversible
{
    public:
        virtual void Reverse(void *obj) = 0;
        virtual int32_t GetType() = 0;
        virtual int32_t Count();
};

class ReversibleActions : public Reversible
{
    public:
        static std::shared_ptr<ReversibleActions> Make();
        virtual void Reverse(void *obj);
        virtual int32_t GetType(); // Returns 0 unless overidden
        virtual int32_t Count();
        virtual void Insert(std::shared_ptr<Reversible> action);

    protected:
        ReversibleActions();

    private:
        std::vector<std::shared_ptr<Reversible>> actions;
        bool reversed;
};

#endif