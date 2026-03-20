#pragma once

#include <string>

// Supported log levels
enum ELogLevel { LogLevelMinimal, LogLevelExtended, LogLevelFull, LogLevelCount };

constexpr size_t MAX_LOG_ENTRY_CHARS = 512;               // Maximum number of characters in a single log entry
constexpr ELogLevel DEFAULT_LOG_LEVEL = LogLevelFull;     // Default log level

class Logger
{
public:
  static void LogMessage(ELogLevel level, const char* message, ...);
  static void BlankLine(ELogLevel level);
  static void Flush();
  static void SetLogLevel(size_t level);

private:
  Logger() {};        // Private constructor and destructor since the logger is purely static
  ~Logger() {};

  static ELogLevel s_currentLogLevel;
};