#include "Logger.h"
#include <iostream>
#include <sstream>
#include <cstdarg>

// Set a default log level
ELogLevel Logger::s_currentLogLevel = DEFAULT_LOG_LEVEL;

void Logger::LogMessage(ELogLevel level, const char* format, ...)
{
  // Log the message only if the log level is appropriate
  if (level <= s_currentLogLevel)
  {
    va_list args;
    char formattedString[MAX_LOG_ENTRY_CHARS] = {};
    
  // Format the string with all the given arguments
    va_start(args, format);
#ifdef _MSC_VER
    vsprintf_s(formattedString, format, args);
#else
    vsnprintf(formattedString, MAX_LOG_ENTRY_CHARS, format, args);
#endif
    va_end(args);

  // Print the formatted string
  std::cout << formattedString << std::endl;
  }
}

void Logger::BlankLine(ELogLevel level)
{
  // Log the blank line only if the log level is appropriate
  if (level <= s_currentLogLevel)
  {
    std::cout << std::endl;
  }
}

void Logger::Flush()
{
  // Flush the standard output to display messages immediately
  std::cout.flush();
}

void Logger::SetLogLevel(size_t level)
{
  if (level >= LogLevelCount)
  {
    // Invalid log level given
    LogMessage(LogLevelFull, "Error: Trying to set an invalid log level (%d)", level);
  }
  else
  {
    s_currentLogLevel = static_cast<ELogLevel>(level);
  }
}