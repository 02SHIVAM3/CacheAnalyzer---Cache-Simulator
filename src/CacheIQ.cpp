#include "Logger.h"
#include "MemorySystem.h"
#include "CacheTrace.h"
#include "RandomNumberGenerator.h"
#include <fstream>

constexpr size_t ARGUMENT_COUNT = 6;                  // Number of arguments accepted from the CLI
constexpr size_t CACHE_WAYS_MIN = 1;                  // Minimum number of ways in the cache
constexpr size_t CACHE_WAYS_MAX = 1024;               // Maximum number of ways in the cache
constexpr size_t CACHE_SIZE_MIN = 8;                  // Minimum cache size (8KB)
constexpr size_t CACHE_SIZE_MAX = 64;                 // Maximum cache size (64KB)
constexpr size_t OUTPUT_LEVEL_MIN = 0;                // Minimum output detail level
constexpr size_t OUTPUT_LEVEL_MAX = 1;                // Maximum output detail level
constexpr size_t CACHE_LINE_SIZE = 64;                // 64-byte cache lines

// Returns true if the given value is a power-of-two number. Otherwise, returns false
bool IsPowerOfTwo(size_t value)
{
  // A power-of-two number will always have a single bit that is set (Example: 16 = 00010000)
  // Subtracting one from a power-of-two number results in a sequence of set bits (Example: 15 = 00001111)
  // Performing an AND operation on these values will always result in a zero (00010000 & 00001111 = 0)
  // Any value that is not a power-of-two number will result in a different number
  return ((value & (value - 1)) == 0);
}

// Returns true if the given value is within limits. Otherwise, returns false
bool IsWithinLimits(size_t value, size_t min, size_t max)
{
  return ((value >= min) && (value <= max));
}

int main(int argc, char* argv[])
{
  // Check if argc is correct
  if (argc != ARGUMENT_COUNT)
  {
    // Invalid number of input arguments
    Logger::LogMessage(LogLevelFull, "Error: Invalid number of arguments (%d).", argc - 1);
    Logger::LogMessage(LogLevelFull, "The arguments are:");
    Logger::LogMessage(LogLevelFull, "  1. The path (relative/absolute) to the cache trace file.");
    Logger::LogMessage(LogLevelFull, "  2. Cache size (KB).");
    Logger::LogMessage(LogLevelFull, "  3. Cache associativity (number of ways).");
    Logger::LogMessage(LogLevelFull, "  4. Replacement algorithm (RR/LRU).");
    Logger::LogMessage(LogLevelFull, "  5. Output detail level (0/1).");

    return EXIT_FAILURE;
  }

  // Extract data from input argument list
  std::string cacheTracePath = argv[1];
  std::string cacheSizeString = argv[2];
  std::string cacheAssociativityString = argv[3];
  std::string cacheReplacementAlgorithm = argv[4];
  std::string outputLevelString = argv[5];

  size_t cacheSizeKB = 0;
  size_t cacheAssociativity = 0;
  size_t outputLevel = 0;

  try {
    // Try converting input strings to integer values
    cacheSizeKB = std::stoi(cacheSizeString);
    cacheAssociativity = std::stoi(cacheAssociativityString);
    outputLevel = std::stoi(outputLevelString);
  }
  catch (...)
  {
    Logger::LogMessage(LogLevelFull, "Error: one or more arguments are invalid.");
    Logger::LogMessage(LogLevelFull, "Please check that the arguments are:");
    Logger::LogMessage(LogLevelFull, "  1. The path (relative/absolute) to the cache trace file.");
    Logger::LogMessage(LogLevelFull, "  2. Cache size (KB).");
    Logger::LogMessage(LogLevelFull, "  3. Cache associativity (number of ways).");
    Logger::LogMessage(LogLevelFull, "  4. Replacement algorithm (RR/LRU).");
    Logger::LogMessage(LogLevelFull, "  5. Output detail level (0/1).");

    return EXIT_FAILURE;
  }

  // Try opening the file at the given path
  std::ifstream cacheTraceFile(cacheTracePath);
  if (!cacheTraceFile.is_open())
  {
    // Error opening the file
    Logger::LogMessage(LogLevelFull, "Error: Failed to open trace file (%s).", cacheTracePath.c_str());
    Logger::LogMessage(LogLevelFull, "Please check the following:");
    Logger::LogMessage(LogLevelFull, "  1. The file path is correct.");
    Logger::LogMessage(LogLevelFull, "  2. You have the necessary read permissions for the file.");

    return EXIT_FAILURE;
  }

  // Check if the given cache size is allowed
  if ((!IsWithinLimits(cacheSizeKB, CACHE_SIZE_MIN, CACHE_SIZE_MAX)) || (!IsPowerOfTwo(cacheSizeKB)))
  {
    // Illegal cache size: out of bounds
    Logger::LogMessage(LogLevelFull, "Error: Invalid cache size (%dKB).", cacheSizeKB);
    Logger::LogMessage(LogLevelFull, "The cache size must be one of the following: 8, 16, 32, 64.");

    return EXIT_FAILURE;
  }

  // Check if the given cache associativity is allowed
  size_t cacheLineCount = cacheSizeKB * 1024 / CACHE_LINE_SIZE;
  if ((cacheAssociativity > cacheLineCount) ||
      (!IsWithinLimits(cacheAssociativity, CACHE_WAYS_MIN, CACHE_WAYS_MAX)) ||
      (!IsPowerOfTwo(cacheAssociativity)))
  {
    // Illegal number of ways: out of bounds
    Logger::LogMessage(LogLevelFull, "Error: Invalid number of ways (%d).", cacheAssociativity);
    Logger::LogMessage(LogLevelFull, "The cache associativity (number of ways) must:");
    Logger::LogMessage(LogLevelFull, "  1. Be a power-of-two number (1, 2, 4, 8, ...).");
    Logger::LogMessage(LogLevelFull, "  2. Be equal to or below a value of 1024.");
    Logger::LogMessage(LogLevelFull, "  3. Fit within the cache, based on the given cache size and a 64-byte cache line.");

    return EXIT_FAILURE;
  }

  // Search the given replacement algorithm name in the supported algorithms list
  bool replacementAlgoSupported = std::find(s_replacementAlgoNames.begin(),
    s_replacementAlgoNames.end(), cacheReplacementAlgorithm) != s_replacementAlgoNames.end();

  if (!replacementAlgoSupported)
  {
    Logger::LogMessage(LogLevelFull, "Error: Invalid replacement algorithm name (%s).", cacheReplacementAlgorithm.c_str());
    Logger::LogMessage(LogLevelFull, "Algorithm name should be one of the following:");
    Logger::LogMessage(LogLevelFull, "  1. RR: Random Replacement.");
    Logger::LogMessage(LogLevelFull, "  2. LRU: Least Recently Used.");

    return EXIT_FAILURE;
  }

  // Check if the given cache associativity is within limits
  if (!IsWithinLimits(outputLevel, OUTPUT_LEVEL_MIN, OUTPUT_LEVEL_MAX))
  {
    // Illegal output detail level
    Logger::LogMessage(LogLevelFull, "Error: Invalid output detail level (%d).", outputLevel);
    Logger::LogMessage(LogLevelFull, "The output detail options are:");
    Logger::LogMessage(LogLevelFull, "  0: Minimal (general cache statistics).");
    Logger::LogMessage(LogLevelFull, "  1: Extended (per-instruction statistics).");

    return EXIT_FAILURE;
  }

  // Allocate dynamic memory for the cache trace (could reach several MBs)
  std::unique_ptr<CacheTrace> cacheTrace = std::make_unique<CacheTrace>(cacheTraceFile);
  if (cacheTrace->Size() == 0)
  {
    // Cache trace file is empty or invalid
    Logger::LogMessage(LogLevelFull, "Error: Trace file has no valid entries.");
    Logger::LogMessage(LogLevelFull, "Please provide a cache trace with at least one valid entry.");

    return EXIT_FAILURE;
  }

  // Create the memory system (includes the cache and main memory)
  const CacheConfiguration& cacheConfiguration = {
    cacheSizeKB, CACHE_LINE_SIZE, cacheAssociativity, cacheReplacementAlgorithm };

  MemorySystem memory(cacheConfiguration);

  // Set the requested output detail level
  Logger::SetLogLevel(outputLevel);

  // The main loop that executes the instructions
  for (size_t instructionIndex = 0; instructionIndex < cacheTrace->Size(); instructionIndex++)
  {
    const TraceInstruction& instruction = cacheTrace->Get(instructionIndex);

    // Execute the instruction based on the next trace entry
    switch (instruction.instructionType)
    {
      case InstructionTypeRead:
      {
        memory.Read(instructionIndex, instruction.memoryAddress);
        break;
      }
      case InstructionTypeWrite:
      {
        memory.Write(instructionIndex, instruction.memoryAddress, RandomNumberGenerator::GetRandom());
        break;
      }
      default:
        break;
    }
  }

  // Print detailed cache statistics
  const CacheDetailedStatistics& cacheDetailedStatistics = memory.GetCacheDetailedStatistics();
  cacheDetailedStatistics.Print();

  // Print summary of cache statistics
  const CacheSimpleStatistics cacheSimpleStatistics(cacheDetailedStatistics);
  cacheSimpleStatistics.Print();

  return EXIT_SUCCESS;
}