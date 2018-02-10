#include "Logger.h"
#include <string>
#include <ctime>

Logger::Logger() : logLevel(LogLevel::Info), outputStream(std::shared_ptr<std::ostream>(&std::cout, [](std::ostream* os) {})), aggregator(nullptr)
{

}

Logger::Logger(LogLevel logLevel) : logLevel(logLevel), outputStream(nullptr), aggregator(nullptr)
{

}

Logger::Logger(LogLevel logLevel, std::shared_ptr<std::ostream> outputStream) : logLevel(logLevel), outputStream(outputStream), aggregator(nullptr)
{

}

Logger::Logger(LogLevel logLevel, std::shared_ptr<std::ostream> outputStream, std::shared_ptr<Logger> aggregator) : logLevel(logLevel), outputStream(outputStream), aggregator(aggregator)
{

}

Logger::Logger(LogLevel logLevel, std::shared_ptr<Logger> aggregator) : logLevel(logLevel), outputStream(nullptr), aggregator(aggregator)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream) : logLevel(LogLevel::Info), outputStream(outputStream), aggregator(nullptr)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, std::shared_ptr<Logger> aggregator) : logLevel(LogLevel::Info), outputStream(outputStream), aggregator(aggregator)
{

}

Logger::Logger(std::shared_ptr<Logger> aggregator) : logLevel(LogLevel::Info), outputStream(nullptr), aggregator(aggregator)
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
}

void Logger::setOutputStream(std::shared_ptr<std::ostream> outputStream)
{
    this->outputStream = outputStream;
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
        default: return getTimestamp() + " LEVEL[" + std::to_string((uint32_t)logLevel) + "]: ";
    }
}

std::ostream & Logger::os()
{
    return this->outputStream != nullptr ? *this->outputStream : std::cout;
}

std::ostream & Logger::log(LogLevel logLevel)
{
    std::ostream & outputStream = this->outputStream != nullptr ? *this->outputStream : std::cout;
    outputStream << getPrefix(logLevel);
    return outputStream;
}

std::ostream & Logger::fatal()
{
    std::ostream & outputStream = this->outputStream != nullptr ? *this->outputStream : std::cout;
    outputStream << getPrefix(LogLevel::Fatal);
    return outputStream;
}

std::ostream & Logger::error()
{
    std::ostream & outputStream = this->outputStream != nullptr ? *this->outputStream : std::cout;
    outputStream << getPrefix(LogLevel::Error);
    return outputStream;
}

std::ostream & Logger::warn()
{
    std::ostream & outputStream = this->outputStream != nullptr ? *this->outputStream : std::cout;
    outputStream << getPrefix(LogLevel::Warn);
    return outputStream;
}

std::ostream & Logger::info()
{
    std::ostream & outputStream = this->outputStream != nullptr ? *this->outputStream : std::cout;
    outputStream << getPrefix(LogLevel::Info);
    return outputStream;
}

std::ostream & Logger::debug()
{
    std::ostream & outputStream = this->outputStream != nullptr ? *this->outputStream : std::cout;
    outputStream << getPrefix(LogLevel::Debug);
    return outputStream;
}

std::ostream & Logger::trace()
{
    std::ostream & outputStream = this->outputStream != nullptr ? *this->outputStream : std::cout;
    outputStream << getPrefix(LogLevel::Trace);
    return outputStream;
}

