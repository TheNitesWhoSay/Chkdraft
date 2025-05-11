#include <mapping_core/scenario.h>
#include <RareCpp/editor.h>

struct TestMap : RareEdit::Tracked<MapData, TestMap>
{
    TestMap() : Tracked{this} {}

    void init();

    void doSomething();

    void printChangeHistory();

    void undoAction();

    void redoAction();
};
