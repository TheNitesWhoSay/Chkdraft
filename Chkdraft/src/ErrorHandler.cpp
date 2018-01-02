#include "ErrorHandler.h"

std::atomic<u32> KnownError::nextErrorId(1);

std::map<u32, std::shared_ptr<ErrorHandler>> ErrorHandler::knownErrorHandlers;

KnownError::KnownError(u32 id)
{
    
}

KnownError::KnownError(const KnownError& other) : std::exception(other.what())
{
    this->errorId = other.errorId;
}

u32 KnownError::getErrorId()
{
    return this->errorId;
}

std::atomic<u32> KnownError::GetNextErrorId()
{
    return nextErrorId ++;
}

ErrorHandlerResult ErrorHandler::HandleError(GenericCommand* command, KnownError& e) {
    auto handler = knownErrorHandlers.find(e.getErrorId());
    if ( handler != knownErrorHandlers.end() )
        return handler->second->HandleException(command, e);
    else
        return ErrorHandlerResult(ErrorAction::DiscardCommand, LogLevel::Error, std::string("Unknown error occured in command: ") + command->toString());
}
