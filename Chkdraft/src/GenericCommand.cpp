#include "GenericCommand.h"
#include "ErrorHandler.h"
#include <iostream>
#include <algorithm>
#include <vector>

GenericCommand::GenericCommand(Logger &logger, bool isSynchronous) : logger(logger), isCommandSynchronous(isSynchronous), subCommandsAreAcid(false),
undoRedoTypeId(~((u32)0))
{

}

GenericCommand::GenericCommand(Logger &logger, const std::vector<GenericCommandPtr> &subCommands, bool isSynchronous, bool subCommandsAreAcid)
    : logger(logger), subCommands(subCommands), isCommandSynchronous(isSynchronous), subCommandsAreAcid(subCommandsAreAcid), undoRedoTypeId(~((u32)0))
{

}

bool GenericCommand::isSynchronous()
{
    return this->isCommandSynchronous;
}

bool GenericCommand::hasAcidSubCommands()
{
    return subCommandsAreAcid;
}

u32 GenericCommand::Id()
{
    return ~((u32)0);
}

std::string GenericCommand::toString()
{
    return std::string("[Override this method to print command details]");
}

bool GenericCommand::DoCommand()
{
    return DoCommand(false);
}

bool GenericCommand::UndoCommand()
{
    return UndoCommand(false);
}

u32 GenericCommand::GetUndoRedoTypeId()
{
    if ( undoRedoTypeId == GenericCommand::Id() )
        undoRedoTypeId = Id();

    return undoRedoTypeId;
}

void GenericCommand::SetUndoRedoTypeId(u32 undoRedoTypeId)
{
    this->undoRedoTypeId = undoRedoTypeId;
}

bool GenericCommand::DoCommand(bool hasAcidParent)
{
    if ( hasAcidParent || hasAcidSubCommands() )
    {
        if ( subCommands.size() > 0 )
            return DoAcidSubItems();
        else
            return DoDo(false);
    }
    else
    {
        if ( subCommands.size() > 0 )
            DoSubItems();
        else
            DoDo(false);

        return true;
    }
    return false;
}

bool GenericCommand::UndoCommand(bool hasAcidParent)
{
    if ( hasAcidParent || hasAcidSubCommands() )
    {
        if ( subCommands.size() > 0 )
            return UndoAcidSubItems();
    }
    else
    {
        if ( subCommands.size() > 0 )
            UndoSubItems();
        else
            Undo();

        return true;
    }
    return false;
}

bool GenericCommand::DoDo(bool retrying)
{
    try {
        Do();
    } catch ( KnownError &e ) {
        ErrorHandlerResult result = ErrorHandler::HandleError(this, e);
        logger.log(result.logString, result.logLevel);
        if ( result.primaryAction == ErrorAction::RetryCommand && !retrying )
            return DoDo(true);
        else
            return false;
    } catch ( std::exception &e ) {
        logger.error(std::string(e.what()));
        logger.error("An unknown exception occured during command: " + this->toString());
    }
    return true;
}

bool GenericCommand::DoUndo(bool retrying)
{
    try {
        Undo();
    } catch ( KnownError &e ) {
        ErrorHandlerResult result = ErrorHandler::HandleError(this, e);
        logger.log(result.logString, result.logLevel);
        if ( result.primaryAction == ErrorAction::RetryCommand && !retrying )
            return DoUndo(true);
        else
            return false;
    } catch ( std::exception &e ) {
        logger.error(std::string(e.what()));
        logger.error("An unknown exception occured during undo command: " + this->toString());
    }
    return true;
}

void GenericCommand::Do()
{
    // Override this method to perform some 
    std::cout << "Default Do() hit, override me!" << std::endl;
}

void GenericCommand::Undo()
{
    // Override this method to perform some action
    std::cout << "Default Undo(), override me!" << std::endl;
}

void GenericCommand::DoSubItems()
{
    for ( auto subCommand : subCommands )
        subCommand->DoCommand(false);
}

void GenericCommand::UndoSubItems()
{
    for ( auto subCommand : subCommands )
        subCommand->UndoCommand(false);
}

bool GenericCommand::DoAcidSubItems()
{
    for ( auto subCommand = subCommands.begin(); subCommand != subCommands.end(); ++subCommand )
    {
        bool success = false;
        try {
            success = (*subCommand)->DoCommand(true);
        } catch ( KnownError &e ) {
            ErrorHandlerResult result = ErrorHandler::HandleError(this, e);
            logger.log(result.logString, result.logLevel);
            if ( result.primaryAction == ErrorAction::RetryCommand )
            {
                try {
                    success = (*subCommand)->DoCommand(true);
                } catch ( KnownError &e ) {
                    result = ErrorHandler::HandleError(this, e);
                    logger.log(result.logString, result.logLevel);
                } catch ( std::exception &e ) {
                    logger.error(std::string(e.what()));
                    logger.error("An unknown exception occured during command: " + this->toString());
                }
            }
        } catch ( std::exception &e ) {
            logger.error(std::string(e.what()));
            logger.error("An unknown exception occured during undo command: " + this->toString());
        }
        if ( !success )
        {
            while ( true )
            {
                if ( subCommand == subCommands.begin() )
                    return false;

                --subCommand;

                bool undoSucceeded = false;
                try {
                    undoSucceeded = (*subCommand)->UndoCommand(true);
                } catch ( KnownError &e ) {
                    ErrorHandlerResult result = ErrorHandler::HandleError(this, e);
                    logger.log(result.logString, result.logLevel);
                    if ( result.primaryAction == ErrorAction::RetryCommand )
                    {
                        try {
                            success = (*subCommand)->DoCommand(true);
                        } catch ( KnownError &e ) {
                            result = ErrorHandler::HandleError(this, e);
                            logger.log(result.logString, result.logLevel);
                        } catch ( std::exception &e ) {
                            logger.error(std::string(e.what()));
                            logger.error("An unknown exception occured during command: " + this->toString());
                        }
                    }
                } catch ( std::exception &e ) {
                    logger.error(std::string(e.what()));
                    logger.error("An unknown exception occured during undo command: " + this->toString());
                }
                if ( !undoSucceeded )
                    throw new AcidRollbackFailure("ACID Do rollback Failed!");
            }
        }
    }
    return true;
}

bool GenericCommand::UndoAcidSubItems()
{
    for ( auto subCommand = subCommands.rbegin(); subCommand != subCommands.rend(); ++subCommand )
    {
        bool success = false;
        try {
            success = (*subCommand)->UndoCommand(true);
        } catch ( KnownError &e ) {
            ErrorHandlerResult result = ErrorHandler::HandleError(this, e);
            logger.log(result.logString, result.logLevel);
            if ( result.primaryAction == ErrorAction::RetryCommand )
            {
                try {
                    success = (*subCommand)->UndoCommand(true);
                } catch ( KnownError &e ) {
                    result = ErrorHandler::HandleError(this, e);
                    logger.log(result.logString, result.logLevel);
                } catch ( std::exception &e ) {
                    logger.error(std::string(e.what()));
                    logger.error("An unknown exception occured during undo command: " + this->toString());
                }
            }
        } catch ( std::exception &e ) {
            logger.error(std::string(e.what()));
            logger.error("An unknown exception occured during undo command: " + this->toString());
        }
        if ( !success )
        {
            while ( true )
            {
                if ( subCommand == subCommands.rbegin() )
                    return false;

                --subCommand;

                bool doSucceeded = false;
                try {
                    doSucceeded = (*subCommand)->DoCommand(true);
                } catch ( KnownError &e ) {
                    ErrorHandlerResult result = ErrorHandler::HandleError(this, e);
                    logger.log(result.logString, result.logLevel);
                    if ( result.primaryAction == ErrorAction::RetryCommand )
                    {
                        try {
                            success = (*subCommand)->UndoCommand(true);
                        } catch ( KnownError &e ) {
                            result = ErrorHandler::HandleError(this, e);
                            logger.log(result.logString, result.logLevel);
                        } catch ( std::exception &e ) {
                            logger.error(std::string(e.what()));
                            logger.error("An unknown exception occured during command: " + this->toString());
                        }
                    }
                } catch ( std::exception &e ) {
                    logger.error(std::string(e.what()));
                    logger.error("An unknown exception occured during undo command: " + this->toString());
                }
                if ( !doSucceeded )
                    throw new AcidRollbackFailure("ACID Undo Rollback Failed!");
            }
        }
    }
    return true;
}
