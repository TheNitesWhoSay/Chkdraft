#include "TestCommands.h"
#include "CommandTypes.h"
#include <iostream>
#include <string>

extern Logger logger;

std::string ExampleCommand::text;

ExampleCommand::ExampleCommand(const std::string & addition) : GenericCommand(true, (u32)CommandClass::ExampleCommand, (u32)UndoRedoType::Example), addition(addition)
{

}

ExampleCommand::~ExampleCommand()
{

}

ExampleCommandPtr ExampleCommand::C(const std::string & addition)
{
    return std::shared_ptr<ExampleCommand>(new ExampleCommand(addition));
}

void ExampleCommand::Do()
{
    text += addition;
    logger.info(text);
}

void ExampleCommand::Undo()
{
    text = text.substr(0, text.size()-addition.size());
    logger.info(text);
}
