#ifndef TESTCOMMANDS_H
#define TESTCOMMANDS_H
#include "CommonFiles\CommonFiles.h"
#include "GenericCommand.h"

class ExampleCommand;
typedef std::shared_ptr<ExampleCommand> ExampleCommandPtr;

class ExampleCommand : public GenericCommand
{
    public:
        ExampleCommand(Logger &logger, const std::string &addition);
        static ExampleCommandPtr C(Logger &logger, const std::string &addition);
        virtual u32 Id();

    protected:
        virtual void Do();
        virtual void Undo();

    private:
        std::string addition;
        static std::string text;
};

#endif