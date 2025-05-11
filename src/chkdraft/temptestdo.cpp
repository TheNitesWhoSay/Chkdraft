#include <temptest.h>

void TestMap::init()
{
    auto edit = createAction();
    edit->units.append(Chk::Unit{});
}

void TestMap::doSomething()
{
    auto edit = createAction();
    edit->units[0].xc = 13;
}
