#include "Logger.h"
#include <ctime>

#ifdef HAS_CONSOLE
#include <Windows.h>

Console Console::console = Console();
FILE* consoleIn = NULL;
FILE* consoleOut = NULL;

Console::Console()
{
    if ( ::GetConsoleWindow() == NULL )
    {
        if ( ::AllocConsole() != 0 )
        {
            if ( ::freopen_s(&::consoleIn, "CONIN$", "r", stdin) == 0 )
                std::cin.clear();
            if ( ::freopen_s(&::consoleOut, "CONOUT$", "w", stdout) == 0 )
                std::cout.clear();
        }
    }
}

Console::~Console()
{
    if ( consoleIn != NULL )
        ::fclose(consoleIn);

    if ( consoleOut != NULL )
        ::fclose(consoleOut);

    ::FreeConsole();
}

void Console::setVisible(bool visible)
{
    HWND handle = ::GetConsoleWindow();
    if ( handle != NULL )
        ::ShowWindow(::GetConsoleWindow(), visible ? SW_SHOW : SW_HIDE);
}

void Console::toggleVisible()
{
    HWND handle = ::GetConsoleWindow();
    setVisible(!::IsWindowVisible(handle), handle);
}

void Console::setVisible(bool visible, void* voidHandle)
{
    HWND handle = (HWND)voidHandle;
    if ( handle != NULL )
        ::ShowWindow(handle, visible ? SW_SHOW : SW_HIDE);
}
#endif

Logger::Logger(LogLevel logLevel) :
    logLevel(logLevel), outputStream(std::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){})), aggregator(nullptr),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::ostream & outputStream, LogLevel logLevel) :
    logLevel(logLevel), outputStream(std::shared_ptr<std::ostream>(&outputStream, [](std::ostream*){})), aggregator(nullptr),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::ostream & outputStream, Logger & aggregator, LogLevel logLevel) :
    logLevel(logLevel), outputStream(std::shared_ptr<std::ostream>(&outputStream, [](std::ostream*){})), aggregator(std::shared_ptr<Logger>(&aggregator, [](Logger*){})),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::ostream & outputStream, std::shared_ptr<Logger> aggregator, LogLevel logLevel) :
    logLevel(logLevel), outputStream(std::shared_ptr<std::ostream>(&outputStream, [](std::ostream*){})), aggregator(aggregator),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, LogLevel logLevel) :
    logLevel(logLevel), outputStream(outputStream), aggregator(nullptr),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, Logger & aggregator, LogLevel logLevel) :
    logLevel(logLevel), outputStream(outputStream), aggregator(std::shared_ptr<Logger>(&aggregator, [](Logger*){})),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, std::shared_ptr<Logger> aggregator, LogLevel logLevel) :
    logLevel(logLevel), outputStream(outputStream), aggregator(aggregator),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(Logger & aggregator, LogLevel logLevel) :
    logLevel(logLevel), outputStream(std::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){})), aggregator(std::shared_ptr<Logger>(&aggregator, [](Logger*){})),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::shared_ptr<Logger> aggregator, LogLevel logLevel) :
    logLevel(logLevel), outputStream(std::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){})), aggregator(aggregator),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::~Logger()
{

}

LogLevel Logger::getLogLevel()
{
    return logLevel;
}

std::shared_ptr<std::ostream> Logger::getOutputStream()
{
    return outputStream;
}

std::shared_ptr<Logger> Logger::getAggregator()
{
    return aggregator;
}

void Logger::setLogLevel(LogLevel logLevel)
{
    this->logLevel = logLevel;
    this->streamLogLevel = logLevel;
}

void Logger::setOutputStream(std::ostream & outputStream)
{
    this->outputStream = std::shared_ptr<std::ostream>(&outputStream, [](std::ostream*){});
}

void Logger::setOutputStream(std::shared_ptr<std::ostream> outputStream)
{
    this->outputStream = outputStream;
}

void Logger::setAggregator(Logger & aggregator)
{
    this->aggregator = std::shared_ptr<Logger>(&aggregator, [](Logger*){});
}

void Logger::setAggregator(std::shared_ptr<Logger> aggregator)
{
    this->aggregator = aggregator;
}

std::string Logger::getTimestamp()
{
    time_t rawTime = -1;
    if ( time(&rawTime) != -1 )
    {
        struct tm* timeInfo = localtime(&rawTime);

        char timeString[32] = {};
        if ( strftime(timeString, 32, "%Y-%m-%dT%H-%M-%SZ", timeInfo) > 0 )
            return std::string(timeString);
    }
    return std::string();
}

std::string Logger::getPrefix(LogLevel logLevel)
{
    switch ( logLevel )
    {
        case LogLevel::Fatal: return getTimestamp() + " FATAL: ";
        case LogLevel::Error: return getTimestamp() + " ERROR: ";
        case LogLevel::Warn: return getTimestamp() + " WARN: ";
        case LogLevel::Info: return getTimestamp() + " INFO: ";
        case LogLevel::Debug: return getTimestamp() + " DEBUG: ";
        case LogLevel::Trace: return getTimestamp() + " TRACE: ";
        default: return getTimestamp() + " LEVEL[" + std::to_string(logLevel) + "]: ";
    }
}

Logger & Logger::log(LogLevel logLevel)
{
    streamLogLevel = logLevel;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->log(streamLogLevel);

    if ( logLevel <= this->logLevel && outputStream != nullptr )
        *outputStream << getPrefix(logLevel);

    return *this;
}

Logger & Logger::fatal()
{
    streamLogLevel = LogLevel::Fatal;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->fatal();

    if ( LogLevel::Fatal <= logLevel && outputStream != nullptr )
        *outputStream << getPrefix(LogLevel::Fatal);

    return *this;
}

Logger & Logger::error()
{
    streamLogLevel = LogLevel::Error;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->error();

    if ( LogLevel::Error <= logLevel && outputStream != nullptr )
        *outputStream << getPrefix(LogLevel::Error);

    return *this;
}

Logger & Logger::warn()
{
    streamLogLevel = LogLevel::Warn;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->warn();

    if ( LogLevel::Warn <= logLevel && outputStream != nullptr )
        *outputStream << getPrefix(LogLevel::Warn);
        
    return *this;
}

Logger & Logger::info()
{
    streamLogLevel = LogLevel::Info;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->info();

    if ( LogLevel::Info <= logLevel && outputStream != nullptr )
        *outputStream << getPrefix(LogLevel::Info);
    
    return *this;
}

Logger & Logger::debug()
{
    streamLogLevel = LogLevel::Debug;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->debug();

    if ( LogLevel::Debug <= logLevel && outputStream != nullptr )
        *outputStream << getPrefix(LogLevel::Debug);

    return *this;
}

Logger & Logger::trace()
{
    streamLogLevel = LogLevel::Trace;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->trace();

    if ( LogLevel::Trace <= logLevel && outputStream != nullptr )
        *outputStream << getPrefix(LogLevel::Trace);
        
    return *this;
}

std::shared_ptr<std::ostream> Logger::getDefaultOutputStream()
{
    return std::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){});
}

int Logger::sync()
{
    if ( outputStream != nullptr )
        outputStream->flush();

    return aggregator != nullptr ? aggregator->sync() : 0;
}

int Logger::overflow(int c)
{
#ifdef _WIN32
    if ( c == '\r' ) // Text read from windows will occasionally have \r\n, the \n automatically becomes \r\n so the output becomes \r\r\n, skip \r to fix
        return 0;
#endif

    if ( outputStream != nullptr && streamLogLevel <= logLevel && streamLogLevel > LogLevel::Off )
        outputStream->put(c);

    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->overflowAggregator(c, streamLogLevel);

    return 0;
}

void Logger::overflowAggregator(int c, LogLevel sourceStreamLogLevel)
{
    if ( outputStream != nullptr && sourceStreamLogLevel <= logLevel && logLevel > LogLevel::Off )
        outputStream->put(c);

    if ( aggregator != nullptr && sourceStreamLogLevel <= aggregator->logLevel )
        aggregator->overflowAggregator(c, sourceStreamLogLevel);
}
