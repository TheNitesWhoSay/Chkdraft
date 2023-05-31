#include "Commander.h"
#include <memory>

class GenericUndoCommand : public GenericCommand
{
public:
    GenericUndoCommand() : GenericCommand(true, (u32)ReservedCommandClassIds::GenericUndoCommand) {}
    virtual ~GenericUndoCommand() {}
};

class GenericRedoCommand : public GenericCommand
{
public:
    GenericRedoCommand() : GenericCommand(true, (u32)ReservedCommandClassIds::GenericRedoCommand) {}
    virtual ~GenericRedoCommand() {}
};

class KillCommand : public GenericCommand
{
public:
    KillCommand() : GenericCommand(true, (u32)ReservedCommandClassIds::KillCommanderCommand) {}
    virtual ~KillCommand() {}
};

void begin(Commander* commander);

Commander::Commander(std::shared_ptr<Logger> logger) : logger(logger), hasCommandsToExecute(false), numSynchronousCommands(0),
    synchronousLock(synchronousLocker),
    undoCommand(std::shared_ptr<GenericUndoCommand>(new GenericUndoCommand())),
    redoCommand(std::shared_ptr<GenericRedoCommand>(new GenericRedoCommand())),
    killCommand(std::shared_ptr<KillCommand>(new KillCommand()))
{
    commandThread = std::unique_ptr<std::thread>(new std::thread(begin, this));
}

Commander::~Commander()
{
    End();
}

void Commander::Do(GenericCommandPtr command)
{
    commandLocker.lock();

    bool isSynchronous = command->isSynchronous;
    if ( isSynchronous )
        ++numSynchronousCommands;

    todoBuffer.push(command);
    hasCommandsToExecute = true;
    hasCommands.notify_one();
    commandLocker.unlock();

    if ( isSynchronous )
    {
        synchronousExecution.wait(synchronousLock, [this] {
            return numSynchronousCommands == 0;
        });
    }
}

void Commander::Do(const std::vector<GenericCommandPtr> & commands)
{
    commandLocker.lock();

    for ( GenericCommandPtr command : commands )
    {
        if ( command->isSynchronous )
            ++numSynchronousCommands;

        this->todoBuffer.push(command);
    }

    hasCommandsToExecute = true;
    hasCommands.notify_one();
    commandLocker.unlock();

    if ( numSynchronousCommands > 0 )
    {
        synchronousExecution.wait(synchronousLock, [this] {
            return numSynchronousCommands == 0;
        });
    }
}

void Commander::DoAcid(const std::vector<GenericCommandPtr> & subCommands, u32 undoRedoTypeid, bool async)
{
    commandLocker.lock();

    if ( !async )
        ++numSynchronousCommands;

    this->todoBuffer.push(GenericCommandPtr(new GenericCommand(subCommands, async, true, (u32)ReservedCommandClassIds::AcidCommandParent)));
    hasCommandsToExecute = true;
    hasCommands.notify_one();
    commandLocker.unlock();

    if ( !async )
    {
        synchronousExecution.wait(synchronousLock, [this] {
            return numSynchronousCommands == 0;
        });
    }
}

void Commander::Undo(u32 undoRedoTypeId)
{
    commandLocker.lock();
    undoCommand->undoRedoTypeId = undoRedoTypeId;

    bool isSynchronous = undoCommand->isSynchronous;
    if ( isSynchronous )
        ++numSynchronousCommands;

    this->todoBuffer.push(undoCommand);
    hasCommandsToExecute = true;
    hasCommands.notify_one();
    commandLocker.unlock();

    if ( numSynchronousCommands > 0 )
    {
        synchronousExecution.wait(synchronousLock, [this] {
            return numSynchronousCommands == 0;
        });
    }
}

void Commander::Redo(u32 undoRedoTypeId)
{
    commandLocker.lock();
    redoCommand->undoRedoTypeId = undoRedoTypeId;

    bool isSynchronous = undoCommand->isSynchronous;
    if ( isSynchronous )
        ++numSynchronousCommands;

    this->todoBuffer.push(redoCommand);
    hasCommandsToExecute = true;
    hasCommands.notify_one();
    commandLocker.unlock();

    if ( numSynchronousCommands > 0 )
    {
        synchronousExecution.wait(synchronousLock, [this] {
            return numSynchronousCommands == 0;
        });
    }
}

void Commander::RegisterCommandListener(u32 commandClassId, CommandListenerPtr listener)
{
    auto found = commandListeners.find(commandClassId);
    if ( found != commandListeners.end() )
    {
        found->second->push_back(listener);
    }
    else
    {
        ListenerVectorPtr newListenerList = ListenerVectorPtr(new std::vector<CommandListenerPtr>());
        newListenerList->push_back(listener);
        commandListeners.insert(std::pair<u32, ListenerVectorPtr>(commandClassId, newListenerList));
    }
}

void Commander::RegisterErrorHandler(u32 errorId, ErrorHandlerPtr errorHandler)
{
    errorHandlers.insert(std::pair<u32, ErrorHandlerPtr>(errorId, errorHandler));
}

void Commander::NoOp()
{

}

bool Commander::DoCommand(GenericCommandPtr command)
{
    if ( DoCommand(command, false) )
    {
        NotifyCommandFinished(command);
        return true;
    }
    return false;
}

bool Commander::UndoCommand(GenericCommandPtr command)
{
    if ( UndoCommand(command, false) )
    {
        NotifyCommandFinished(command);
        return true;
    }
    return false;
}

bool Commander::DoCommand(GenericCommandPtr command, bool hasAcidParent)
{
    if ( hasAcidParent || command->subCommandsAreAcid )
    {
        if ( command->subCommands.size() > 0 )
            return DoAcidSubItems(command);
        else
            return DoDo(command, false);
    }
    else
    {
        if ( command->subCommands.size() > 0 )
            DoSubItems(command);
        else
            DoDo(command, false);

        return true;
    }
    return false;
}

bool Commander::UndoCommand(GenericCommandPtr command, bool hasAcidParent)
{
    if ( hasAcidParent || command->subCommandsAreAcid )
    {
        if ( command->subCommands.size() > 0 )
            return UndoAcidSubItems(command);
    }
    else
    {
        if ( command->subCommands.size() > 0 )
            UndoSubItems(command);
        else
            command->Undo(*logger);

        return true;
    }
    return false;
}

bool Commander::DoDo(GenericCommandPtr command, bool retrying)
{
    try {
        command->Do(*logger);
    } catch ( KnownError & e ) {
        ErrorHandlerResult result = HandleError(command, e);
        logger->log(result.logLevel, result.logString);
        if ( result.primaryAction == ErrorAction::RetryCommand && !retrying )
            return DoDo(command, true);
        else
            return false;
    } catch ( std::exception & e ) {
        logger->error(std::string(e.what()));
        logger->error("An unknown exception occured during command: " + command->toString());
    }
    return true;
}

bool Commander::DoUndo(GenericCommandPtr command, bool retrying)
{
    try {
        command->Undo(*logger);
    } catch ( KnownError & e ) {
        ErrorHandlerResult result = HandleError(command, e);
        logger->log(result.logLevel, result.logString);
        if ( result.primaryAction == ErrorAction::RetryCommand && !retrying )
            return DoUndo(command, true);
        else
            return false;
    } catch ( std::exception & e ) {
        logger->error(std::string(e.what()));
        logger->error("An unknown exception occured during undo command: " + command->toString());
    }
    return true;
}

void Commander::DoSubItems(GenericCommandPtr command)
{
    for ( auto subCommand : command->subCommands )
        DoCommand(subCommand, false);
}

void Commander::UndoSubItems(GenericCommandPtr command)
{
    for ( auto subCommand : command->subCommands )
        UndoCommand(subCommand, false);
}

bool Commander::DoAcidSubItems(GenericCommandPtr command)
{
    for ( auto subCommand = command->subCommands.begin(); subCommand != command->subCommands.end(); ++subCommand )
    {
        bool success = false;
        try {
            success = DoCommand(*subCommand, true);
        } catch ( KnownError & e ) {
            ErrorHandlerResult result = HandleError(*subCommand, e);
            logger->log(result.logLevel, result.logString);
            if ( result.primaryAction == ErrorAction::RetryCommand )
            {
                try {
                    success = DoCommand(*subCommand, true);
                } catch ( KnownError & e ) {
                    result = HandleError(*subCommand, e);
                    logger->log(result.logLevel, result.logString);
                } catch ( std::exception & e ) {
                    logger->error(std::string(e.what()));
                    logger->error("An unknown exception occured during command: " + (*subCommand)->toString());
                }
            }
        } catch ( std::exception & e ) {
            logger->error(std::string(e.what()));
            logger->error("An unknown exception occured during undo command: " + (*subCommand)->toString());
        }
        if ( !success )
        {
            while ( true )
            {
                if ( subCommand == command->subCommands.begin() )
                    return false;

                --subCommand;

                bool undoSucceeded = false;
                try {
                    undoSucceeded = UndoCommand(*subCommand, true);
                } catch ( KnownError & e ) {
                    ErrorHandlerResult result = HandleError(*subCommand, e);
                    logger->log(result.logLevel, result.logString);
                    if ( result.primaryAction == ErrorAction::RetryCommand )
                    {
                        try {
                            success = DoCommand(*subCommand, true);
                        } catch ( KnownError & e ) {
                            result = HandleError(*subCommand, e);
                            logger->log(result.logLevel, result.logString);
                        } catch ( std::exception & e ) {
                            logger->error(std::string(e.what()));
                            logger->error("An unknown exception occured during command: " + (*subCommand)->toString());
                        }
                    }
                } catch ( std::exception & e ) {
                    logger->error(std::string(e.what()));
                    logger->error("An unknown exception occured during undo command: " + (*subCommand)->toString());
                }
                if ( !undoSucceeded )
                    throw new AcidRollbackFailure("ACID Do rollback Failed!");
            }
        }
    }
    return true;
}

bool Commander::UndoAcidSubItems(GenericCommandPtr command)
{
    for ( auto subCommand = command->subCommands.rbegin(); subCommand != command->subCommands.rend(); ++subCommand )
    {
        bool success = false;
        try {
            success = UndoCommand(*subCommand, true);
        } catch ( KnownError & e ) {
            ErrorHandlerResult result = HandleError(*subCommand, e);
            logger->log(result.logLevel, result.logString);
            if ( result.primaryAction == ErrorAction::RetryCommand )
            {
                try {
                    success = UndoCommand(*subCommand, true);
                } catch ( KnownError & e ) {
                    result = HandleError(command, e);
                    logger->log(result.logLevel, result.logString);
                } catch ( std::exception & e ) {
                    logger->error(std::string(e.what()));
                    logger->error("An unknown exception occured during undo command: " + (*subCommand)->toString());
                }
            }
        } catch ( std::exception & e ) {
            logger->error(std::string(e.what()));
            logger->error("An unknown exception occured during undo command: " + (*subCommand)->toString());
        }
        if ( !success )
        {
            while ( true )
            {
                if ( subCommand == command->subCommands.rbegin() )
                    return false;

                --subCommand;

                bool doSucceeded = false;
                try {
                    doSucceeded = DoCommand(*subCommand, true);
                } catch ( KnownError & e ) {
                    ErrorHandlerResult result = HandleError(*subCommand, e);
                    logger->log(result.logLevel, result.logString);
                    if ( result.primaryAction == ErrorAction::RetryCommand )
                    {
                        try {
                            success = UndoCommand(*subCommand, true);
                        } catch ( KnownError & e ) {
                            result = HandleError(*subCommand, e);
                            logger->log(result.logLevel, result.logString);
                        } catch ( std::exception & e ) {
                            logger->error(std::string(e.what()));
                            logger->error("An unknown exception occured during command: " + (*subCommand)->toString());
                        }
                    }
                } catch ( std::exception & e ) {
                    logger->error(std::string(e.what()));
                    logger->error("An unknown exception occured during undo command: " + (*subCommand)->toString());
                }
                if ( !doSucceeded )
                    throw new AcidRollbackFailure("ACID Undo Rollback Failed!");
            }
        }
    }
    return true;
}

void Commander::NotifyCommandFinished(GenericCommandPtr command)
{
    u32 commandClassId = command->commandClassId;
    auto found = commandListeners.find(commandClassId);
    if ( found != commandListeners.end() )
    {
        for ( auto listener = found->second->begin(); listener != found->second->end(); ++listener )
            (*listener)->Listen(commandClassId, command);
    }
}

ErrorHandlerResult Commander::HandleError(GenericCommandPtr command, KnownError & e)
{
    auto handler = errorHandlers.find(e.getErrorId());
    if ( handler != errorHandlers.end() )
        return handler->second->HandleException(command, e);
    else
        return ErrorHandlerResult(ErrorAction::DiscardCommand, LogLevel::Error, std::string("Unknown error occured in command: ") + command->toString());
}

void begin(Commander* commander)
{
    commander->Run();
}

void Commander::Run()
{
    std::mutex hasCommandsLocker;
    std::unique_lock<std::mutex> hasCommandsLock(hasCommandsLocker);
    bool keepRunning = true;
    do
    {
        GenericCommandPtr command(nullptr);

        commandLocker.lock();
        if ( todoBuffer.size() == 0 ) // Sleep until additional commands are entered
        {
            hasCommandsToExecute = false;
            commandLocker.unlock(); // Release resources before sleeping!
            hasCommands.wait(hasCommandsLock, [this] {
                return hasCommandsToExecute;
            }); // hasCommandsLock will be locked by the wait method
            hasCommandsLock.unlock();
            commandLocker.lock(); // Re-aquire commandLocker as sleeping is complete
        }
        if ( todoBuffer.size() > 0 ) // Check if there's any commands to execute
        {
            command = todoBuffer.front();
            todoBuffer.pop();
        }
        commandLocker.unlock();

        if ( command != nullptr )
        {
            // Execute the command
            if ( command == undoCommand )
                TryUndo();
            else if ( command == redoCommand )
                TryRedo();
            else if ( command == killCommand )
                keepRunning = false;
            else if ( DoCommand(command) )
            {
                u32 undoRedoTypeId = command->undoRedoTypeId;
                ClipRedos(undoRedoTypeId);
                AddUndo(undoRedoTypeId, command);
            }

            // Notify completion of command if synchronous
            if ( command->isSynchronous )
            {
                --numSynchronousCommands;
                synchronousExecution.notify_one();
            }
        }
    } while ( keepRunning );
}

void Commander::End()
{
    commandLocker.lock();

    bool isSynchronous = killCommand->isSynchronous;
    if ( isSynchronous )
        ++numSynchronousCommands;

    this->todoBuffer.push(killCommand);
    hasCommandsToExecute = true;
    hasCommands.notify_one();
    commandLocker.unlock();

    if ( numSynchronousCommands > 0 )
    {
        synchronousExecution.wait(synchronousLock, [this] {
            return numSynchronousCommands == 0;
        });
    }
    commandThread->join();
}

void Commander::ClipRedos(u32 undoRedoTypeId)
{
    auto foundRedoBuffer = redoBuffers.find(undoRedoTypeId);
    if ( foundRedoBuffer != redoBuffers.end() )
    {
        auto redoBuffer = foundRedoBuffer->second;
        while ( !redoBuffer->empty() )
            redoBuffer->pop();
    }
}

void Commander::AddUndo(u32 undoRedoTypeId, GenericCommandPtr command)
{
    auto foundUndoBuffer = undoBuffers.find(undoRedoTypeId);
    if ( foundUndoBuffer != undoBuffers.end() )
    {
        auto undoBuffer = foundUndoBuffer->second;
        undoBuffer->push(command);
    }
    else
    {
        CommandStackPtr undoBuffer(new std::stack<GenericCommandPtr>());
        undoBuffer->push(command);
        undoBuffers.insert(std::pair<u32, CommandStackPtr>(undoRedoTypeId, undoBuffer));
    }
}

void Commander::AddRedo(u32 undoRedoTypeId, GenericCommandPtr command)
{
    auto foundRedoBuffer = redoBuffers.find(undoRedoTypeId);
    if ( foundRedoBuffer != redoBuffers.end() )
        foundRedoBuffer->second->push(command);
    else
    {
        CommandStackPtr redoBuffer(new std::stack<GenericCommandPtr>());
        redoBuffer->push(command);
        redoBuffers.insert(std::pair<u32, CommandStackPtr>(undoRedoTypeId, redoBuffer));
    }
}

void Commander::TryUndo()
{
    u32 undoRedoTypeId = undoCommand->undoRedoTypeId;
    auto foundUndoBuffer = undoBuffers.find(undoRedoTypeId);

    if ( foundUndoBuffer != undoBuffers.end() )
    {
        auto undoBuffer = foundUndoBuffer->second;
        if ( !undoBuffer->empty() )
        {
            GenericCommandPtr command = undoBuffer->top();
            undoBuffer->pop();
            if ( UndoCommand(command) )
                AddRedo(undoRedoTypeId, command);
        }
    }
}

void Commander::TryRedo()
{
    u32 undoRedoTypeId = redoCommand->undoRedoTypeId;
    auto foundRedoBuffer = redoBuffers.find(undoRedoTypeId);

    if ( foundRedoBuffer != redoBuffers.end() )
    {
        auto redoBuffer = foundRedoBuffer->second;
        if ( !redoBuffer->empty() )
        {
            GenericCommandPtr command = redoBuffer->top();
            redoBuffer->pop();
            if ( DoCommand(command) )
                AddUndo(undoRedoTypeId, command);
        }
    }
}

AcidRollbackFailure::~AcidRollbackFailure()
{

}
