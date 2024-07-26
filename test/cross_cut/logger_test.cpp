#include <gtest/gtest.h>
#include "../CrossCutLib/Logger.h"
#include <regex>

constexpr const char* testExceptionWhatString = "TEST EXCEPTION";

class TestException : public std::exception
{
    public:
        virtual const char* what() const throw() { return testExceptionWhatString; }
};

TEST(LoggerTest, CtorDtorLogLevel)
{
    std::unique_ptr<Logger> logger = nullptr;

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger()));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Default);
    EXPECT_EQ(logger->getOutputStream(), Logger::getDefaultOutputStream());
    EXPECT_TRUE(logger->getAggregator() == nullptr);
    EXPECT_NO_THROW(logger = nullptr);

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(LogLevel::Fatal)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Fatal);
    EXPECT_EQ(logger->getOutputStream(), Logger::getDefaultOutputStream());
    EXPECT_TRUE(logger->getAggregator() == nullptr);
    EXPECT_NO_THROW(logger = nullptr);
}

TEST(LoggerTest, CtorDtorOsRefLogLevel)
{
    std::unique_ptr<Logger> logger = nullptr;
    std::stringstream stringStream;

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(stringStream)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Default);
    EXPECT_EQ(logger->getOutputStream().get(), &stringStream);
    EXPECT_TRUE(logger->getAggregator() == nullptr);
    EXPECT_NO_THROW(logger = nullptr);

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(stringStream, LogLevel::Warn)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Warn);
    EXPECT_EQ(logger->getOutputStream().get(), &stringStream);
    EXPECT_TRUE(logger->getAggregator() == nullptr);
    EXPECT_NO_THROW(logger = nullptr);
}

TEST(LoggerTest, CtorDtorOsRefAggRefLogLevel)
{
    std::unique_ptr<Logger> logger = nullptr;
    std::stringstream stringStream;
    Logger aggregateLogger;

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(stringStream, aggregateLogger)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Default);
    EXPECT_EQ(logger->getOutputStream().get(), &stringStream);
    EXPECT_EQ(logger->getAggregator().get(), &aggregateLogger);
    EXPECT_NO_THROW(logger = nullptr);

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(stringStream, aggregateLogger, LogLevel::Debug)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Debug);
    EXPECT_EQ(logger->getOutputStream().get(), &stringStream);
    EXPECT_EQ(logger->getAggregator().get(), &aggregateLogger);
    EXPECT_NO_THROW(logger = nullptr);
}

TEST(LoggerTest, CtorDtorOsRefAggPtrLogLevel)
{
    std::unique_ptr<Logger> logger = nullptr;
    std::stringstream stringStream;
    std::shared_ptr<Logger> aggregateLogger = std::shared_ptr<Logger>(new Logger());

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(stringStream, aggregateLogger)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Default);
    EXPECT_EQ(logger->getOutputStream().get(), &stringStream);
    EXPECT_EQ(logger->getAggregator(), aggregateLogger);
    EXPECT_NO_THROW(logger = nullptr);

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(stringStream, aggregateLogger, LogLevel::Off)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Off);
    EXPECT_EQ(logger->getOutputStream().get(), &stringStream);
    EXPECT_EQ(logger->getAggregator(), aggregateLogger);
    EXPECT_NO_THROW(logger = nullptr);
}

TEST(LoggerTest, CtorDtorOsPtrLogLevel)
{
    std::unique_ptr<Logger> logger = nullptr;
    std::shared_ptr<std::stringstream> stringStream = std::shared_ptr<std::stringstream>(new std::stringstream());

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(stringStream)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Default);
    EXPECT_EQ(logger->getOutputStream(), stringStream);
    EXPECT_TRUE(logger->getAggregator() == nullptr);
    EXPECT_NO_THROW(logger = nullptr);

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(stringStream, LogLevel::All)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::All);
    EXPECT_EQ(logger->getOutputStream(), stringStream);
    EXPECT_TRUE(logger->getAggregator() == nullptr);
    EXPECT_NO_THROW(logger = nullptr);
}

TEST(LoggerTest, CtorDtorOsPtrAggRefLogLevel)
{
    std::unique_ptr<Logger> logger = nullptr;
    std::shared_ptr<std::stringstream> stringStream = std::shared_ptr<std::stringstream>(new std::stringstream());
    Logger aggregateLogger;

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(stringStream, aggregateLogger)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Default);
    EXPECT_EQ(logger->getOutputStream(), stringStream);
    EXPECT_EQ(logger->getAggregator().get(), &aggregateLogger);
    EXPECT_NO_THROW(logger = nullptr);

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(stringStream, aggregateLogger, LogLevel::Fatal)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Fatal);
    EXPECT_EQ(logger->getOutputStream(), stringStream);
    EXPECT_EQ(logger->getAggregator().get(), &aggregateLogger);
    EXPECT_NO_THROW(logger = nullptr);
}

TEST(LoggerTest, CtorDtorOsPtrAggPtrLogLevel)
{
    std::unique_ptr<Logger> logger = nullptr;
    std::shared_ptr<std::stringstream> stringStream = std::shared_ptr<std::stringstream>(new std::stringstream());
    std::shared_ptr<Logger> aggregateLogger = std::shared_ptr<Logger>(new Logger());

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(stringStream, aggregateLogger)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Default);
    EXPECT_EQ(logger->getOutputStream(), stringStream);
    EXPECT_EQ(logger->getAggregator(), aggregateLogger);
    EXPECT_NO_THROW(logger = nullptr);

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(stringStream, aggregateLogger, LogLevel::Warn)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Warn);
    EXPECT_EQ(logger->getOutputStream(), stringStream);
    EXPECT_EQ(logger->getAggregator(), aggregateLogger);
    EXPECT_NO_THROW(logger = nullptr);
}

TEST(LoggerTest, CtorDtorAggRefLogLevel)
{
    std::unique_ptr<Logger> logger = nullptr;
    Logger aggregateLogger;

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(aggregateLogger)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Default);
    EXPECT_EQ(logger->getOutputStream(), Logger::getDefaultOutputStream());
    EXPECT_EQ(logger->getAggregator().get(), &aggregateLogger);
    EXPECT_NO_THROW(logger = nullptr);

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(aggregateLogger, LogLevel::Debug)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Debug);
    EXPECT_EQ(logger->getOutputStream(), Logger::getDefaultOutputStream());
    EXPECT_EQ(logger->getAggregator().get(), &aggregateLogger);
    EXPECT_NO_THROW(logger = nullptr);
}

TEST(LoggerTest, CtorDtorAggPtrLogLevel)
{
    std::unique_ptr<Logger> logger = nullptr;
    std::shared_ptr<Logger> aggregateLogger = std::shared_ptr<Logger>(new Logger());

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(aggregateLogger)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Default);
    EXPECT_EQ(logger->getOutputStream(), Logger::getDefaultOutputStream());
    EXPECT_EQ(logger->getAggregator(), aggregateLogger);
    EXPECT_NO_THROW(logger = nullptr);

    EXPECT_NO_THROW(logger = std::unique_ptr<Logger>(new Logger(aggregateLogger, LogLevel::Off)));
    EXPECT_EQ(logger->getLogLevel(), LogLevel::Off);
    EXPECT_EQ(logger->getOutputStream(), Logger::getDefaultOutputStream());
    EXPECT_EQ(logger->getAggregator(), aggregateLogger);
    EXPECT_NO_THROW(logger = nullptr);
}

TEST(LoggerTest, GettersSetters)
{
    constexpr LogLevel testLogLevel = LogLevel::Fatal;
    constexpr LogLevel testLogLevelInt = (LogLevel)33;
    std::stringstream stringStream;
    std::shared_ptr<std::stringstream> stringStreamPtr = std::shared_ptr<std::stringstream>(new std::stringstream());
    Logger aggregateLogger;
    ASSERT_NE(testLogLevel, LogLevel::Default);
    ASSERT_NE(testLogLevelInt, LogLevel::Default);

    Logger logger;

    EXPECT_EQ(logger.getLogLevel(), LogLevel::Default);
    logger.setLogLevel(testLogLevel);
    EXPECT_EQ(logger.getLogLevel(), testLogLevel);

    logger.setLogLevel(testLogLevelInt);
    EXPECT_EQ(logger.getLogLevel(), testLogLevelInt);

    EXPECT_EQ(logger.getOutputStream(), Logger::getDefaultOutputStream());
    logger.setOutputStream(stringStream);
    EXPECT_EQ(logger.getOutputStream().get(), &stringStream);

    logger.setOutputStream(stringStreamPtr);
    EXPECT_EQ(logger.getOutputStream(), stringStreamPtr);

    EXPECT_TRUE(logger.getAggregator() == nullptr);
    logger.setAggregator(aggregateLogger);
    EXPECT_EQ(logger.getAggregator().get(), &aggregateLogger);
}

TEST(LoggerTest, GetTimestamp)
{
    Logger logger;
    std::string timestamp = logger.getTimestamp();
    EXPECT_FALSE(timestamp.empty());
    EXPECT_TRUE(std::regex_match(timestamp, std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z")));
}

TEST(LoggerTest, GetPrefix)
{
    Logger logger;
    EXPECT_TRUE(std::regex_match(logger.getPrefix(LogLevel::Fatal), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z FATAL: ")));
    EXPECT_TRUE(std::regex_match(logger.getPrefix(LogLevel::Error), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z ERROR: ")));
    EXPECT_TRUE(std::regex_match(logger.getPrefix(LogLevel::Warn), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z WARN: ")));
    EXPECT_TRUE(std::regex_match(logger.getPrefix(LogLevel::Info), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z INFO: ")));
    EXPECT_TRUE(std::regex_match(logger.getPrefix(LogLevel::Debug), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z DEBUG: ")));
    EXPECT_TRUE(std::regex_match(logger.getPrefix(LogLevel::Trace), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z TRACE: ")));
    EXPECT_TRUE(std::regex_match(logger.getPrefix((LogLevel)33), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z LEVEL\\[33\\]: ")));
}

TEST(LoggerTest, OutputStreamsBasic)
{
    std::stringstream stringStream;
    Logger logger(stringStream, LogLevel::All);

    logger << "1234";
    EXPECT_EQ("1234", stringStream.str());

    logger.log((LogLevel)33) << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z LEVEL\\[33\\]: 1234")));

    logger.fatal() << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z FATAL: 1234")));

    logger.error() << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z ERROR: 1234")));

    logger.warn() << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z WARN: 1234")));

    logger.info() << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z INFO: 1234")));

    logger.debug() << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z DEBUG: 1234")));

    logger.trace() << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z TRACE: 1234")));
}

TEST(LoggerTest, OutputStreamsAdvanced)
{
    std::stringstream stringStream;
    Logger logger(stringStream, LogLevel::All);

    logger << "01234";
    logger << 56 << '7' << "89" << std::hex << std::uppercase << 0xabcdef;

    EXPECT_EQ("0123456789ABCDEF", stringStream.str());
}

TEST(LoggerTest, OutputStreamLogLevelsExclusive)
{
    std::stringstream stringStream;

    Logger logger(stringStream, LogLevel::Off);
    logger << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.log((LogLevel)33) << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.fatal() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.error() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.warn() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.info() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel((LogLevel)32);
    logger.log((LogLevel)33) << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.fatal() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.error() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.warn() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.info() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel((LogLevel)33);
    logger.fatal() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.error() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.warn() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.info() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Fatal);
    logger.error() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.warn() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.info() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Error);
    logger.warn() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.info() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Warn);
    logger.info() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Info);
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Debug);
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Trace);
    logger.log(LogLevel(LogLevel::Trace+1)) << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
}

TEST(LoggerTest, OutputStreamLogLevelsInclusive)
{
    std::stringstream stringStream;

    Logger logger(stringStream, LogLevel::All);
    logger.log((LogLevel)UINT32_MAX) << "1MAX";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1MAX")));
    logger.log(LogLevel(LogLevel::Trace+1)) << "1TRA+";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1TRA+")));
    logger.trace() << "1TRACE";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1TRACE")));
    logger.debug() << "1DEBUG";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1DEBUG")));
    logger.info() << "1INFO";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1INFO")));
    logger.warn() << "1WARN";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1WARN")));
    logger.error() << "1ERROR";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1ERROR")));
    logger.fatal() << "1FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1FATAL")));

    logger.setLogLevel(LogLevel(LogLevel::Trace+1));
    logger.log(LogLevel(LogLevel::Trace+1)) << "2TRA+";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2TRA+")));
    logger.trace() << "2TRACE";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2TRACE")));
    logger.debug() << "2DEBUG";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2DEBUG")));
    logger.info() << "2INFO";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2INFO")));
    logger.warn() << "2WARN";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2WARN")));
    logger.error() << "2ERROR";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2ERROR")));
    logger.fatal() << "2FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2FATAL")));

    logger.setLogLevel(LogLevel::Trace);
    logger.trace() << "3TRACE";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3TRACE")));
    logger.debug() << "3DEBUG";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3DEBUG")));
    logger.info() << "3INFO";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3INFO")));
    logger.warn() << "3WARN";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3WARN")));
    logger.error() << "3ERROR";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3ERROR")));
    logger.fatal() << "3FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3FATAL")));

    logger.setLogLevel(LogLevel::Debug);
    logger.debug() << "4DEBUG";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4DEBUG")));
    logger.info() << "4INFO";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4INFO")));
    logger.warn() << "4WARN";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4WARN")));
    logger.error() << "4ERROR";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4ERROR")));
    logger.fatal() << "4FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4FATAL")));

    logger.setLogLevel(LogLevel::Info);
    logger.info() << "5INFO";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5INFO")));
    logger.warn() << "5WARN";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5WARN")));
    logger.error() << "5ERROR";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5ERROR")));
    logger.fatal() << "5FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5FATAL")));

    logger.setLogLevel(LogLevel::Warn);
    logger.warn() << "6WARN";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6WARN")));
    logger.error() << "6ERROR";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6ERROR")));
    logger.fatal() << "6FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6FATAL")));

    logger.setLogLevel(LogLevel::Error);
    logger.error() << "7ERROR";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("7ERROR")));
    logger.fatal() << "7FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("7FATAL")));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal() << "8FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("8FATAL")));
}

TEST(LoggerTest, OutputMethodsBasic)
{
    std::stringstream stringStream;
    Logger logger(stringStream, LogLevel::All);
    logger.log((LogLevel)33, "1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z LEVEL\\[33\\]: 1234")));

    logger.fatal("1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z FATAL: 1234")));

    logger.error("1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z ERROR: 1234")));

    logger.warn("1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z WARN: 1234")));

    logger.info("1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z INFO: 1234")));

    logger.debug("1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z DEBUG: 1234")));

    logger.trace("1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z TRACE: 1234")));
}

TEST(LoggerTest, OutputMethodsExceptions)
{
    TestException testException;
    std::stringstream stringStream;
    Logger logger(stringStream, LogLevel::All);
    logger.log((LogLevel)33, "1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z LEVEL\\[33\\]: 1234\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.fatal("1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z FATAL: 1234\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.error("1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z ERROR: 1234\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.warn("1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z WARN: 1234\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.info("1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z INFO: 1234\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.debug("1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z DEBUG: 1234\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.trace("1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z TRACE: 1234\\r?\\n?" + std::string(testExceptionWhatString))));
}

TEST(LoggerTest, OutputMethodLogLevelsExclusive)
{
    std::stringstream stringStream;

    Logger logger(stringStream, LogLevel::Off);
    logger.log((LogLevel)33, "asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.fatal("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.error("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.warn("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.info("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel((LogLevel)32);
    logger.log((LogLevel)33, "asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.fatal("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.error("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.warn("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.info("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel((LogLevel)33);
    logger.fatal("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.error("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.warn("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.info("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Fatal);
    logger.error("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.warn("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.info("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Error);
    logger.warn("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.info("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Warn);
    logger.info("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Info);
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Debug);
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Trace);
    logger.log(LogLevel(LogLevel::Trace+1), "asdf");
    EXPECT_TRUE(stringStream.str().empty());
}

TEST(LoggerTest, OutputMethodLogLevelsInclusive)
{
    std::stringstream stringStream;

    Logger logger(stringStream, LogLevel::All);
    logger.log((LogLevel)UINT32_MAX, "1MAX");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1MAX")));
    logger.log(LogLevel(LogLevel::Trace+1), "1TRA+");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1TRA+")));
    logger.trace("1TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1TRACE")));
    logger.debug("1DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1DEBUG")));
    logger.info("1INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1INFO")));
    logger.warn("1WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1WARN")));
    logger.error("1ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1ERROR")));
    logger.fatal("1FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1FATAL")));

    logger.setLogLevel(LogLevel(LogLevel::Trace+1));
    logger.log(LogLevel(LogLevel::Trace+1), "2TRA+");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2TRA+")));
    logger.trace("2TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2TRACE")));
    logger.debug("2DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2DEBUG")));
    logger.info("2INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2INFO")));
    logger.warn("2WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2WARN")));
    logger.error("2ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2ERROR")));
    logger.fatal("2FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2FATAL")));

    logger.setLogLevel(LogLevel::Trace);
    logger.trace("3TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3TRACE")));
    logger.debug("3DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3DEBUG")));
    logger.info("3INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3INFO")));
    logger.warn("3WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3WARN")));
    logger.error("3ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3ERROR")));
    logger.fatal("3FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3FATAL")));

    logger.setLogLevel(LogLevel::Debug);
    logger.debug("4DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4DEBUG")));
    logger.info("4INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4INFO")));
    logger.warn("4WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4WARN")));
    logger.error("4ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4ERROR")));
    logger.fatal("4FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4FATAL")));

    logger.setLogLevel(LogLevel::Info);
    logger.info("5INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5INFO")));
    logger.warn("5WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5WARN")));
    logger.error("5ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5ERROR")));
    logger.fatal("5FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5FATAL")));

    logger.setLogLevel(LogLevel::Warn);
    logger.warn("6WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6WARN")));
    logger.error("6ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6ERROR")));
    logger.fatal("6FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6FATAL")));

    logger.setLogLevel(LogLevel::Error);
    logger.error("7ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("7ERROR")));
    logger.fatal("7FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("7FATAL")));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal("8FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("8FATAL")));
}

TEST(LoggerTest, OutputMethodExceptionsLogLevelsExclusive)
{
    TestException testException;
    std::stringstream stringStream;

    Logger logger(stringStream, LogLevel::Off);
    logger.log((LogLevel)33, "asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.fatal("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.error("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.warn("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.info("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel((LogLevel)32);
    logger.log((LogLevel)33, "asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.fatal("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.error("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.warn("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.info("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel((LogLevel)33);
    logger.fatal("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.error("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.warn("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.info("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Fatal);
    logger.error("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.warn("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.info("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Error);
    logger.warn("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.info("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Warn);
    logger.info("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Info);
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Debug);
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());

    logger.setLogLevel(LogLevel::Trace);
    logger.log(LogLevel(LogLevel::Trace+1), "asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
}

TEST(LoggerTest, OutputMethodExceptionsLogLevelsInclusive)
{
    TestException testException;
    std::stringstream stringStream;

    Logger logger(stringStream, LogLevel::All);
    logger.log((LogLevel)UINT32_MAX, "1MAX", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1MAX\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.log(LogLevel(LogLevel::Trace+1), "1TRA+", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1TRA\\+\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.trace("1TRACE", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1TRACE\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.debug("1DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1DEBUG\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.info("1INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.warn("1WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.error("1ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("1FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel(LogLevel::Trace+1));
    logger.log(LogLevel(LogLevel::Trace+1), "2TRA+", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2TRA\\+\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.trace("2TRACE", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2TRACE\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.debug("2DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2DEBUG\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.info("2INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.warn("2WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.error("2ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("2FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Trace);
    logger.trace("3TRACE", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3TRACE\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.debug("3DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3DEBUG\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.info("3INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.warn("3WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.error("3ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("3FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Debug);
    logger.debug("4DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4DEBUG\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.info("4INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.warn("4WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.error("4ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("4FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Info);
    logger.info("5INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.warn("5WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.error("5ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("5FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Warn);
    logger.warn("6WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.error("6ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("6FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Error);
    logger.error("7ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("7ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("7FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("7FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal("8FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("8FATAL\\r?\\n?" + std::string(testExceptionWhatString))));
}

TEST(LoggerTest, OutputStreamsAggregationBasic)
{
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::All);
    Logger logger(stringStream, aggregateLogger, LogLevel::All);

    logger << "1234";
    EXPECT_EQ("1234", stringStream.str());
    EXPECT_EQ("1234", aggregateStringStream.str());

    logger.log((LogLevel)33) << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z LEVEL\\[33\\]: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z LEVEL\\[33\\]: 1234")));

    logger.fatal() << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z FATAL: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z FATAL: 1234")));

    logger.error() << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z ERROR: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z ERROR: 1234")));

    logger.warn() << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z WARN: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z WARN: 1234")));

    logger.info() << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z INFO: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z INFO: 1234")));

    logger.debug() << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z DEBUG: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z DEBUG: 1234")));

    logger.trace() << "1234";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z TRACE: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z TRACE: 1234")));
}

TEST(LoggerTest, OutputStreamsAggregationAdvanced)
{
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::All);
    Logger logger(stringStream, aggregateLogger, LogLevel::All);

    logger << "01234";
    logger << 56 << '7' << "89" << std::hex << std::uppercase << 0xabcdef;

    EXPECT_EQ("0123456789ABCDEF", stringStream.str());
    EXPECT_EQ("0123456789ABCDEF", aggregateStringStream.str());
}

TEST(LoggerTest, OutputStreamAggregationLogLevelsEqExclusive)
{
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::Off);
    Logger logger(stringStream, aggregateLogger, LogLevel::Off);

    logger << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.log((LogLevel)33) << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.fatal() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.error() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.warn() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel((LogLevel)32);
    aggregateLogger.setLogLevel((LogLevel)32);
    logger.log((LogLevel)33) << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.fatal() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.error() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.warn() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel((LogLevel)33);
    aggregateLogger.setLogLevel((LogLevel)33);
    logger.fatal() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.error() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.warn() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Fatal);
    aggregateLogger.setLogLevel(LogLevel::Fatal);
    logger.error() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.warn() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Error);
    aggregateLogger.setLogLevel(LogLevel::Error);
    logger.warn() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Warn);
    aggregateLogger.setLogLevel(LogLevel::Warn);
    logger.info() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Info);
    aggregateLogger.setLogLevel(LogLevel::Info);
    logger.debug() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Debug);
    aggregateLogger.setLogLevel(LogLevel::Debug);
    logger.trace() << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Trace);
    aggregateLogger.setLogLevel(LogLevel::Trace);
    logger.log(LogLevel(LogLevel::Trace+1)) << "asdf";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
}

TEST(LoggerTest, OutputStreamAggregationLogLevelsEqInclusive)
{
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::All);
    Logger logger(stringStream, aggregateLogger, LogLevel::All);

    logger.log((LogLevel)UINT32_MAX, "1MAX");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1MAX")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1MAX")));
    logger.log(LogLevel(LogLevel::Trace+1), "1TRA+");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1TRA+")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1TRA+")));
    logger.trace("1TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1TRACE")));
    logger.debug("1DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1DEBUG")));
    logger.info("1INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1INFO")));
    logger.warn("1WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1WARN")));
    logger.error("1ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1ERROR")));
    logger.fatal("1FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1FATAL")));

    logger.setLogLevel(LogLevel(LogLevel::Trace+1));
    aggregateLogger.setLogLevel(LogLevel(LogLevel::Trace+1));
    logger.log(LogLevel(LogLevel::Trace+1), "2TRA+");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2TRA+")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2TRA+")));
    logger.trace("2TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2TRACE")));
    logger.debug("2DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2DEBUG")));
    logger.info("2INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2INFO")));
    logger.warn("2WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2WARN")));
    logger.error("2ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2ERROR")));
    logger.fatal("2FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2FATAL")));

    logger.setLogLevel(LogLevel::Trace);
    aggregateLogger.setLogLevel(LogLevel::Trace);
    logger.trace("3TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3TRACE")));
    logger.debug("3DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3DEBUG")));
    logger.info("3INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3INFO")));
    logger.warn("3WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3WARN")));
    logger.error("3ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3ERROR")));
    logger.fatal("3FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3FATAL")));

    logger.setLogLevel(LogLevel::Debug);
    aggregateLogger.setLogLevel(LogLevel::Debug);
    logger.debug("4DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4DEBUG")));
    logger.info("4INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4INFO")));
    logger.warn("4WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4WARN")));
    logger.error("4ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4ERROR")));
    logger.fatal("4FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4FATAL")));

    logger.setLogLevel(LogLevel::Info);
    aggregateLogger.setLogLevel(LogLevel::Info);
    logger.info("5INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("5INFO")));
    logger.warn("5WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("5WARN")));
    logger.error("5ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("5ERROR")));
    logger.fatal("5FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("5FATAL")));

    logger.setLogLevel(LogLevel::Warn);
    aggregateLogger.setLogLevel(LogLevel::Warn);
    logger.warn("6WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("6WARN")));
    logger.error("6ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("6ERROR")));
    logger.fatal("6FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("6FATAL")));

    logger.setLogLevel(LogLevel::Error);
    aggregateLogger.setLogLevel(LogLevel::Error);
    logger.error("7ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("7ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("7ERROR")));
    logger.fatal("7FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("7FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("7FATAL")));

    logger.setLogLevel(LogLevel::Fatal);
    aggregateLogger.setLogLevel(LogLevel::Fatal);
    logger.fatal("8FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("8FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("8FATAL")));
}

TEST(LoggerTest, OutputStreamAggregationLogLevelsNotEq)
{
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::All);
    Logger logger(stringStream, aggregateLogger, LogLevel::Off);

    logger << "OFF_ALL";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str() == "OFF_ALL");
    logger.fatal() << "OFF_ALL_FATAL";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_ALL_FATAL")));
    logger.trace() << "OFF_ALL_TRACE";
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_ALL_TRACE")));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal() << "FATAL_ALL_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_ALL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_ALL_FATAL")));
    logger.debug() << "FATAL_ALL_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_ALL_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_ALL_DEBUG")));
    logger.trace() << "FATAL_ALL_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_ALL_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_ALL_TRACE")));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal() << "INFO_ALL_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_FATAL")));
    logger.warn() << "INFO_ALL_WARN";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_WARN")));
    logger.info() << "INFO_ALL_INFO";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_INFO")));
    logger.debug() << "INFO_ALL_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_DEBUG")));
    logger.trace() << "INFO_ALL_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_TRACE")));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal() << "TRACE_ALL_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_ALL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_ALL_FATAL")));
    logger.debug() << "TRACE_ALL_DEBUG";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_ALL_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_ALL_DEBUG")));
    logger.trace() << "TRACE_ALL_TRACE";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_ALL_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_ALL_TRACE")));

    aggregateLogger.setLogLevel(LogLevel::Trace);
    logger.setLogLevel(LogLevel::Off);

    logger << "OFF_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_TRACE")));
    logger.fatal() << "OFF_TRACE_FATAL";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_TRACE_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_TRACE_FATAL")));
    logger.debug() << "OFF_TRACE_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_TRACE_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_TRACE_DEBUG")));
    logger.trace() << "OFF_TRACE_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_TRACE_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_TRACE_TRACE")));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal() << "FATAL_TRACE_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_TRACE_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_TRACE_FATAL")));
    logger.debug() << "FATAL_TRACE_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_TRACE_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_TRACE_DEBUG")));
    logger.trace() << "FATAL_TRACE_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_TRACE_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_TRACE_TRACE")));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal() << "INFO_TRACE_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_FATAL")));
    logger.warn() << "INFO_TRACE_WARN";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_WARN")));
    logger.info() << "INFO_TRACE_INFO";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_INFO")));
    logger.debug() << "INFO_TRACE_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_DEBUG")));
    logger.trace() << "INFO_TRACE_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_TRACE")));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal() << "TRACE_TRACE_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_TRACE_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_TRACE_FATAL")));
    logger.debug() << "TRACE_TRACE_DEBUG";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_TRACE_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_TRACE_DEBUG")));
    logger.trace() << "TRACE_TRACE_TRACE";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_TRACE_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_TRACE_TRACE")));

    aggregateLogger.setLogLevel(LogLevel::Info);
    logger.setLogLevel(LogLevel::Off);

    logger << "OFF_INFO";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_INFO")));
    logger.fatal() << "OFF_INFO_FATAL";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_INFO_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_INFO_FATAL")));
    logger.debug() << "OFF_INFO_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_INFO_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_INFO_DEBUG")));
    logger.trace() << "OFF_INFO_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_INFO_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_INFO_TRACE")));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal() << "FATAL_INFO_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_INFO_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_INFO_FATAL")));
    logger.debug() << "FATAL_INFO_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_INFO_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_INFO_DEBUG")));
    logger.trace() << "FATAL_INFO_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_INFO_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_INFO_TRACE")));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal() << "INFO_INFO_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_FATAL")));
    logger.warn() << "INFO_INFO_WARN";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_WARN")));
    logger.info() << "INFO_INFO_INFO";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_INFO")));
    logger.debug() << "INFO_INFO_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_DEBUG")));
    logger.trace() << "INFO_INFO_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_TRACE")));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal() << "TRACE_INFO_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_INFO_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_INFO_FATAL")));
    logger.debug() << "TRACE_INFO_DEBUG";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_INFO_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_INFO_DEBUG")));
    logger.trace() << "TRACE_INFO_TRACE";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_INFO_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_INFO_TRACE")));

    aggregateLogger.setLogLevel(LogLevel::Fatal);
    logger.setLogLevel(LogLevel::Off);

    logger << "OFF_FATAL";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_FATAL")));
    logger.fatal() << "OFF_FATAL_FATAL";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_FATAL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_FATAL_FATAL")));
    logger.debug() << "OFF_FATAL_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_FATAL_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_FATAL_DEBUG")));
    logger.trace() << "OFF_FATAL_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_FATAL_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_FATAL_TRACE")));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal() << "FATAL_FATAL_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_FATAL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_FATAL_FATAL")));
    logger.debug() << "FATAL_FATAL_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_FATAL_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_FATAL_DEBUG")));
    logger.trace() << "FATAL_FATAL_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_FATAL_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_FATAL_TRACE")));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal() << "INFO_FATAL_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_FATAL")));
    logger.warn() << "INFO_FATAL_WARN";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_WARN")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_WARN")));
    logger.info() << "INFO_FATAL_INFO";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_INFO")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_INFO")));
    logger.debug() << "INFO_FATAL_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_DEBUG")));
    logger.trace() << "INFO_FATAL_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_TRACE")));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal() << "TRACE_FATAL_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_FATAL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_FATAL_FATAL")));
    logger.debug() << "TRACE_FATAL_DEBUG";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_FATAL_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_FATAL_DEBUG")));
    logger.trace() << "TRACE_FATAL_TRACE";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_FATAL_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_FATAL_TRACE")));

    aggregateLogger.setLogLevel(LogLevel::Off);
    logger.setLogLevel(LogLevel::Off);

    logger << "OFF_OFF";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_OFF")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_OFF")));
    logger.fatal() << "OFF_OFF_FATAL";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_OFF_FATAL")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_OFF_FATAL")));
    logger.debug() << "OFF_OFF_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_OFF_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_OFF_DEBUG")));
    logger.trace() << "OFF_OFF_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_OFF_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_OFF_TRACE")));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal() << "FATAL_OFF_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_OFF_FATAL")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_OFF_FATAL")));
    logger.debug() << "FATAL_OFF_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_OFF_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_OFF_DEBUG")));
    logger.trace() << "FATAL_OFF_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_OFF_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_OFF_TRACE")));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal() << "INFO_OFF_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_FATAL")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_FATAL")));
    logger.warn() << "INFO_OFF_WARN";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_WARN")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_WARN")));
    logger.info() << "INFO_OFF_INFO";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_INFO")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_INFO")));
    logger.debug() << "INFO_OFF_DEBUG";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_DEBUG")));
    logger.trace() << "INFO_OFF_TRACE";
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_TRACE")));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal() << "TRACE_OFF_FATAL";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_OFF_FATAL")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_OFF_FATAL")));
    logger.debug() << "TRACE_OFF_DEBUG";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_OFF_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_OFF_DEBUG")));
    logger.trace() << "TRACE_OFF_TRACE";
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_OFF_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_OFF_TRACE")));
}

TEST(LoggerTest, OutputMethodsAggregationBasic)
{
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::All);
    Logger logger(stringStream, aggregateLogger, LogLevel::All);

    logger.log((LogLevel)33, "1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z LEVEL\\[33\\]: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z LEVEL\\[33\\]: 1234")));

    logger.fatal("1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z FATAL: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z FATAL: 1234")));

    logger.error("1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z ERROR: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z ERROR: 1234")));

    logger.warn("1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z WARN: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z WARN: 1234")));

    logger.info("1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z INFO: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z INFO: 1234")));

    logger.debug("1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z DEBUG: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z DEBUG: 1234")));

    logger.trace("1234");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z TRACE: 1234")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z TRACE: 1234")));
}

TEST(LoggerTest, OutputMethodsExceptionsAggregation)
{
    TestException testException;
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::All);
    Logger logger(stringStream, aggregateLogger, LogLevel::All);

    logger.log((LogLevel)33, "1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z LEVEL\\[33\\]: 1234\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z LEVEL\\[33\\]: 1234\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.fatal("1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z FATAL: 1234\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z FATAL: 1234\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.error("1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z ERROR: 1234\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z ERROR: 1234\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.warn("1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z WARN: 1234\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z WARN: 1234\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.info("1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z INFO: 1234\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z INFO: 1234\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.debug("1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z DEBUG: 1234\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z DEBUG: 1234\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.trace("1234", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z TRACE: 1234\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}-[0-9]{2}-[0-9]{2}Z TRACE: 1234\\r?\\n?" + std::string(testExceptionWhatString))));
}

TEST(LoggerTest, OutputMethodAggregationLogLevelsEqExclusive)
{
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::Off);
    Logger logger(stringStream, aggregateLogger, LogLevel::Off);

    logger.log((LogLevel)33, "asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.fatal("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.error("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.warn("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel((LogLevel)32);
    aggregateLogger.setLogLevel((LogLevel)32);
    logger.log((LogLevel)33, "asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.fatal("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.error("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.warn("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel((LogLevel)33);
    aggregateLogger.setLogLevel((LogLevel)33);
    logger.fatal("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.error("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.warn("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Fatal);
    aggregateLogger.setLogLevel(LogLevel::Fatal);
    logger.error("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.warn("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Error);
    aggregateLogger.setLogLevel(LogLevel::Error);
    logger.warn("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Warn);
    aggregateLogger.setLogLevel(LogLevel::Warn);
    logger.info("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Info);
    aggregateLogger.setLogLevel(LogLevel::Info);
    logger.debug("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Debug);
    aggregateLogger.setLogLevel(LogLevel::Debug);
    logger.trace("asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Trace);
    aggregateLogger.setLogLevel(LogLevel::Trace);
    logger.log(LogLevel(LogLevel::Trace+1), "asdf");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
}

TEST(LoggerTest, OutputMethodAggregationLogLevelsEqInclusive)
{
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::All);
    Logger logger(stringStream, aggregateLogger, LogLevel::All);

    logger.log((LogLevel)UINT32_MAX, "1MAX");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1MAX")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1MAX")));
    logger.log(LogLevel(LogLevel::Trace+1), "1TRA+");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1TRA+")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1TRA+")));
    logger.trace("1TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1TRACE")));
    logger.debug("1DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1DEBUG")));
    logger.info("1INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1INFO")));
    logger.warn("1WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1WARN")));
    logger.error("1ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1ERROR")));
    logger.fatal("1FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1FATAL")));

    logger.setLogLevel(LogLevel(LogLevel::Trace+1));
    aggregateLogger.setLogLevel(LogLevel(LogLevel::Trace+1));
    logger.log(LogLevel(LogLevel::Trace+1), "2TRA+");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2TRA+")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2TRA+")));
    logger.trace("2TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2TRACE")));
    logger.debug("2DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2DEBUG")));
    logger.info("2INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2INFO")));
    logger.warn("2WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2WARN")));
    logger.error("2ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2ERROR")));
    logger.fatal("2FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2FATAL")));

    logger.setLogLevel(LogLevel::Trace);
    aggregateLogger.setLogLevel(LogLevel::Trace);
    logger.trace("3TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3TRACE")));
    logger.debug("3DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3DEBUG")));
    logger.info("3INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3INFO")));
    logger.warn("3WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3WARN")));
    logger.error("3ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3ERROR")));
    logger.fatal("3FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3FATAL")));

    logger.setLogLevel(LogLevel::Debug);
    aggregateLogger.setLogLevel(LogLevel::Debug);
    logger.debug("4DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4DEBUG")));
    logger.info("4INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4INFO")));
    logger.warn("4WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4WARN")));
    logger.error("4ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4ERROR")));
    logger.fatal("4FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4FATAL")));

    logger.setLogLevel(LogLevel::Info);
    aggregateLogger.setLogLevel(LogLevel::Info);
    logger.info("5INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("5INFO")));
    logger.warn("5WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("5WARN")));
    logger.error("5ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("5ERROR")));
    logger.fatal("5FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("5FATAL")));

    logger.setLogLevel(LogLevel::Warn);
    aggregateLogger.setLogLevel(LogLevel::Warn);
    logger.warn("6WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("6WARN")));
    logger.error("6ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("6ERROR")));
    logger.fatal("6FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("6FATAL")));

    logger.setLogLevel(LogLevel::Error);
    aggregateLogger.setLogLevel(LogLevel::Error);
    logger.error("7ERROR");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("7ERROR")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("7ERROR")));
    logger.fatal("7FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("7FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("7FATAL")));

    logger.setLogLevel(LogLevel::Fatal);
    aggregateLogger.setLogLevel(LogLevel::Fatal);
    logger.fatal("8FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("8FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("8FATAL")));
}

TEST(LoggerTest, OutputMethodAggregationLogLevelsNotEq)
{
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::All);
    Logger logger(stringStream, aggregateLogger, LogLevel::Off);

    logger.fatal("OFF_ALL_FATAL");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_ALL_FATAL")));
    logger.trace("OFF_ALL_TRACE");
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_ALL_TRACE")));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal("FATAL_ALL_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_ALL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_ALL_FATAL")));
    logger.debug("FATAL_ALL_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_ALL_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_ALL_DEBUG")));
    logger.trace("FATAL_ALL_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_ALL_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_ALL_TRACE")));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal("INFO_ALL_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_FATAL")));
    logger.warn("INFO_ALL_WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_WARN")));
    logger.info("INFO_ALL_INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_INFO")));
    logger.debug("INFO_ALL_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_DEBUG")));
    logger.trace("INFO_ALL_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_TRACE")));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal("TRACE_ALL_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_ALL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_ALL_FATAL")));
    logger.debug("TRACE_ALL_DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_ALL_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_ALL_DEBUG")));
    logger.trace("TRACE_ALL_TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_ALL_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_ALL_TRACE")));

    aggregateLogger.setLogLevel(LogLevel::Trace);
    logger.setLogLevel(LogLevel::Off);

    logger.fatal("OFF_TRACE_FATAL");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_TRACE_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_TRACE_FATAL")));
    logger.debug("OFF_TRACE_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_TRACE_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_TRACE_DEBUG")));
    logger.trace("OFF_TRACE_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_TRACE_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_TRACE_TRACE")));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal("FATAL_TRACE_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_TRACE_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_TRACE_FATAL")));
    logger.debug("FATAL_TRACE_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_TRACE_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_TRACE_DEBUG")));
    logger.trace("FATAL_TRACE_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_TRACE_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_TRACE_TRACE")));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal("INFO_TRACE_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_FATAL")));
    logger.warn("INFO_TRACE_WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_WARN")));
    logger.info("INFO_TRACE_INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_INFO")));
    logger.debug("INFO_TRACE_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_DEBUG")));
    logger.trace("INFO_TRACE_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_TRACE")));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal("TRACE_TRACE_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_TRACE_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_TRACE_FATAL")));
    logger.debug("TRACE_TRACE_DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_TRACE_DEBUG")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_TRACE_DEBUG")));
    logger.trace("TRACE_TRACE_TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_TRACE_TRACE")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_TRACE_TRACE")));

    aggregateLogger.setLogLevel(LogLevel::Info);
    logger.setLogLevel(LogLevel::Off);

    logger.fatal("OFF_INFO_FATAL");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_INFO_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_INFO_FATAL")));
    logger.debug("OFF_INFO_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_INFO_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_INFO_DEBUG")));
    logger.trace("OFF_INFO_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_INFO_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_INFO_TRACE")));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal("FATAL_INFO_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_INFO_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_INFO_FATAL")));
    logger.debug("FATAL_INFO_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_INFO_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_INFO_DEBUG")));
    logger.trace("FATAL_INFO_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_INFO_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_INFO_TRACE")));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal("INFO_INFO_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_FATAL")));
    logger.warn("INFO_INFO_WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_WARN")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_WARN")));
    logger.info("INFO_INFO_INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_INFO")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_INFO")));
    logger.debug("INFO_INFO_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_DEBUG")));
    logger.trace("INFO_INFO_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_TRACE")));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal("TRACE_INFO_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_INFO_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_INFO_FATAL")));
    logger.debug("TRACE_INFO_DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_INFO_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_INFO_DEBUG")));
    logger.trace("TRACE_INFO_TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_INFO_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_INFO_TRACE")));

    aggregateLogger.setLogLevel(LogLevel::Fatal);
    logger.setLogLevel(LogLevel::Off);

    logger.fatal("OFF_FATAL_FATAL");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_FATAL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_FATAL_FATAL")));
    logger.debug("OFF_FATAL_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_FATAL_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_FATAL_DEBUG")));
    logger.trace("OFF_FATAL_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_FATAL_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_FATAL_TRACE")));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal("FATAL_FATAL_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_FATAL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_FATAL_FATAL")));
    logger.debug("FATAL_FATAL_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_FATAL_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_FATAL_DEBUG")));
    logger.trace("FATAL_FATAL_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_FATAL_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_FATAL_TRACE")));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal("INFO_FATAL_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_FATAL")));
    logger.warn("INFO_FATAL_WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_WARN")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_WARN")));
    logger.info("INFO_FATAL_INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_INFO")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_INFO")));
    logger.debug("INFO_FATAL_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_DEBUG")));
    logger.trace("INFO_FATAL_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_TRACE")));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal("TRACE_FATAL_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_FATAL_FATAL")));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_FATAL_FATAL")));
    logger.debug("TRACE_FATAL_DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_FATAL_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_FATAL_DEBUG")));
    logger.trace("TRACE_FATAL_TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_FATAL_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_FATAL_TRACE")));

    aggregateLogger.setLogLevel(LogLevel::Off);
    logger.setLogLevel(LogLevel::Off);

    logger.fatal("OFF_OFF_FATAL");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_OFF_FATAL")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_OFF_FATAL")));
    logger.debug("OFF_OFF_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_OFF_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_OFF_DEBUG")));
    logger.trace("OFF_OFF_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_OFF_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_OFF_TRACE")));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal("FATAL_OFF_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_OFF_FATAL")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_OFF_FATAL")));
    logger.debug("FATAL_OFF_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_OFF_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_OFF_DEBUG")));
    logger.trace("FATAL_OFF_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_OFF_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_OFF_TRACE")));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal("INFO_OFF_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_FATAL")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_FATAL")));
    logger.warn("INFO_OFF_WARN");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_WARN")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_WARN")));
    logger.info("INFO_OFF_INFO");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_INFO")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_INFO")));
    logger.debug("INFO_OFF_DEBUG");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_DEBUG")));
    logger.trace("INFO_OFF_TRACE");
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_TRACE")));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal("TRACE_OFF_FATAL");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_OFF_FATAL")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_OFF_FATAL")));
    logger.debug("TRACE_OFF_DEBUG");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_OFF_DEBUG")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_OFF_DEBUG")));
    logger.trace("TRACE_OFF_TRACE");
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_OFF_TRACE")));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_OFF_TRACE")));
}

TEST(LoggerTest, OutputMethodExceptionsAggregationLogLevelsEqExclusive)
{
    TestException testException;
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::Off);
    Logger logger(stringStream, aggregateLogger, LogLevel::Off);

    logger.log((LogLevel)33, "asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.fatal("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.error("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.warn("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel((LogLevel)32);
    aggregateLogger.setLogLevel((LogLevel)32);
    logger.log((LogLevel)33, "asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.fatal("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.error("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.warn("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel((LogLevel)33);
    aggregateLogger.setLogLevel((LogLevel)33);
    logger.fatal("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.error("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.warn("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Fatal);
    aggregateLogger.setLogLevel(LogLevel::Fatal);
    logger.error("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.warn("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Error);
    aggregateLogger.setLogLevel(LogLevel::Error);
    logger.warn("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.info("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Warn);
    aggregateLogger.setLogLevel(LogLevel::Warn);
    logger.info("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Info);
    aggregateLogger.setLogLevel(LogLevel::Info);
    logger.debug("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Debug);
    aggregateLogger.setLogLevel(LogLevel::Debug);
    logger.trace("asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());

    logger.setLogLevel(LogLevel::Trace);
    aggregateLogger.setLogLevel(LogLevel::Trace);
    logger.log(LogLevel(LogLevel::Trace+1), "asdf", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(aggregateStringStream.str().empty());
}

TEST(LoggerTest, OutputMethodExceptionsAggregationLogLevelsEqInclusive)
{
    TestException testException;
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::All);
    Logger logger(stringStream, aggregateLogger, LogLevel::All);

    logger.log((LogLevel)UINT32_MAX, "1MAX", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1MAX\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1MAX\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.log(LogLevel(LogLevel::Trace+1), "1TRA+", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1TRA\\+\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1TRA\\+\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.trace("1TRACE", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1TRACE\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1TRACE\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.debug("1DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1DEBUG\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1DEBUG\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.info("1INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.warn("1WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.error("1ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("1FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("1FATAL\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("1FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel(LogLevel::Trace+1));
    aggregateLogger.setLogLevel(LogLevel(LogLevel::Trace+1));
    logger.log(LogLevel(LogLevel::Trace+1), "2TRA+", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2TRA\\+\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2TRA\\+\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.trace("2TRACE", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2TRACE\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2TRACE\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.debug("2DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2DEBUG\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2DEBUG\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.info("2INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.warn("2WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.error("2ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("2FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("2FATAL\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("2FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Trace);
    aggregateLogger.setLogLevel(LogLevel::Trace);
    logger.trace("3TRACE", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3TRACE\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3TRACE\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.debug("3DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3DEBUG\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3DEBUG\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.info("3INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.warn("3WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.error("3ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("3FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("3FATAL\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("3FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Debug);
    aggregateLogger.setLogLevel(LogLevel::Debug);
    logger.debug("4DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4DEBUG\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4DEBUG\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.info("4INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.warn("4WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.error("4ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("4FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("4FATAL\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("4FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Info);
    aggregateLogger.setLogLevel(LogLevel::Info);
    logger.info("5INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("5INFO\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.warn("5WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("5WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.error("5ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("5ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("5FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("5FATAL\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("5FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Warn);
    aggregateLogger.setLogLevel(LogLevel::Warn);
    logger.warn("6WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("6WARN\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.error("6ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("6ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("6FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("6FATAL\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("6FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Error);
    aggregateLogger.setLogLevel(LogLevel::Error);
    logger.error("7ERROR", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("7ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("7ERROR\\r?\\n?" + std::string(testExceptionWhatString))));
    logger.fatal("7FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("7FATAL\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("7FATAL\\r?\\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Fatal);
    aggregateLogger.setLogLevel(LogLevel::Fatal);
    logger.fatal("8FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("8FATAL\\r?\\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("8FATAL\\r?\\n?" + std::string(testExceptionWhatString))));
}

TEST(LoggerTest, OutputMethodExceptionsAggregationLogLevelsNotEq)
{
    TestException testException;
    std::stringstream aggregateStringStream;
    std::stringstream stringStream;
    Logger aggregateLogger(aggregateStringStream, LogLevel::All);
    Logger logger(stringStream, aggregateLogger, LogLevel::Off);

    logger.fatal("OFF_ALL_FATAL", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_ALL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("OFF_ALL_TRACE", testException);
    EXPECT_TRUE(stringStream.str().empty());
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_ALL_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal("FATAL_ALL_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_ALL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_ALL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("FATAL_ALL_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_ALL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_ALL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("FATAL_ALL_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_ALL_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_ALL_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal("INFO_ALL_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.warn("INFO_ALL_WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_WARN\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_WARN\r?\n?" + std::string(testExceptionWhatString))));
    logger.info("INFO_ALL_INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_INFO\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_INFO\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("INFO_ALL_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("INFO_ALL_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_ALL_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_ALL_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal("TRACE_ALL_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_ALL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_ALL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("TRACE_ALL_DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_ALL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_ALL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("TRACE_ALL_TRACE", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_ALL_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_ALL_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    aggregateLogger.setLogLevel(LogLevel::Trace);
    logger.setLogLevel(LogLevel::Off);

    logger.fatal("OFF_TRACE_FATAL", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_TRACE_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_TRACE_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("OFF_TRACE_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_TRACE_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_TRACE_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("OFF_TRACE_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_TRACE_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_TRACE_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal("FATAL_TRACE_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_TRACE_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_TRACE_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("FATAL_TRACE_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_TRACE_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_TRACE_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("FATAL_TRACE_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_TRACE_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_TRACE_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal("INFO_TRACE_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.warn("INFO_TRACE_WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_WARN\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_WARN\r?\n?" + std::string(testExceptionWhatString))));
    logger.info("INFO_TRACE_INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_INFO\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_INFO\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("INFO_TRACE_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("INFO_TRACE_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_TRACE_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_TRACE_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal("TRACE_TRACE_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_TRACE_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_TRACE_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("TRACE_TRACE_DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_TRACE_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_TRACE_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("TRACE_TRACE_TRACE", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_TRACE_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_TRACE_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    aggregateLogger.setLogLevel(LogLevel::Info);
    logger.setLogLevel(LogLevel::Off);

    logger.fatal("OFF_INFO_FATAL", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_INFO_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_INFO_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("OFF_INFO_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_INFO_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_INFO_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("OFF_INFO_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_INFO_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_INFO_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal("FATAL_INFO_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_INFO_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_INFO_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("FATAL_INFO_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_INFO_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_INFO_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("FATAL_INFO_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_INFO_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_INFO_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal("INFO_INFO_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.warn("INFO_INFO_WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_WARN\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_WARN\r?\n?" + std::string(testExceptionWhatString))));
    logger.info("INFO_INFO_INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_INFO\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_INFO\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("INFO_INFO_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("INFO_INFO_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_INFO_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_INFO_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal("TRACE_INFO_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_INFO_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_INFO_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("TRACE_INFO_DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_INFO_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_INFO_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("TRACE_INFO_TRACE", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_INFO_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_INFO_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    aggregateLogger.setLogLevel(LogLevel::Fatal);
    logger.setLogLevel(LogLevel::Off);

    logger.fatal("OFF_FATAL_FATAL", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_FATAL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_FATAL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("OFF_FATAL_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_FATAL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_FATAL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("OFF_FATAL_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_FATAL_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_FATAL_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal("FATAL_FATAL_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_FATAL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_FATAL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("FATAL_FATAL_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_FATAL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_FATAL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("FATAL_FATAL_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_FATAL_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_FATAL_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal("INFO_FATAL_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.warn("INFO_FATAL_WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_WARN\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_WARN\r?\n?" + std::string(testExceptionWhatString))));
    logger.info("INFO_FATAL_INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_INFO\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_INFO\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("INFO_FATAL_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("INFO_FATAL_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_FATAL_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_FATAL_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal("TRACE_FATAL_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_FATAL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_TRUE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_FATAL_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("TRACE_FATAL_DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_FATAL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_FATAL_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("TRACE_FATAL_TRACE", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_FATAL_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_FATAL_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    aggregateLogger.setLogLevel(LogLevel::Off);
    logger.setLogLevel(LogLevel::Off);

    logger.fatal("OFF_OFF_FATAL", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_OFF_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_OFF_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("OFF_OFF_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_OFF_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_OFF_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("OFF_OFF_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("OFF_OFF_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("OFF_OFF_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Fatal);
    logger.fatal("FATAL_OFF_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("FATAL_OFF_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_OFF_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("FATAL_OFF_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_OFF_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_OFF_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("FATAL_OFF_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("FATAL_OFF_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("FATAL_OFF_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Info);
    logger.fatal("INFO_OFF_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.warn("INFO_OFF_WARN", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_WARN\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_WARN\r?\n?" + std::string(testExceptionWhatString))));
    logger.info("INFO_OFF_INFO", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_INFO\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_INFO\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("INFO_OFF_DEBUG", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("INFO_OFF_TRACE", testException);
    EXPECT_FALSE(std::regex_search(stringStream.str(), std::regex("INFO_OFF_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("INFO_OFF_TRACE\r?\n?" + std::string(testExceptionWhatString))));

    logger.setLogLevel(LogLevel::Trace);
    logger.fatal("TRACE_OFF_FATAL", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_OFF_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_OFF_FATAL\r?\n?" + std::string(testExceptionWhatString))));
    logger.debug("TRACE_OFF_DEBUG", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_OFF_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_OFF_DEBUG\r?\n?" + std::string(testExceptionWhatString))));
    logger.trace("TRACE_OFF_TRACE", testException);
    EXPECT_TRUE(std::regex_search(stringStream.str(), std::regex("TRACE_OFF_TRACE\r?\n?" + std::string(testExceptionWhatString))));
    EXPECT_FALSE(std::regex_search(aggregateStringStream.str(), std::regex("TRACE_OFF_TRACE\r?\n?" + std::string(testExceptionWhatString))));
}

TEST(LoggerTest, NullOutputStream)
{
    Logger logger(nullptr, nullptr, LogLevel::All);
    logger.info() << "Nothing ";
    logger.info() << "Should ";
    logger.fatal() << "Write To Null Buffers";
    EXPECT_TRUE(logger.getOutputStream() == nullptr);
}
