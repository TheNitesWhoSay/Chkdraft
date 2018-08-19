#include "Logger.h"
#include <string>
#include <ctime>

Logger::Logger(LogLevel logLevel) :
    logLevel((uint32_t)logLevel), outputStream(std::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){})), aggregator(nullptr),
    streamLogLevel((uint32_t)logLevel), std::ostream(this)
{

}

Logger::Logger(uint32_t logLevel) :
    logLevel(logLevel), outputStream(std::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){})), aggregator(nullptr),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::ostream &outputStream, LogLevel logLevel) :
    logLevel((uint32_t)logLevel), outputStream(std::shared_ptr<std::ostream>(&outputStream, [](std::ostream*){})), aggregator(nullptr),
    streamLogLevel((uint32_t)logLevel), std::ostream(this)
{

}

Logger::Logger(std::ostream &outputStream, uint32_t logLevel) :
    logLevel(logLevel), outputStream(std::shared_ptr<std::ostream>(&outputStream, [](std::ostream*){})), aggregator(nullptr),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::ostream &outputStream, Logger &aggregator, LogLevel logLevel) :
    logLevel((uint32_t)logLevel), outputStream(std::shared_ptr<std::ostream>(&outputStream, [](std::ostream*){})), aggregator(std::shared_ptr<Logger>(&aggregator, [](Logger*){})),
    streamLogLevel((uint32_t)logLevel), std::ostream(this)
{

}

Logger::Logger(std::ostream &outputStream, Logger &aggregator, uint32_t logLevel) :
    logLevel(logLevel), outputStream(std::shared_ptr<std::ostream>(&outputStream, [](std::ostream*){})), aggregator(std::shared_ptr<Logger>(&aggregator, [](Logger*){})),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::ostream &outputStream, std::shared_ptr<Logger> aggregator, LogLevel logLevel) :
    logLevel((uint32_t)logLevel), outputStream(std::shared_ptr<std::ostream>(&outputStream, [](std::ostream*){})), aggregator(aggregator),
    streamLogLevel((uint32_t)logLevel), std::ostream(this)
{

}

Logger::Logger(std::ostream &outputStream, std::shared_ptr<Logger> aggregator, uint32_t logLevel) :
    logLevel(logLevel), outputStream(std::shared_ptr<std::ostream>(&outputStream, [](std::ostream*){})), aggregator(aggregator),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, LogLevel logLevel) :
    logLevel((uint32_t)logLevel), outputStream(outputStream), aggregator(nullptr),
    streamLogLevel((uint32_t)logLevel), std::ostream(this)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, uint32_t logLevel) :
    logLevel(logLevel), outputStream(outputStream), aggregator(nullptr),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, Logger &aggregator, LogLevel logLevel) :
    logLevel((uint32_t)logLevel), outputStream(outputStream), aggregator(std::shared_ptr<Logger>(&aggregator, [](Logger*){})),
    streamLogLevel((uint32_t)logLevel), std::ostream(this)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, Logger &aggregator, uint32_t logLevel) :
    logLevel(logLevel), outputStream(outputStream), aggregator(std::shared_ptr<Logger>(&aggregator, [](Logger*){})),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, std::shared_ptr<Logger> aggregator, LogLevel logLevel) :
    logLevel((uint32_t)logLevel), outputStream(outputStream), aggregator(aggregator),
    streamLogLevel((uint32_t)logLevel), std::ostream(this)
{

}

Logger::Logger(std::shared_ptr<std::ostream> outputStream, std::shared_ptr<Logger> aggregator, uint32_t logLevel) :
    logLevel(logLevel), outputStream(outputStream), aggregator(aggregator),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(Logger &aggregator, LogLevel logLevel) :
    logLevel((uint32_t)logLevel), outputStream(std::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){})), aggregator(std::shared_ptr<Logger>(&aggregator, [](Logger*){})),
    streamLogLevel((uint32_t)logLevel), std::ostream(this)
{

}

Logger::Logger(Logger &aggregator, uint32_t logLevel) :
    logLevel(logLevel), outputStream(std::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){})), aggregator(std::shared_ptr<Logger>(&aggregator, [](Logger*){})),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::Logger(std::shared_ptr<Logger> aggregator, LogLevel logLevel) :
    logLevel((uint32_t)logLevel), outputStream(std::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){})), aggregator(aggregator),
    streamLogLevel((uint32_t)logLevel), std::ostream(this)
{

}

Logger::Logger(std::shared_ptr<Logger> aggregator, uint32_t logLevel) :
    logLevel(logLevel), outputStream(std::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){})), aggregator(aggregator),
    streamLogLevel(logLevel), std::ostream(this)
{

}

Logger::~Logger()
{

}

uint32_t Logger::getLogLevel()
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
    this->logLevel = (uint32_t)logLevel;
    this->streamLogLevel = (uint32_t)logLevel;
}

void Logger::setLogLevel(uint32_t logLevel)
{
    this->logLevel = logLevel;
    this->streamLogLevel = logLevel;
}

void Logger::setOutputStream(std::ostream &outputStream)
{
    this->outputStream = std::shared_ptr<std::ostream>(&outputStream, [](std::ostream*){});
}

void Logger::setOutputStream(std::shared_ptr<std::ostream> outputStream)
{
    this->outputStream = outputStream;
}

void Logger::setAggregator(Logger &aggregator)
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
    return getPrefix((uint32_t)logLevel);
}

std::string Logger::getPrefix(uint32_t logLevel)
{
    switch ( logLevel )
    {
        case (uint32_t)LogLevel::Fatal: return getTimestamp() + " FATAL: ";
        case (uint32_t)LogLevel::Error: return getTimestamp() + " ERROR: ";
        case (uint32_t)LogLevel::Warn: return getTimestamp() + " WARN: ";
        case (uint32_t)LogLevel::Info: return getTimestamp() + " INFO: ";
        case (uint32_t)LogLevel::Debug: return getTimestamp() + " DEBUG: ";
        case (uint32_t)LogLevel::Trace: return getTimestamp() + " TRACE: ";
        default: return getTimestamp() + " LEVEL[" + std::to_string((uint32_t)logLevel) + "]: ";
    }
}

Logger & Logger::log(LogLevel logLevel)
{
    streamLogLevel = (uint32_t)logLevel;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->log(streamLogLevel);

    if ( (uint32_t)logLevel <= this->logLevel && outputStream != nullptr )
        *outputStream << getPrefix((uint32_t)logLevel);

    return *this;
}

Logger & Logger::log(uint32_t logLevel)
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
    streamLogLevel = (uint32_t)LogLevel::Fatal;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->fatal();

    if ( (uint32_t)LogLevel::Fatal <= logLevel && outputStream != nullptr )
        *outputStream << getPrefix((uint32_t)LogLevel::Fatal);

    return *this;
}

Logger & Logger::error()
{
    streamLogLevel = (uint32_t)LogLevel::Error;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->error();

    if ( (uint32_t)LogLevel::Error <= logLevel && outputStream != nullptr )
        *outputStream << getPrefix((uint32_t)LogLevel::Error);

    return *this;
}

Logger & Logger::warn()
{
    streamLogLevel = (uint32_t)LogLevel::Warn;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->warn();

    if ( (uint32_t)LogLevel::Warn <= logLevel && outputStream != nullptr )
        *outputStream << getPrefix((uint32_t)LogLevel::Warn);
        
    return *this;
}

Logger & Logger::info()
{
    streamLogLevel = (uint32_t)LogLevel::Info;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->info();

    if ( (uint32_t)LogLevel::Info <= logLevel && outputStream != nullptr )
        *outputStream << getPrefix((uint32_t)LogLevel::Info);
        
    return *this;
}

Logger & Logger::debug()
{
    streamLogLevel = (uint32_t)LogLevel::Debug;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->debug();

    if ( (uint32_t)LogLevel::Debug <= logLevel && outputStream != nullptr )
        *outputStream << getPrefix((uint32_t)LogLevel::Debug);

    return *this;
}

Logger & Logger::trace()
{
    streamLogLevel = (uint32_t)LogLevel::Trace;
    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->trace();

    if ( (uint32_t)LogLevel::Trace <= logLevel && outputStream != nullptr )
        *outputStream << getPrefix((uint32_t)LogLevel::Trace);
        
    return *this;
}

std::shared_ptr<std::ostream> Logger::getDefaultOutputStream()
{
    return std::shared_ptr<std::ostream>(&std::cout, [](std::ostream*){});
}

int Logger::overflow(int c)
{
    if ( outputStream != nullptr && streamLogLevel <= logLevel && streamLogLevel > (uint32_t)LogLevel::Off )
        outputStream->put(c);

    if ( aggregator != nullptr && streamLogLevel <= aggregator->logLevel )
        aggregator->overflowAggregator(c, streamLogLevel);

    return 0;
}

void Logger::overflowAggregator(int c, uint32_t sourceStreamLogLevel)
{
    if ( outputStream != nullptr && sourceStreamLogLevel <= logLevel && logLevel > (uint32_t)LogLevel::Off )
        outputStream->put(c);

    if ( aggregator != nullptr && sourceStreamLogLevel <= aggregator->logLevel )
        aggregator->overflowAggregator(c, sourceStreamLogLevel);
}
