#include "TestCommands.h"

ExampleCommand::ExampleCommand(const std::string &addition) : GenericCommand(true, (u32)CommandClass::ExampleCommand, (u32)UndoRedoType::Example), addition(addition)
{

}

ExampleCommandPtr ExampleCommand::C(const std::string &addition)
{
    return std::shared_ptr<ExampleCommand>(new ExampleCommand(addition));
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
