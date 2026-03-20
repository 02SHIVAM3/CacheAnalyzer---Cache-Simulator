#include "Cache.h"
#include "Logger.h"
#include <string>
#include <iostream>
#include <format>

// Names of supported replacement algorithms
const std::string LEAST_RECENTLY_USED = "LRU";
const std::string RANDOM_REPLACEMENT = "RR";

CacheSet::CacheSet(size_t wayCount, const std::string& replacementAlgorithm)
{
  // Resize the vector of lines according to the required number of ways
  m_lines.resize(wayCount);

  // Create the requested replacement algorithm
  if (replacementAlgorithm == LEAST_RECENTLY_USED)
  {
    m_replacementAlgorithm = std::make_unique<LeastRecentlyUsed>(wayCount);
  }
  else
  {
    m_replacementAlgorithm = std::make_unique<RandomReplacement>(wayCount);
  }
}

CacheSet::~CacheSet()
{

}

const CacheLine& CacheSet::ReadCacheLine(const CacheAddressFields& addressfields,
  MainMemory& mainMemory, CacheInstructionData& instructionData)
{
  // Find the requested cache line
  uint32_t wayIndex = FindWayIndex(addressfields.tag);

  if (wayIndex != INVALID_WAY_INDEX)    // Cache hit (the requested tag was found in the cache set)
  {
    // notify the replacement algorithm about the accessed cache line
    m_replacementAlgorithm->UpdateOnHit(wayIndex);

    // Update cache statistics (access result)
    instructionData.accessResult = CacheAccessResultHit;

    // Return the cache line stored at the way index
    return m_lines[wayIndex];
  }

  else    // Cache Miss (the requested tag was not found in the cache set)
  {
    // Find the beginning of the cache line (clear 6 LSBs)
    uint32_t lineStart = addressfields.address & ~(0x3F);

    // Read the data at the calculated address from main memory
    DataBlock dataBlock = mainMemory.ReadBlock(lineStart);

    // Find the next available cache line in the set
    uint32_t targetWayIndex = FindAvailableLine();

    if (targetWayIndex == INVALID_WAY_INDEX)    // The set is full
    {
      // Choose a victim cache line to replace
      targetWayIndex = m_replacementAlgorithm->GetVictim();

      // Update cache statistics (victim way index)
      instructionData.victimWayIndex = targetWayIndex;
    }

    // Update cache statistics (access result)
    instructionData.accessResult = CacheAccessResultMiss;

    // Store the data read from main memory into the target cache line and return it
    return StoreCacheLine(targetWayIndex, addressfields.tag, dataBlock);
  }
}

void CacheSet::WriteCacheLine(const CacheAddressFields& addressfields,
  const DataPacket& sourceData, MainMemory& mainMemory, CacheInstructionData& instructionData)
{
  // Find the requested line in the cache
  uint32_t wayIndex = FindWayIndex(addressfields.tag);

  if (wayIndex != INVALID_WAY_INDEX) // Cache hit (the requested tag was found in the cache set)
  {
    // Copy the source data to the cache memory
    std::copy(sourceData.begin(), sourceData.end(),
      m_lines[wayIndex].data.begin() + addressfields.byteOffset);

    // Update cache statistics (access result)
    instructionData.accessResult = CacheAccessResultHit;
  }
  else // Cache miss (the requested tag was not found in the cache set)
  {
    // Update cache statistics (access result)
    instructionData.accessResult = CacheAccessResultMiss;
  }

  // Write-through / No-Write-Allocate cache (immediately write modified data to main memory)
  mainMemory.Write(addressfields.address, sourceData);
}

void CacheSet::Flush()
{
  for (CacheLine& cacheLine : m_lines)
  {
    // Mark all cache lines in the set as invalid
    cacheLine.valid = false;
  }
}

uint32_t CacheSet::FindWayIndex(uint32_t tag) const
{
  // Find the way of a cache line based on the given tag
  for (uint32_t way = 0; way < m_lines.size(); way++)
  {
    if (m_lines[way].valid && m_lines[way].tag == tag)
    {
      return way;
    }
  }

  // Cache line wasn't found
  return INVALID_WAY_INDEX;
}

uint32_t CacheSet::FindAvailableLine() const
{
  // Find the way of the next available cache line
  for (uint32_t way = 0; way < m_lines.size(); way++)
  {
    if (!m_lines[way].valid)
    {
      return way;
    }
  }

  // Cache set is full
  return INVALID_WAY_INDEX;
}

CacheLine& CacheSet::StoreCacheLine(uint32_t wayIndex, uint32_t tag, DataBlock& source)
{
  CacheLine& targetCacheLine = m_lines[wayIndex];

  // Copy the data from main memory to the target cache line
  std::copy(source.begin(), source.end(), targetCacheLine.data.begin());

  // Set new cache line attributes
  targetCacheLine.valid = true;
  targetCacheLine.tag = tag;

  // Notify replacement algorithm that a new cache line has been stored
  m_replacementAlgorithm->UpdateOnMiss(wayIndex);

  // Return the new cache line
  return targetCacheLine;
}

Cache::Cache(const CacheConfiguration& cacheConfiguration, MainMemory& memory) : m_mainMemory(memory)
{
  // Resize the vector of sets according to the cache size and associativity requirements
  for (size_t setIndex = 0; setIndex < cacheConfiguration.setCount; setIndex++)
  {
    m_sets.emplace_back(cacheConfiguration.wayCount, cacheConfiguration.replacementAlgorithm);
  }

  // Calculate the size and location of each field in a memory address
  m_addressConfig.byteOffset = {0, static_cast<size_t>(std::log2(cacheConfiguration.lineSize)) };
  m_addressConfig.setIndex = { m_addressConfig.byteOffset.length, static_cast<size_t>(std::log2(cacheConfiguration.setCount)) };
  m_addressConfig.tag = { m_addressConfig.setIndex.start + m_addressConfig.setIndex.length,
                          MEMORY_ADDRESS_SIZE - m_addressConfig.setIndex.length - m_addressConfig.byteOffset.length };
}

Cache::~Cache()
{

}

DataPacket Cache::Read(uint32_t address, CacheInstructionData& instructionData)
{
  // Parse the given memory address
  CacheAddressFields addressFields = GetAddressFields(address);

  // Update statistics (address parts)
  instructionData.addressFields.tag = addressFields.tag;
  instructionData.addressFields.setIndex = addressFields.setIndex;
  instructionData.addressFields.byteOffset = addressFields.byteOffset;

  // Read the cache line from the cache set
  const CacheLine& cacheLine = m_sets[addressFields.setIndex].ReadCacheLine(addressFields, m_mainMemory, instructionData);

  DataPacket dataPacket = {};

  // Copy the bytes from the cache line to the data packet
  std::copy(cacheLine.data.begin() + addressFields.byteOffset,
    cacheLine.data.begin() + addressFields.byteOffset + DATA_PACKET_SIZE,
    dataPacket.begin());

  return dataPacket;
}

void Cache::Write(uint32_t address, const DataPacket& data, CacheInstructionData& instructionData)
{
  CacheAddressFields addressFields = GetAddressFields(address);

  // Update statistics (address parts)
  instructionData.addressFields.tag = addressFields.tag;
  instructionData.addressFields.setIndex = addressFields.setIndex;
  instructionData.addressFields.byteOffset = addressFields.byteOffset;

  // Write the data to the cache line
  m_sets[addressFields.setIndex].WriteCacheLine(addressFields, data, m_mainMemory, instructionData);
}

void Cache::Flush()
{
  for (CacheSet& cacheSet : m_sets)
  {
    // Flush all cache sets in the cache
    cacheSet.Flush();
  }
}

const CacheSimpleStatistics& Cache::GetStatistics() const
{
  return m_statistics;
}

CacheAddressFields Cache::GetAddressFields(uint32_t address) const
{
  return CacheAddressFields(address, address >> m_addressConfig.tag.start,
    (address >> m_addressConfig.setIndex.start) & ((1 << m_addressConfig.setIndex.length) - 1),
    address & ((1 << m_addressConfig.byteOffset.length) - 1));
}