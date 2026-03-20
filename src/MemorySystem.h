#pragma once

#include "MainMemory.h"
#include "Cache.h"

constexpr uint32_t ADDRESS_ALIGNMENT_BITS = 0x03;   // Used for clearing the first 2 bits in address alignment

class MemorySystem
{
public:
  MemorySystem(const CacheConfiguration& cacheConfiguration);
  ~MemorySystem();

  uint32_t Read(size_t id, uint32_t address);
  void Write(size_t id, uint32_t address, uint32_t value);

  const CacheDetailedStatistics& GetCacheDetailedStatistics() const;

private:
  MainMemory m_mainMemory{};
  std::unique_ptr<Cache> m_cache{};
  CacheDetailedStatistics m_cacheDetailedStatistics{};

  // Checks if the given memory address is aligned to 4 bytes
  bool IsAligned(uint32_t address) const;

  // Aligns the given memory address to the closest-lower multiple of 4
  uint32_t AlignMemoryAddress(uint32_t address) const;
};