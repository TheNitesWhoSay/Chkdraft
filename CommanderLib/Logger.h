#ifndef LOGGER_H
#define LOGGER_H
#include <exception>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#ifndef enum_t
/**
    enum_t "enum type (scoped)" assumes the property of enum classes that encloses the enum values within a particular scope
    e.g. MyClass::MyEnum::Value cannot be accessed via MyClass::Value (as it could with regular enums) and potentially cause redefinition errors
    while avoiding the property of enum classes that removes the one-to-one relationship with the underlying type (which forces excessive type-casting)

    Usage:
    enum_t(name, type, {
        enumerator = constexpr,
        enumerator = constexpr,
        ...
    });
*/
#pragma warning(disable: 26812) // In the context of using enum_t, enum class is definitely not preferred, disable the warning in visual studios

/** enum_t "enum type (scoped)" documentation minimized for expansion visibility, see definition for description and usage */
#define enum_t(name, type, ...) struct name ## _ { enum type ## _ : type __VA_ARGS__; }; using name = name ## _::type ## _;
#endif

enum_t(LogLevel, uint32_t, {
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
});

class Logger;
class Logger : public std::ostream, public std::streambuf
{
    public:
        Logger(LogLevel logLevel = LogLevel::Default);
        Logger(std::ostream & outputStream, LogLevel logLevel = LogLevel::Default);
        Logger(std::ostream & outputStream, Logger & aggregator, LogLevel logLevel = LogLevel::Default);
        Logger(std::ostream & outputStream, std::shared_ptr<Logger> aggregator, LogLevel logLevel = LogLevel::Default);
        Logger(std::shared_ptr<std::ostream> outputStream, LogLevel logLevel = LogLevel::Default);
        Logger(std::shared_ptr<std::ostream> outputStream, Logger & aggregator, LogLevel logLevel = LogLevel::Default);
        Logger(std::shared_ptr<std::ostream> outputStream, std::shared_ptr<Logger> aggregator, LogLevel logLevel = LogLevel::Default);
        Logger(Logger & aggregator, LogLevel logLevel = LogLevel::Default);
        Logger(std::shared_ptr<Logger> aggregator, LogLevel logLevel = LogLevel::Default);
        virtual ~Logger();

        LogLevel getLogLevel();
        std::shared_ptr<std::ostream> getOutputStream();
        std::shared_ptr<Logger> getAggregator();

        void setLogLevel(LogLevel logLevel);
        void setOutputStream(std::ostream & outputStream);
        void setOutputStream(std::shared_ptr<std::ostream> outputStream);
        void setAggregator(Logger & logger);
        void setAggregator(std::shared_ptr<Logger> aggregator);

        static std::string getTimestamp(); // Gets a timestamp string in the format "YY-MM-DDThh-mm-ssZ"
        std::string getPrefix(LogLevel logLevel); // Gets a timestamp followed by the log level representation, a colon, and a space

        Logger & log(LogLevel logLevel);
        Logger & fatal();
        Logger & error();
        Logger & warn();
        Logger & info();
        Logger & debug();
        Logger & trace();

        template <typename T> void log(LogLevel logLevel, const T & message);
        template <typename T> void log(LogLevel logLevel, const T & message, const std::exception & e);
        template <typename T> void fatal(const T & message);
        template <typename T> void fatal(const T & message, const std::exception & e);
        template <typename T> void error(const T & message);
        template <typename T> void error(const T & message, const std::exception & e);
        template <typename T> void warn(const T & message);
        template <typename T> void warn(const T & message, const std::exception & e);
        template <typename T> void info(const T & message);
        template <typename T> void info(const T & message, const std::exception & e);
        template <typename T> void debug(const T & message);
        template <typename T> void debug(const T & message, const std::exception & e);
        template <typename T> void trace(const T & message);
        template <typename T> void trace(const T & message, const std::exception & e);

        static std::shared_ptr<std::ostream> getDefaultOutputStream(); // Gets a reference to the default output stream (std::out)

    protected:
        virtual int sync();
        virtual int overflow(int c);
        void overflowAggregator(int c, LogLevel sourceStreamLogLevel);

    private:
        LogLevel logLevel;
        LogLevel streamLogLevel;
        std::shared_ptr<std::ostream> outputStream;
        std::shared_ptr<Logger> aggregator;
};

template <typename T> void Logger::log(LogLevel logLevel, const T & message)
{
    if ( outputStream != nullptr && logLevel <= this->logLevel )
        *outputStream << getPrefix(logLevel) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->log(logLevel, message);
}

template <typename T> void Logger::log(LogLevel logLevel, const T & message, const std::exception & e)
{
    if ( outputStream != nullptr && logLevel <= this->logLevel )
        *outputStream << getPrefix(logLevel) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->log(logLevel, message, e);
}

template <typename T> void Logger::fatal(const T & message)
{
    if ( outputStream != nullptr && LogLevel::Fatal <= logLevel )
        *outputStream << getPrefix(LogLevel::Fatal) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->fatal(message);
}

template <typename T> void Logger::fatal(const T & message, const std::exception & e)
{
    if ( outputStream != nullptr && LogLevel::Fatal <= logLevel )
        *outputStream << getPrefix(LogLevel::Fatal) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->fatal(message, e);
}

template <typename T> void Logger::error(const T & message)
{
    if ( outputStream != nullptr && LogLevel::Error <= logLevel )
        *outputStream << getPrefix(LogLevel::Error) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->error(message);
}

template <typename T> void Logger::error(const T & message, const std::exception & e)
{
    if ( outputStream != nullptr && LogLevel::Error <= logLevel )
        *outputStream << getPrefix(LogLevel::Error) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->error(message, e);
}

template <typename T> void Logger::warn(const T & message)
{
    if ( outputStream != nullptr && LogLevel::Warn <= logLevel )
        *outputStream << getPrefix(LogLevel::Warn) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->warn(message);
}

template <typename T> void Logger::warn(const T & message, const std::exception & e)
{
    if ( outputStream != nullptr && LogLevel::Warn <= logLevel )
        *outputStream << getPrefix(LogLevel::Warn) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->warn(message, e);
}

template <typename T> void Logger::info(const T & message)
{
    if ( outputStream != nullptr && LogLevel::Info <= logLevel )
        *outputStream << getPrefix(LogLevel::Info) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->info(message);
}

template <typename T> void Logger::info(const T & message, const std::exception & e)
{
    if ( outputStream != nullptr && LogLevel::Info <= logLevel )
        *outputStream << getPrefix(LogLevel::Info) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->info(message, e);
}

template <typename T> void Logger::debug(const T & message)
{
    if ( outputStream != nullptr && LogLevel::Debug <= logLevel )
        *outputStream << getPrefix(LogLevel::Debug) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->debug(message);
}

template <typename T> void Logger::debug(const T & message, const std::exception & e)
{
    if ( outputStream != nullptr && LogLevel::Debug <= logLevel )
        *outputStream << getPrefix(LogLevel::Debug) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->debug(message, e);
}

template <typename T> void Logger::trace(const T & message)
{
    if ( outputStream != nullptr && LogLevel::Trace <= logLevel )
        *outputStream << getPrefix(LogLevel::Trace) << message << std::endl;
    if ( aggregator != nullptr )
        aggregator->trace(message);
}

template <typename T> void Logger::trace(const T & message, const std::exception & e)
{
    if ( outputStream != nullptr && LogLevel::Trace <= logLevel )
        *outputStream << getPrefix(LogLevel::Trace) << message << std::endl << e.what() << std::endl;
    if ( aggregator != nullptr )
        aggregator->trace(message, e);
}

#endif