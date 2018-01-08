#include "Logger.h"
#include <string>
#include <ctime>

Logger::Logger() : outputStream(nullptr), maximumLogLevel(LogLevel::Info)
{

}

Logger::Logger(LogLevel logLevel) : outputStream(nullptr), maximumLogLevel(logLevel)
{

}

Logger::Logger(Logger* aggregator) : outputStream(nullptr), maximumLogLevel(LogLevel::Info), aggregator(aggregator)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream) : outputStream(outputStream), maximumLogLevel(LogLevel::Info)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, Logger* aggregator) : outputStream(outputStream), maximumLogLevel(LogLevel::Info), aggregator(aggregator)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, LogLevel logLevel) : outputStream(outputStream), maximumLogLevel(logLevel)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, LogLevel logLevel, Logger* aggregator) : outputStream(outputStream), maximumLogLevel(logLevel), aggregator(aggregator)
{

}

void Logger::setOutputStream(std::shared_ptr<std::ostream> outputStream)
{
    this->outputStream = outputStream;
}

void Logger::setAggregator(Logger* aggregator)
{
    this->aggregator = aggregator;
}

void Logger::setLogLevel(LogLevel logLevel)
{
    this->maximumLogLevel = logLevel;
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

std::string Logger::getTimestamp()
{
    time_t rawTime = {};
    struct tm* timeInfo;
    
    time(&rawTime);
    timeInfo = localtime(&rawTime);

    int year = timeInfo->tm_year+1900;
    int month = timeInfo->tm_mon+1;
    int day = timeInfo->tm_mday;

    char timeString[32] = {};
    strftime(timeString, 32, "%Y-%m-%dT%H-%M-%SZ", timeInfo);

    return std::string(timeString);
}
