#pragma once

#include <vector>
#include <cmath>
#include "SystemDefinitions.h"
#include "CacheDefinitions.h"
#include "Logger.h"

// Information about a single cache memory instruction
struct CacheInstructionData
{
  size_t id = 0;
  ECacheAccessType accessType = CacheAccessTypeUnknown;
  ECacheAccessResult accessResult = CacheAccessResultUnknown;
  CacheAddressFields addressFields = {};
  uint32_t alignedMemoryAddress = 0x00000000;
  uint32_t data = 0x00000000;
  uint32_t victimWayIndex = INVALID_WAY_INDEX;

  void Print() const;
};

struct CacheDetailedStatistics
{
  std::vector<CacheInstructionData> cacheStatistics = {};

  CacheInstructionData& AddInstruction(size_t idNumber,
    ECacheAccessType cacheAccessType, uint32_t memoryAddress, uint32_t alignedMemoryAddress);

  void Print() const;
};

struct CacheSimpleStatistics
{
  size_t controlInstructionCount = 0;       // Number of executed non-memory access instructions
  size_t readHits = 0;                      // Number of cache read instruction hits
  size_t readMisses = 0;                    // Number of cache read instruction misses
  size_t writeHits = 0;                     // Number of cache write instruction hits
  size_t writeMisses = 0;                   // Number of cache write instruction misses
  size_t totalEvictions = 0;                // Total number of evicted cache lines

  CacheSimpleStatistics();
  CacheSimpleStatistics(const CacheDetailedStatistics& cacheDetailedStatistics);
  ~CacheSimpleStatistics();

  // Returns the number of executed read instructions
  size_t GetReadInstructionCount() const;

  // Returns the hit rate for read instructions
  float_t GetReadHitRate() const;

  // Returns the miss rate for read instructions
  float_t GetReadMissRate() const;

  // Returns the number of bytes transferred during read instructions
  size_t GetReadBytesTransferred() const;

  // Returns the number of executed write instructions
  size_t GetWriteInstructionCount() const;

  // Returns the hit rate for write instructions
  float_t GetWriteHitRate() const;

  // Returns the miss rate for write instructions
  float_t GetWriteMissRate() const;

  // Returns the number of bytes transferred during write instructions
  size_t GetWriteBytesTransferred() const;

  // Returns the total number of executed memory (read/write) instructions
  size_t GetMemoryInstructionCount() const;

  // Returns the total number of executed instructions
  size_t GetTotalInstructionCount() const;

  // Returns the total number of memory instruction cache hits
  size_t GetTotalHits() const;

  // Returns the total number of memory instruction cache misses
  size_t GetTotalMisses() const;

  // Returns the total hit rate for all memory instructions
  float_t GetTotalHitRate() const;

  // Returns the total miss rate for all memory instructions
  float_t GetTotalMissRate() const;

  // Returns the total number of bytes transferred during memory instructions
  size_t GetTotalBytesTransferred() const;

  // Print aggregate cache statistics
  void Print() const;
};