#include "GameCommon/Logger.hpp"

// STD
#include <iostream>
#include <iomanip>
#include <chrono>

std::array<std::string, 5> Logger::LogTypeStr {{
"\033[1;36mINFO\033[0m",
"\033[1;33mWARNING\033[0m",
"\033[1;31mERROR\033[0m",
"\033[1;31mFATAL\033[0m",
"\033[1;35mDEBUG\033[0m"
}};

// TODO Implement thread collector
void Logger::log(const char *pText, Logger::LogType pType) {
    auto now{std::chrono::system_clock::now()};
    auto time{std::chrono::system_clock::to_time_t(now)};
    std::cout << "[" << std::put_time(std::localtime(&time), "%d-%b-%Y %H:%M:%S")
              << " " << std::setw(3) << std::setfill('0') << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000 << "] <"
              << LogTypeStr[static_cast<unsigned long>(pType)] << "> " << pText << "\n";
}

void Logger::log(const std::string &pText, Logger::LogType pType) {
    log(pText.c_str(), pType);
}

void Logger::log(const std::ostream &pStream, Logger::LogType pType) {
    log(dynamic_cast<std::ostringstream&>(const_cast<std::ostream&>(pStream)).str().c_str(), pType);
}

void Logger::Info(const std::string &pText) {
    log(pText,LogType::INFO);
}

void Logger::Info(const std::ostream &pStream) {
    log(pStream,LogType::INFO);
}

void Logger::Info(const char *pText) {
    log(pText,LogType::INFO);
}

void Logger::Warn(const std::string &pText) {
    log(pText,LogType::WARNING);
}

void Logger::Warn(const std::ostream &pStream) {
    log(pStream,LogType::WARNING);
}

void Logger::Warn(const char *pText) {
    log(pText,LogType::WARNING);
}

void Logger::Error(const std::string &pText) {
    log(pText,LogType::ERROR);
}

void Logger::Error(const std::ostream &pStream) {
    log(pStream,LogType::ERROR);
}

void Logger::Error(const char *pText) {
    log(pText,LogType::ERROR);
}

void Logger::Fatal(const std::string &pText) {
    log(pText,LogType::FATAL);
}

void Logger::Fatal(const std::ostream &pStream) {
    log(pStream,LogType::FATAL);
}

void Logger::Fatal(const char *pText) {
    log(pText,LogType::FATAL);
}

void Logger::Debug(const std::string &pText) {
    log(pText,LogType::DEBUG);
}

void Logger::Debug(const std::ostream &pStream) {
    log(pStream,LogType::DEBUG);
}

void Logger::Debug(const char *pText) {
    log(pText,LogType::DEBUG);
}
