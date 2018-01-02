#ifndef LOGGER_H
#define LOGGER_H
#include <exception>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

enum class LogLevel : uint32_t {
    Off = 0,
    None = 0,
    Fatal = 100,
    Error = 200,
    Warn = 300,
    Info = 400,
    Debug = 500,
    Trace = 600,
    All = UINT32_MAX
};

class Logger;
class Logger
{
    public:
        Logger();
        Logger(LogLevel logLevel);
        Logger(Logger* aggregator);
        Logger(std::shared_ptr<std::ostream> outputStream);
        Logger(std::shared_ptr<std::ostream> outputStream, Logger* aggregator);
        Logger(std::shared_ptr<std::ostream> outputStream, LogLevel logLevel);
        Logger(std::shared_ptr<std::ostream> outputStream, LogLevel logLevel, Logger* aggregator);

        void setOutputStream(std::shared_ptr<std::ostream> outputStream);
        void setAggregator(Logger* aggregator);
        void setLogLevel(LogLevel logLevel);

        template <typename T> void log(T& message, LogLevel logLevel);
        template <typename T> void log(T& message, std::exception& e, LogLevel logLevel);
        template <typename T> void fatal(T& message);
        template <typename T> void fatal(T& message, std::exception& e);
        template <typename T> void error(T& message);
        template <typename T> void error(T& message, std::exception& e);
        template <typename T> void warn(T& message);
        template <typename T> void warn(T& message, std::exception& e);
        template <typename T> void info(T& message);
        template <typename T> void info(T& message, std::exception& e);
        template <typename T> void debug(T& message);
        template <typename T> void debug(T& message, std::exception& e);
        template <typename T> void trace(T& message);
        template <typename T> void trace(T& message, std::exception& e);
        
        std::string getPrefix(LogLevel logLevel);
        static std::string getTimestamp(); // 2017-07-16T11:24:34Z

        

    private:
        LogLevel maximumLogLevel;
        std::shared_ptr<std::ostream> outputStream;
        Logger* aggregator;
        std::vector<Logger*> aggregates;
};

template <typename T> void Logger::log(T& message, LogLevel logLevel)
{
    if ( outputStream != nullptr && logLevel <= maximumLogLevel )
        *outputStream << getPrefix(logLevel) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->log(message, logLevel);
}

template <typename T> void Logger::log(T& message, std::exception& e, LogLevel logLevel)
{
    if ( outputStream != nullptr && logLevel <= maximumLogLevel )
        *outputStream << getPrefix(logLevel) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->log(message, logLevel);
}

template <typename T> void Logger::fatal(T& message)
{
    if ( outputStream != nullptr && LogLevel::Fatal <= maximumLogLevel )
        *outputStream << getPrefix(LogLevel::Fatal) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->fatal(message);
}

template <typename T> void Logger::fatal(T& message, std::exception& e)
{
    if ( outputStream != nullptr && LogLevel::Fatal <= maximumLogLevel )
        *outputStream << getPrefix(LogLevel::Fatal) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->fatal(message, e);
}

template <typename T> void Logger::error(T& message)
{
    if ( outputStream != nullptr && LogLevel::Error <= maximumLogLevel )
        *outputStream << getPrefix(LogLevel::Error) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->error(message);
}

template <typename T> void Logger::error(T& message, std::exception& e)
{
    if ( outputStream != nullptr && LogLevel::Error <= maximumLogLevel )
        *outputStream << getPrefix(LogLevel::Error) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->error(message, e);
}

template <typename T> void Logger::warn(T& message)
{
    if ( outputStream != nullptr && LogLevel::Warn <= maximumLogLevel )
        *outputStream << getPrefix(LogLevel::Warn) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->warn(message);
}

template <typename T> void Logger::warn(T& message, std::exception& e)
{
    if ( outputStream != nullptr && LogLevel::Warn <= maximumLogLevel )
        *outputStream << getPrefix(LogLevel::Warn) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->warn(message, e);
}

template <typename T> void Logger::info(T& message)
{
    if ( outputStream != nullptr && LogLevel::Info <= maximumLogLevel )
        *outputStream << getPrefix(LogLevel::Info) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->info(message);
}

template <typename T> void Logger::info(T& message, std::exception& e)
{
    if ( outputStream != nullptr && LogLevel::Info <= maximumLogLevel )
        *outputStream << getPrefix(LogLevel::Info) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->info(message, e);
}

template <typename T> void Logger::debug(T& message)
{
    if ( outputStream != nullptr && LogLevel::Debug <= maximumLogLevel )
        *outputStream << getPrefix(LogLevel::Debug) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->debug(message);
}

template <typename T> void Logger::debug(T& message, std::exception& e)
{
    if ( outputStream != nullptr && LogLevel::Debug <= maximumLogLevel )
        *outputStream << getPrefix(LogLevel::Debug) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->debug(message, e);
}

template <typename T> void Logger::trace(T& message)
{
    if ( outputStream != nullptr && LogLevel::Trace <= maximumLogLevel )
        *outputStream << getPrefix(LogLevel::Trace) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->trace(message);
}

template <typename T> void Logger::trace(T& message, std::exception& e)
{
    if ( outputStream != nullptr && LogLevel::Trace <= maximumLogLevel )
        *outputStream << getPrefix(LogLevel::Trace) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->trace(message, e);
}

#endif