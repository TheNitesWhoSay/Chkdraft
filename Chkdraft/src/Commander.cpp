#include "Commander.h"
#include <memory>

class UndoCommand : public GenericCommand
{
public:
    UndoCommand(Logger &logger) : GenericCommand(logger, true) {}
};

class RedoCommand : public GenericCommand
{
public:
    RedoCommand(Logger &logger) : GenericCommand(logger, true) {}
};

Commander::Commander(Logger &logger) : logger(logger), hasCommandsToExecute(false), numSynchronousCommands(0),
    hasCommandsLock(hasCommandsLocker), synchronousLock(synchronousLocker),
    undoCommand(std::shared_ptr<UndoCommand>(new UndoCommand(logger))),
    redoCommand(std::shared_ptr<RedoCommand>(new RedoCommand(logger)))
{
    commandThread = std::unique_ptr<std::thread>(new std::thread(begin, this));
}

Commander::~Commander()
{
    if ( commandThread )
        commandThread->join();
}

void Commander::Do(GenericCommandPtr command)
{
    commandLocker.lock();

    bool isSynchronous = command.get()->isSynchronous();
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

void Commander::Do(const std::vector<GenericCommandPtr> &commands)
{
    commandLocker.lock();

    for ( GenericCommandPtr command : commands )
    {
        if ( command.get()->isSynchronous() )
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

void Commander::DoAcid(const std::vector<GenericCommandPtr> &subCommands, u32 undoRedoTypeid, bool async)
{
    commandLocker.lock();

    if ( !async )
        ++numSynchronousCommands;

    this->todoBuffer.push(GenericCommandPtr(new GenericCommand(logger, subCommands, async, true)));
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
    undoCommand->SetUndoRedoTypeId(undoRedoTypeId);

    bool isSynchronous = undoCommand.get()->isSynchronous();
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
    redoCommand->SetUndoRedoTypeId(undoRedoTypeId);

    bool isSynchronous = undoCommand.get()->isSynchronous();
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

void begin(Commander* commander)
{
    commander->Run();
}

void Commander::Run()
{
    while ( true )
    {
        GenericCommandPtr command(nullptr);

        commandLocker.lock();
        if ( todoBuffer.size() == 0 ) // Sleep until additional commands are entered
        {
            hasCommandsToExecute = false;
            commandLocker.unlock(); // Release resources before sleeping!
            hasCommands.wait(hasCommandsLock, [this] {
                return hasCommandsToExecute;
            });
            commandLocker.lock();
        }
        if ( todoBuffer.size() > 0 ) // Check if there's any commands to execute
        {
            command = todoBuffer.front();
            todoBuffer.pop();
        }
        commandLocker.unlock();

        if ( command )
        {
            // Execute the command
            if ( command == undoCommand )
                TryUndo();
            else if ( command == redoCommand )
                TryRedo();
            else if ( command->DoCommand() )
            {
                u32 undoRedoTypeId = command->Id();
                ClipRedos(undoRedoTypeId);
                AddUndo(undoRedoTypeId, command);
            }

            // Notify completion of command if synchronous
            if ( command->isSynchronous() )
            {
                --numSynchronousCommands;
                synchronousExecution.notify_one();
            }
        }
    }
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
        StackPtr undoBuffer(new std::stack<GenericCommandPtr>());
        undoBuffer->push(command);
        undoBuffers.insert(std::pair<u32, StackPtr>(undoRedoTypeId, undoBuffer));
    }
}

void Commander::AddRedo(u32 undoRedoTypeId, GenericCommandPtr command)
{
    auto foundRedoBuffer = redoBuffers.find(undoRedoTypeId);
    if ( foundRedoBuffer != redoBuffers.end() )
        foundRedoBuffer->second->push(command);
    else
    {
        StackPtr redoBuffer(new std::stack<GenericCommandPtr>());
        redoBuffer->push(command);
        redoBuffers.insert(std::pair<u32, StackPtr>(undoRedoTypeId, redoBuffer));
    }
}

void Commander::TryUndo()
{
    u32 undoRedoTypeId = undoCommand->GetUndoRedoTypeId();
    auto foundUndoBuffer = undoBuffers.find(undoRedoTypeId);

    if ( foundUndoBuffer != undoBuffers.end() )
    {
        auto undoBuffer = foundUndoBuffer->second;
        if ( !undoBuffer->empty() )
        {
            GenericCommandPtr command = undoBuffer->top();
            undoBuffer->pop();
            if ( command->UndoCommand() )
                AddRedo(undoRedoTypeId, command);
        }
    }
}

void Commander::TryRedo()
{
    u32 undoRedoTypeId = redoCommand->GetUndoRedoTypeId();
    auto foundRedoBuffer = redoBuffers.find(undoRedoTypeId);

    if ( foundRedoBuffer != redoBuffers.end() )
    {
        auto redoBuffer = foundRedoBuffer->second;
        if ( !redoBuffer->empty() )
        {
            GenericCommandPtr command = redoBuffer->top();
            redoBuffer->pop();
            if ( command->DoCommand() )
                AddUndo(undoRedoTypeId, command);
        }
    }
}
