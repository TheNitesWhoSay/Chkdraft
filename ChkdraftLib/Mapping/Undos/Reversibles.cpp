#include "Reversibles.h"

Reversible::~Reversible()
{

}

ReversibleAction::~ReversibleAction()
{

}

void ReversibleAction::Reverse(void *obj)
{

}

int32_t ReversibleAction::Count()
{
    return 1;
}

ReversibleActions::~ReversibleActions()
{

}

std::shared_ptr<ReversibleActions> ReversibleActions::Make()
{
    return std::shared_ptr<ReversibleActions>(new ReversibleActions);
}

void ReversibleActions::Reverse(void *obj)
{
    if ( reversed )
    {
        for ( auto it = actions.rbegin(); it != actions.rend(); ++it )
            (*it)->Reverse(obj);
    }
    else
    {
        for ( auto &action : actions )
            action->Reverse(obj);
    }
    reversed = !reversed;
}

int32_t ReversibleActions::GetType()
{
    for ( auto &action : actions )
    {
        int32_t type = action->GetType();
        if ( type != 0 )
            return type;
    }
    return 0;
}

int32_t ReversibleActions::Count()
{
    return (int32_t)actions.size();
}

void ReversibleActions::Insert(std::shared_ptr<Reversible> action)
{
    actions.push_back(action);
}

ReversibleActions::ReversibleActions() : reversed(false)
{

}
