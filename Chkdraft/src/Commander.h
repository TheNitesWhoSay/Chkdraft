#ifndef COMMANDER_H
#define COMMANDER_H
#include "CommonFiles\CommonFiles.h"
#include "GenericCommand.h"
#include <condition_variable>
#include <initializer_list>
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
        - Support solid error handling both in mapping core and in Chkdraft (todo)
        - Support registering listeners specific commands and types of commands (todo)
*/


class Commander;
typedef std::shared_ptr<Commander> CommanderPtr;
typedef std::shared_ptr<std::stack<GenericCommandPtr>> StackPtr;

class Commander
{
    public:
        Commander();
        ~Commander();

        void Do(GenericCommandPtr command);
        void Do(const std::vector<GenericCommandPtr> &commands);
        void DoAcid(const std::vector<GenericCommandPtr> &commands, u32 undoRedoTypeid, bool async = false);
        void Undo(u32 undoRedoTypeId);
        void Redo(u32 undoRedoTypeId);

    private:
        friend void begin(Commander* commander);
        void Run();
        void ClipRedos(u32 undoRedoTypeId);
        void AddUndo(u32 undoRedoTypeId, GenericCommandPtr command);
        void AddRedo(u32 undoRedoTypeId, GenericCommandPtr command);
        void TryUndo();
        void TryRedo();

        std::queue<GenericCommandPtr> todoBuffer; // Only use this if you've locked the commandLocker
        std::map<u32, StackPtr> undoBuffers;
        std::map<u32, StackPtr> redoBuffers;

        bool hasCommandsToExecute; // Only use this if you've locked the commandLocker
        bool synchronousCommandFinished;

        std::unique_ptr<std::thread> commandThread;
        std::mutex commandLocker;
        std::mutex hasCommandsLocker;
        std::mutex synchronousLocker;
        std::unique_lock<std::mutex> hasCommandsLock;
        std::unique_lock<std::mutex> synchronousLock;
        std::condition_variable hasCommands;
        std::condition_variable synchronousExecution;

        static GenericCommandPtr undoCommand;
        static GenericCommandPtr redoCommand;
};

#endif