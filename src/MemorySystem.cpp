#include <cstring>
#include "MemorySystem.h"

MemorySystem::MemorySystem(const CacheConfiguration& cacheConfiguration)
{
  // Create an instance of the cache
  m_cache = std::make_unique<Cache>(cacheConfiguration, m_mainMemory);
}

MemorySystem::~MemorySystem()
{

}

uint32_t MemorySystem::Read(size_t id, uint32_t address)
{
  uint32_t alignedAddress = address;

  // Check if the given memory address needs to be aligned
  if (!IsAligned(address))
  {
    // Align the given memory address to the closest-lower multiple of 4
    alignedAddress = AlignMemoryAddress(address);
  }

  // Update statistics (create new entry)
  CacheInstructionData& instructionData = m_cacheDetailedStatistics.AddInstruction(
    id, CacheAccessTypeRead, address, alignedAddress);

  // Read the data packet from memory
  const DataPacket& dataPacket = m_cache->Read(alignedAddress, instructionData);

  // Convert the byte array (DataPacket) into a single 32-bit unsigned integer
  uint32_t value = 0x00000000;
  memcpy(&value, dataPacket.data(), sizeof(uint32_t));

  // Update statistics (data)
  instructionData.data = value;

  return value;
}

void MemorySystem::Write(size_t id, uint32_t address, uint32_t value)
{
  DataPacket dataPacket = {};
  uint32_t alignedAddress = address;

  // Convert the single 32-bit unsigned integer into a byte array (DataPacket)
  memcpy(dataPacket.data(), &value, sizeof(uint32_t));

  // Check if the given memory address needs to be aligned
  if (!IsAligned(address))
  {
    // Align the given memory address to the closest-lower multiple of 4
    alignedAddress = AlignMemoryAddress(address);
  }

  // Update statistics (create new entry)
  CacheInstructionData& instructionData = m_cacheDetailedStatistics.AddInstruction(
    id, CacheAccessTypeWrite, address, alignedAddress);

  // Update statistics (data)
  instructionData.data = value;

  // Write the data packet to memory
  m_cache->Write(alignedAddress, dataPacket, instructionData);
}

const CacheDetailedStatistics& MemorySystem::GetCacheDetailedStatistics() const
{
  return m_cacheDetailedStatistics;
}

bool MemorySystem::IsAligned(uint32_t address) const
{
  return (address & ADDRESS_ALIGNMENT_BITS) == 0;
}

uint32_t MemorySystem::AlignMemoryAddress(uint32_t address) const
{
  return address & ~(ADDRESS_ALIGNMENT_BITS);
}