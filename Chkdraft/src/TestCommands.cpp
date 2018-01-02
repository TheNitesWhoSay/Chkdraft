#include "TestCommands.h"

ExampleCommand::ExampleCommand(Logger &logger, const std::string &addition) : GenericCommand(logger, true), addition(addition)
{

}

ExampleCommandPtr ExampleCommand::C(Logger &logger, const std::string &addition)
{
    return std::shared_ptr<ExampleCommand>(new ExampleCommand(logger, addition));
}

u32 ExampleCommand::Id()
{
    return 1;
}

void ExampleCommand::Do()
{
    text += addition;
    std::cout << text << std::endl;
}

void ExampleCommand::Undo()
{
    text = text.substr(0, text.size()-addition.size());
    std::cout << text << std::endl;
}
