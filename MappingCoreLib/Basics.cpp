#include "Basics.h"
#include <cmath>
/* Allow file to be partitioned along templates */ #ifndef INCLUDE_TEMPLATES_ONLY

void IgnoreErr(const std::string & file, unsigned int line, const std::string msg, ...)
{

}

void PrintError(const std::string & file, unsigned int line, const std::string msg, ...)
{

}

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename valueType>
void ascendingOrder(valueType & low, valueType & high)
{
    if ( low > high )
        std::swap(low, high);
}
/* End templates */ #else

/* Templates */ #endif
                #ifdef INCLUDE_TEMPLATES_ONLY
template <typename T>
s32 round(T value)
{    
    return s32(std::floor(static_cast<double>(value) + 0.5));
}

/* End file partitioning */ #endif
