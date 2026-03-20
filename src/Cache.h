#pragma once

#include "MainMemory.h"
#include "ReplacementAlgorithms.h"
#include "Logger.h"
#include "CacheStatistics.h"
#include <array>
#include <vector>

struct CacheLine
{
  uint32_t tag = 0;
  DataBlock data = {};
  bool valid = false;
};

class CacheSet
{
public:
  CacheSet(size_t wayCount, const std::string& replacementAlgorithm);
  ~CacheSet();

  // Explicitly delete Copy Operations (Necessary because of unique_ptr)
  CacheSet(const CacheSet& other) = delete;
  CacheSet& operator=(const CacheSet& other) = delete;

  // Explicitly default Move Operations (Required for std::vector operations)
  CacheSet(CacheSet&& other) noexcept = default;
  CacheSet& operator=(CacheSet&& other) noexcept = default;

  const CacheLine& ReadCacheLine(const CacheAddressFields& addressfields,
                                 MainMemory& mainMemory, CacheInstructionData& instructionData);

  void WriteCacheLine(const CacheAddressFields& addressfields, const DataPacket& sourceData,
                      MainMemory& mainMemory, CacheInstructionData& instructionData);

  void UpdateOnHit(uint32_t accessedWayIndex);
  void Flush();

private:
  std::vector<CacheLine> m_lines;                                 // Vector of cache lines that form the set
  std::unique_ptr<ReplacementAlgorithm> m_replacementAlgorithm;   // Algorithm that replaces cache lines

  // Find the way index of a cache line based on the given tag
  uint32_t FindWayIndex(uint32_t tag) const;

  // Find the next available cache line in the set
  uint32_t FindAvailableLine() const;

  // Store the given tag and data in the cache line found at the requested way index
  CacheLine& StoreCacheLine(uint32_t wayIndex, uint32_t tag, DataBlock& source);
};

class Cache
{
public:

  Cache(const CacheConfiguration& cacheConfiguration, MainMemory& memory);
  ~Cache();

  DataPacket Read(uint32_t address, CacheInstructionData& instructionData);
  void Write(uint32_t address, const DataPacket& data, CacheInstructionData& instructionData);
  void Flush();

  const CacheSimpleStatistics& GetStatistics() const;

private:
  CacheAddressConfiguration m_addressConfig;    // Sizes and locations of the fields within a memory address
  std::vector<CacheSet> m_sets;                 // Vector of cache sets
  MainMemory& m_mainMemory;                     // Reference to main memory
  CacheSimpleStatistics m_statistics;

  // Returns the internal fields in the given memory address
  CacheAddressFields GetAddressFields(uint32_t address) const;
};