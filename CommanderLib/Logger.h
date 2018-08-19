#ifndef LOGGER_H
#define LOGGER_H
#include <exception>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

enum class LogLevel : uint32_t {
    Off = 0,
    None = 0,
    Fatal = 100,
    Error = 200,
    Warn = 300,
    Info = 400,
    Debug = 500,
    Trace = 600,
    All = UINT32_MAX,
    Default = Info
};

class Logger;
class Logger : public std::ostream, public std::streambuf
{
    public:
        Logger(LogLevel logLevel = LogLevel::Default);
        Logger(uint32_t logLevel);
        Logger(std::ostream &outputStream, LogLevel logLevel = LogLevel::Default);
        Logger(std::ostream &outputStream, uint32_t logLevel);
        Logger(std::ostream &outputStream, Logger &aggregator, LogLevel logLevel = LogLevel::Default);
        Logger(std::ostream &outputStream, Logger &aggregator, uint32_t logLevel);
        Logger(std::ostream &outputStream, std::shared_ptr<Logger> aggregator, LogLevel logLevel = LogLevel::Default);
        Logger(std::ostream &outputStream, std::shared_ptr<Logger> aggregator, uint32_t logLevel);
        Logger(std::shared_ptr<std::ostream> outputStream, LogLevel logLevel = LogLevel::Default);
        Logger(std::shared_ptr<std::ostream> outputStream, uint32_t logLevel);
        Logger(std::shared_ptr<std::ostream> outputStream, Logger &aggregator, LogLevel logLevel = LogLevel::Default);
        Logger(std::shared_ptr<std::ostream> outputStream, Logger &aggregator, uint32_t logLevel);
        Logger(std::shared_ptr<std::ostream> outputStream, std::shared_ptr<Logger> aggregator, LogLevel logLevel = LogLevel::Default);
        Logger(std::shared_ptr<std::ostream> outputStream, std::shared_ptr<Logger> aggregator, uint32_t logLevel);
        Logger(Logger &aggregator, LogLevel logLevel = LogLevel::Default);
        Logger(Logger &aggregator, uint32_t logLevel);
        Logger(std::shared_ptr<Logger> aggregator, LogLevel logLevel = LogLevel::Default);
        Logger(std::shared_ptr<Logger> aggregator, uint32_t logLevel);
        virtual ~Logger();

        uint32_t getLogLevel();
        std::shared_ptr<std::ostream> getOutputStream();
        std::shared_ptr<Logger> getAggregator();

        void setLogLevel(LogLevel logLevel);
        void setLogLevel(uint32_t logLevel);
        void setOutputStream(std::ostream &outputStream);
        void setOutputStream(std::shared_ptr<std::ostream> outputStream);
        void setAggregator(Logger &logger);
        void setAggregator(std::shared_ptr<Logger> aggregator);

        static std::string getTimestamp(); // Gets a timestamp string in the format "YY-MM-DDThh-mm-ssZ"
        std::string getPrefix(LogLevel logLevel); // Gets a timestamp followed by the log level representation, a colon, and a space
        std::string getPrefix(uint32_t logLevel); // Gets a timestamp followed by the log level representation, a colon, and a space

        Logger & log(LogLevel logLevel);
        Logger & log(uint32_t logLevel);
        Logger & fatal();
        Logger & error();
        Logger & warn();
        Logger & info();
        Logger & debug();
        Logger & trace();

        template <typename T> void log(LogLevel logLevel, const T& message);
        template <typename T> void log(LogLevel logLevel, const T& message, const std::exception& e);
        template <typename T> void log(uint32_t logLevel, const T& message);
        template <typename T> void log(uint32_t logLevel, const T& message, const std::exception& e);
        template <typename T> void fatal(const T& message);
        template <typename T> void fatal(const T& message, const std::exception& e);
        template <typename T> void error(const T& message);
        template <typename T> void error(const T& message, const std::exception& e);
        template <typename T> void warn(const T& message);
        template <typename T> void warn(const T& message, const std::exception& e);
        template <typename T> void info(const T& message);
        template <typename T> void info(const T& message, const std::exception& e);
        template <typename T> void debug(const T& message);
        template <typename T> void debug(const T& message, const std::exception& e);
        template <typename T> void trace(const T& message);
        template <typename T> void trace(const T& message, const std::exception& e);

        static std::shared_ptr<std::ostream> getDefaultOutputStream(); // Gets a reference to the default output stream (std::out)

    protected:
        int overflow(int c);
        void overflowAggregator(int c, uint32_t sourceStreamLogLevel);

    private:
        uint32_t logLevel;
        uint32_t streamLogLevel;
        std::shared_ptr<std::ostream> outputStream;
        std::shared_ptr<Logger> aggregator;
};

template <typename T> void Logger::log(LogLevel logLevel, const T& message)
{
    if ( outputStream != nullptr && (uint32_t)logLevel <= this->logLevel )
        *outputStream << getPrefix((uint32_t)logLevel) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->log((uint32_t)logLevel, message);
}

template <typename T> void Logger::log(LogLevel logLevel, const T& message, const std::exception& e)
{
    if ( outputStream != nullptr && (uint32_t)logLevel <= this->logLevel )
        *outputStream << getPrefix((uint32_t)logLevel) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->log((uint32_t)logLevel, message, e);
}

template <typename T> void Logger::log(uint32_t logLevel, const T& message)
{
    if ( outputStream != nullptr && logLevel <= this->logLevel )
        *outputStream << getPrefix(logLevel) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->log(logLevel, message);
}

template <typename T> void Logger::log(uint32_t logLevel, const T& message, const std::exception& e)
{
    if ( outputStream != nullptr && logLevel <= this->logLevel )
        *outputStream << getPrefix(logLevel) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->log(logLevel, message, e);
}

template <typename T> void Logger::fatal(const T& message)
{
    if ( outputStream != nullptr && (uint32_t)LogLevel::Fatal <= logLevel )
        *outputStream << getPrefix((uint32_t)LogLevel::Fatal) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->fatal(message);
}

template <typename T> void Logger::fatal(const T& message, const std::exception& e)
{
    if ( outputStream != nullptr && (uint32_t)LogLevel::Fatal <= logLevel )
        *outputStream << getPrefix((uint32_t)LogLevel::Fatal) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->fatal(message, e);
}

template <typename T> void Logger::error(const T& message)
{
    if ( outputStream != nullptr && (uint32_t)LogLevel::Error <= logLevel )
        *outputStream << getPrefix((uint32_t)LogLevel::Error) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->error(message);
}

template <typename T> void Logger::error(const T& message, const std::exception& e)
{
    if ( outputStream != nullptr && (uint32_t)LogLevel::Error <= logLevel )
        *outputStream << getPrefix((uint32_t)LogLevel::Error) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->error(message, e);
}

template <typename T> void Logger::warn(const T& message)
{
    if ( outputStream != nullptr && (uint32_t)LogLevel::Warn <= logLevel )
        *outputStream << getPrefix((uint32_t)LogLevel::Warn) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->warn(message);
}

template <typename T> void Logger::warn(const T& message, const std::exception& e)
{
    if ( outputStream != nullptr && (uint32_t)LogLevel::Warn <= logLevel )
        *outputStream << getPrefix((uint32_t)LogLevel::Warn) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->warn(message, e);
}

template <typename T> void Logger::info(const T& message)
{
    if ( outputStream != nullptr && (uint32_t)LogLevel::Info <= logLevel )
        *outputStream << getPrefix((uint32_t)LogLevel::Info) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->info(message);
}

template <typename T> void Logger::info(const T& message, const std::exception& e)
{
    if ( outputStream != nullptr && (uint32_t)LogLevel::Info <= logLevel )
        *outputStream << getPrefix((uint32_t)LogLevel::Info) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->info(message, e);
}

template <typename T> void Logger::debug(const T& message)
{
    if ( outputStream != nullptr && (uint32_t)LogLevel::Debug <= logLevel )
        *outputStream << getPrefix((uint32_t)LogLevel::Debug) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->debug(message);
}

template <typename T> void Logger::debug(const T& message, const std::exception& e)
{
    if ( outputStream != nullptr && (uint32_t)LogLevel::Debug <= logLevel )
        *outputStream << getPrefix((uint32_t)LogLevel::Debug) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->debug(message, e);
}

template <typename T> void Logger::trace(const T& message)
{
    if ( outputStream != nullptr && (uint32_t)LogLevel::Trace <= logLevel )
        *outputStream << getPrefix((uint32_t)LogLevel::Trace) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->trace(message);
}

template <typename T> void Logger::trace(const T& message, const std::exception& e)
{
    if ( outputStream != nullptr && (uint32_t)LogLevel::Trace <= logLevel )
        *outputStream << getPrefix((uint32_t)LogLevel::Trace) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->trace(message, e);
}

#endif