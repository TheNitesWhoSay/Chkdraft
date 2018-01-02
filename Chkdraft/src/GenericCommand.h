#ifndef GENERICCOMMAND_H
#define GENERICCOMMAND_H
#include "CommonFiles\CommonFiles.h"
#include "Logger.h"
#include <vector>
#include <queue>

class CommandListener;
class GenericCommand;

typedef std::shared_ptr<CommandListener> CommandListenerPtr;
typedef std::shared_ptr<GenericCommand> GenericCommandPtr;

class CommandListener{};

class GenericCommand
{
    public:
        GenericCommand(Logger &logger, bool isSynchronous);
        GenericCommand(Logger &logger, const std::vector<GenericCommandPtr> &subCommands, bool isSynchronous, bool subCommandsAreAcid);

        bool isSynchronous();
        bool hasAcidSubCommands();

        bool DoCommand(); // Returns true if this action should be undoable
        bool UndoCommand(); // Returns true if this action should be redoable

        u32 GetUndoRedoTypeId();
        void SetUndoRedoTypeId(u32 undoRedoTypeId);

        virtual u32 Id(); // Calculate the id of the undo-redo group this command should belong to

        virtual std::string toString();

    protected:

        bool DoCommand(bool hasAcidParent);
        bool UndoCommand(bool hasAcidParent);

        bool DoDo(bool retrying);
        bool DoUndo(bool retrying);

        virtual void Do(); // Override this method to perform some action when there are no subCommands
        virtual void Undo(); // Override this method to perform some action when there are no subCommands

        virtual void DoSubItems();
        virtual void UndoSubItems();

        virtual bool DoAcidSubItems();
        virtual bool UndoAcidSubItems();

    private:
        GenericCommand(); // Disallow ctor
        u32 undoRedoTypeId;
        bool isCommandSynchronous;
        bool subCommandsAreAcid;
        std::vector<GenericCommandPtr> subCommands;
        std::vector<CommandListenerPtr> listeners;
        Logger &logger;
};

class AcidRollbackFailure : public std::exception
{
    public:
        explicit AcidRollbackFailure(const std::string &exceptionText) : exceptionText(exceptionText) { }
        virtual const char* what() const throw() { return exceptionText.c_str(); }
        virtual ~AcidRollbackFailure() throw() { }

    private:
        std::string exceptionText;
};

#endif