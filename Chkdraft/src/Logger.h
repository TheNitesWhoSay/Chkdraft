#ifndef LOGGER_H
#define LOGGER_H
#include <exception>
#include <cstdint>
#include <iostream>
#include <fstream>
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
        Logger(LogLevel logLevel, std::shared_ptr<std::ostream> outputStream);
        Logger(LogLevel logLevel, std::shared_ptr<std::ostream> outputStream, std::shared_ptr<Logger> aggregator);
        Logger(LogLevel logLevel, std::shared_ptr<Logger> aggregator);
        Logger(std::shared_ptr<std::ostream> outputStream);
        Logger(std::shared_ptr<std::ostream> outputStream, std::shared_ptr<Logger> aggregator);
        Logger(std::shared_ptr<Logger> aggregator);
        virtual ~Logger();

        LogLevel getLogLevel();
        std::shared_ptr<std::ostream> getOutputStream();
        std::shared_ptr<Logger> getAggregator();

        void setLogLevel(LogLevel logLevel);
        void setOutputStream(std::shared_ptr<std::ostream> outputStream);
        void setAggregator(std::shared_ptr<Logger> aggregator);

        static std::string getTimestamp();
        std::string getPrefix(LogLevel logLevel);

        std::ostream & os(); // Gets a reference to the current output stream, or std::out if no output stream is set
        std::ostream & log(LogLevel logLevel); // Prints the log header and gets a reference to the current output stream, or std::out if no output stream is set
        std::ostream & fatal(); // Prints the fatal header and gets a reference to the current output stream, or std::out if no output stream is set
        std::ostream & error(); // Prints the error header and gets a reference to the current output stream, or std::out if no output stream is set
        std::ostream & warn(); // Prints the warn header and gets a reference to the current output stream, or std::out if no output stream is set
        std::ostream & info(); // Prints the info header and gets a reference to the current output stream, or std::out if no output stream is set
        std::ostream & debug(); // Prints the debug header and gets a reference to the current output stream, or std::out if no output stream is set
        std::ostream & trace(); // Prints the trace header and gets a reference to the current output stream, or std::out if no output stream is set

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


    private:
        LogLevel logLevel;
        std::shared_ptr<std::ostream> outputStream;
        std::shared_ptr<Logger> aggregator;
};

template <typename T> void Logger::log(T& message, LogLevel logLevel)
{
    if ( outputStream != nullptr && logLevel <= this->logLevel )
        *outputStream << getPrefix(logLevel) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->log(message, logLevel);
}

template <typename T> void Logger::log(T& message, std::exception& e, LogLevel logLevel)
{
    if ( outputStream != nullptr && logLevel <= this->logLevel )
        *outputStream << getPrefix(logLevel) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->log(message, logLevel);
}

template <typename T> void Logger::fatal(T& message)
{
    if ( outputStream != nullptr && LogLevel::Fatal <= logLevel )
        *outputStream << getPrefix(LogLevel::Fatal) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->fatal(message);
}

template <typename T> void Logger::fatal(T& message, std::exception& e)
{
    if ( outputStream != nullptr && LogLevel::Fatal <= logLevel )
        *outputStream << getPrefix(LogLevel::Fatal) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->fatal(message, e);
}

template <typename T> void Logger::error(T& message)
{
    if ( outputStream != nullptr && LogLevel::Error <= logLevel )
        *outputStream << getPrefix(LogLevel::Error) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->error(message);
}

template <typename T> void Logger::error(T& message, std::exception& e)
{
    if ( outputStream != nullptr && LogLevel::Error <= logLevel )
        *outputStream << getPrefix(LogLevel::Error) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->error(message, e);
}

template <typename T> void Logger::warn(T& message)
{
    if ( outputStream != nullptr && LogLevel::Warn <= logLevel )
        *outputStream << getPrefix(LogLevel::Warn) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->warn(message);
}

template <typename T> void Logger::warn(T& message, std::exception& e)
{
    if ( outputStream != nullptr && LogLevel::Warn <= logLevel )
        *outputStream << getPrefix(LogLevel::Warn) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->warn(message, e);
}

template <typename T> void Logger::info(T& message)
{
    if ( outputStream != nullptr && LogLevel::Info <= logLevel )
        *outputStream << getPrefix(LogLevel::Info) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->info(message);
}

template <typename T> void Logger::info(T& message, std::exception& e)
{
    if ( outputStream != nullptr && LogLevel::Info <= logLevel )
        *outputStream << getPrefix(LogLevel::Info) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->info(message, e);
}

template <typename T> void Logger::debug(T& message)
{
    if ( outputStream != nullptr && LogLevel::Debug <= logLevel )
        *outputStream << getPrefix(LogLevel::Debug) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->debug(message);
}

template <typename T> void Logger::debug(T& message, std::exception& e)
{
    if ( outputStream != nullptr && LogLevel::Debug <= logLevel )
        *outputStream << getPrefix(LogLevel::Debug) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->debug(message, e);
}

template <typename T> void Logger::trace(T& message)
{
    if ( outputStream != nullptr && LogLevel::Trace <= logLevel )
        *outputStream << getPrefix(LogLevel::Trace) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->trace(message);
}

template <typename T> void Logger::trace(T& message, std::exception& e)
{
    if ( outputStream != nullptr && LogLevel::Trace <= logLevel )
        *outputStream << getPrefix(LogLevel::Trace) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->trace(message, e);
}

#endif