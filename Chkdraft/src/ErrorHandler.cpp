#include "ErrorHandler.h"

std::atomic<u32> KnownError::nextErrorId(1);

KnownError::KnownError(u32 id)
{
    
}

KnownError::KnownError(const KnownError& other) : std::exception(other.what())
{
    this->errorId = other.errorId;
}

u32 KnownError::getErrorId()
{
    return this->errorId;
}

std::atomic<u32> KnownError::GetNextErrorId()
{
    return nextErrorId ++;
}
