#ifndef MINECRAFTBUTACTUALLYGOOD_LOGGER_HPP
#define MINECRAFTBUTACTUALLYGOOD_LOGGER_HPP

//STD
#include <array>
#include <string>
#include <sstream>

#define OSS std::ostringstream().flush()

class Logger {
public:
    enum class LogType {
        INFO,
        WARNING,
        ERROR,
        FATAL,
        DEBUG
    };
private:
    static std::array<std::string, 5> LogTypeStr;

    static void log(const std::string& pText, LogType pType);
    static void log(const std::ostream& pStream, LogType pType);
    static void log(const char* pText, LogType pType);

public:

    // Until I put a thread collector
    Logger() = delete;
    virtual ~Logger() = delete;

    static void Info(const std::string& pText);
    static void Info(const std::ostream& pStream);
    static void Info(const char* pText);

    static void Warn(const std::string& pText);
    static void Warn(const std::ostream& pStream);
    static void Warn(const char* pText);

    static void Error(const std::string& pText);
    static void Error(const std::ostream& pStream);
    static void Error(const char* pText);

    static void Fatal(const std::string& pText);
    static void Fatal(const std::ostream& pStream);
    static void Fatal(const char* pText);

    static void Debug(const std::string& pText);
    static void Debug(const std::ostream& pStream);
    static void Debug(const char* pText);
};

#endif //MINECRAFTBUTACTUALLYGOOD_LOGGER_HPP
