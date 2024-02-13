#ifndef COMMANDER_H
#define COMMANDER_H
#include "GenericCommand.h"
#include "ErrorHandler.h"
#include <condition_variable>
#include <atomic>
#include <vector>
#include <thread>
#include <queue>
#include <stack>
#include <map>

/**
    Design Goals
        - Support ACID Commands
        - Support syncronous and asyncronous commands
        - Support using commands in mapping core while maintaining mapping cores reusability
        - Support solid error handling both in mapping core and in Chkdraft
        - Support registering listeners to specific commands and types of commands
*/

class Commander;
typedef std::shared_ptr<Commander> CommanderPtr;
typedef std::shared_ptr<std::stack<GenericCommandPtr>> CommandStackPtr;
typedef std::shared_ptr<std::vector<CommandListenerPtr>> ListenerVectorPtr;
using u32 = std::uint32_t;

class Commander
{
    public:
        Commander(std::shared_ptr<Logger> logger);
        virtual ~Commander();

        void Do(GenericCommandPtr command);
        void Do(const std::vector<GenericCommandPtr> & commands);
        void DoAcid(const std::vector<GenericCommandPtr> & commands, u32 undoRedoTypeid, bool async = false);
        void Undo(u32 undoRedoTypeId);
        void Redo(u32 undoRedoTypeId);

        void RegisterCommandListener(u32 commandClassId, CommandListenerPtr listener);
        void RegisterErrorHandler(u32 errorId, ErrorHandlerPtr errorHandler);

        void NoOp();

    protected:
        bool DoCommand(GenericCommandPtr command); // Attempts to do the command, returns true if this action should be undoable
        bool UndoCommand(GenericCommandPtr command); // Attempts to undo this command, returns true if this action should be redoable

        bool DoCommand(GenericCommandPtr command, bool hasAcidParent);
        bool UndoCommand(GenericCommandPtr command, bool hasAcidParent);

        bool DoDo(GenericCommandPtr command, bool retrying);
        bool DoUndo(GenericCommandPtr command, bool retrying);

        void DoSubItems(GenericCommandPtr command);
        void UndoSubItems(GenericCommandPtr command);

        bool DoAcidSubItems(GenericCommandPtr command);
        bool UndoAcidSubItems(GenericCommandPtr command);

        void NotifyCommandFinished(GenericCommandPtr command);

        ErrorHandlerResult HandleError(GenericCommandPtr command, KnownError & e);

    private:
        friend void begin(Commander* commander);
        void Run();
        void End(); // Finishes all pending commands and kills the command thread
        void ClipRedos(u32 undoRedoTypeId);
        void AddUndo(u32 undoRedoTypeId, GenericCommandPtr command);
        void AddRedo(u32 undoRedoTypeId, GenericCommandPtr command);
        void TryUndo();
        void TryRedo();

        std::shared_ptr<Logger> logger;

        std::queue<GenericCommandPtr> todoBuffer; // Only use this if you've locked the commandLocker
        std::map<u32, CommandStackPtr> undoBuffers;
        std::map<u32, CommandStackPtr> redoBuffers;
        std::map<u32, ListenerVectorPtr> commandListeners;
        std::map<u32, ErrorHandlerPtr> errorHandlers;

        bool hasCommandsToExecute; // Only use this if you've locked the commandLocker
        std::atomic<u32> numSynchronousCommands;

        std::unique_ptr<std::thread> commandThread;
        std::mutex commandLocker;
        std::mutex synchronousLocker;
        std::unique_lock<std::mutex> synchronousLock;
        std::condition_variable hasCommands;
        std::condition_variable synchronousExecution;

        GenericCommandPtr undoCommand;
        GenericCommandPtr redoCommand;
        GenericCommandPtr killCommand;
};

class AcidRollbackFailure : public std::exception
{
    public:
        explicit AcidRollbackFailure(const std::string & exceptionText) : exceptionText(exceptionText) { }
        virtual ~AcidRollbackFailure() throw();
        virtual const char* what() const throw() { return exceptionText.c_str(); }

    private:
        std::string exceptionText;
};

#endif