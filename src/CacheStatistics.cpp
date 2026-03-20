#include "CacheStatistics.h"
#include "Logger.h"

void CacheInstructionData::Print() const
{
  // Print all information about a single instruction
  Logger::LogMessage(LogLevelExtended, "[%08d] Instruction: %s | Original Address: 0x%08x | Aligned Address: 0x%08x "
    "| Tag: %04d | Set Index: %04d | Byte Offset: %02d | Result: %s | Data: 0x%08x | Evicted Way Index: %02d",
    id, s_accessTypeNames.at(accessType).c_str(), addressFields.address, alignedMemoryAddress,
    addressFields.tag, addressFields.setIndex, addressFields.byteOffset,
    s_accessResultNames.at(accessResult).c_str(), data, victimWayIndex);
}

CacheInstructionData& CacheDetailedStatistics::AddInstruction(
  size_t idNumber, ECacheAccessType cacheAccessType, uint32_t memoryAddress, uint32_t alignedMemoryAddress)
{
  CacheInstructionData newInstruction;
  newInstruction.id = idNumber;
  newInstruction.accessType = cacheAccessType;
  newInstruction.addressFields.address = memoryAddress;
  newInstruction.alignedMemoryAddress = alignedMemoryAddress;

  // Add the new instruction information to the vector
  cacheStatistics.push_back(newInstruction);

  return cacheStatistics.back();
}

void CacheDetailedStatistics::Print() const
{
  // Print information about all recorded instructions
  for (const CacheInstructionData& instructionData : cacheStatistics) { instructionData.Print(); }
}

CacheSimpleStatistics::CacheSimpleStatistics()
{

}

CacheSimpleStatistics::CacheSimpleStatistics(const CacheDetailedStatistics& cacheDetailedStatistics)
{
  // Aggregate all information into a short summary
  for (const CacheInstructionData& instructionData : cacheDetailedStatistics.cacheStatistics)
  {
    if (instructionData.accessType == CacheAccessTypeRead)
    {
      if (instructionData.accessResult == CacheAccessResultHit)
      {
        readHits++;
      }
      else if (instructionData.accessResult == CacheAccessResultMiss)
      {
        readMisses++;
      }
      else
      {
        // Error: Unknown cache access result
      }
    }
    else if (instructionData.accessType == CacheAccessTypeWrite)
    {
      if (instructionData.accessResult == CacheAccessResultHit)
      {
        writeHits++;
      }
      else if (instructionData.accessResult == CacheAccessResultMiss)
      {
        writeMisses++;
      }
      else
      {
        // Error: Unknown cache access result
      }
    }
    else
    {
      // Error: Not a memory instruction
    }

    // Count total cache evictions
    if (instructionData.victimWayIndex != INVALID_WAY_INDEX)
    {
      totalEvictions++;
    }
  }
}

CacheSimpleStatistics::~CacheSimpleStatistics()
{

}

size_t CacheSimpleStatistics::GetReadInstructionCount() const
{
  return (readHits + readMisses);
}

float_t CacheSimpleStatistics::GetReadHitRate() const
{
  return (static_cast<float_t>(readHits) / GetReadInstructionCount());
}

float_t CacheSimpleStatistics::GetReadMissRate() const
{
  return (static_cast<float_t>(readMisses) / GetReadInstructionCount());
}

size_t CacheSimpleStatistics::GetReadBytesTransferred() const
{
  return (GetReadInstructionCount() * DATA_PACKET_SIZE);
}

size_t CacheSimpleStatistics::GetWriteInstructionCount() const
{
  return (writeHits + writeMisses);
}

float_t CacheSimpleStatistics::GetWriteHitRate() const
{
  return (static_cast<float_t>(writeHits) / GetWriteInstructionCount());
}

float_t CacheSimpleStatistics::GetWriteMissRate() const
{
  return (static_cast<float_t>(writeMisses) / GetWriteInstructionCount());
}

size_t CacheSimpleStatistics::GetWriteBytesTransferred() const
{
  return (GetWriteInstructionCount() * DATA_PACKET_SIZE);
}

size_t CacheSimpleStatistics::GetMemoryInstructionCount() const
{
  return (GetReadInstructionCount() + GetWriteInstructionCount());
}

size_t CacheSimpleStatistics::GetTotalInstructionCount() const
{
  return (GetMemoryInstructionCount() + controlInstructionCount);
}

size_t CacheSimpleStatistics::GetTotalHits() const
{
  return (readHits + writeHits);
}

size_t CacheSimpleStatistics::GetTotalMisses() const
{
  return (readMisses + writeMisses);
}

float_t CacheSimpleStatistics::GetTotalHitRate() const
{
  return (static_cast<float_t>(GetTotalHits()) / GetMemoryInstructionCount());
}

float_t CacheSimpleStatistics::GetTotalMissRate() const
{
  return (static_cast<float_t>(GetTotalMisses()) / GetMemoryInstructionCount());
}

size_t CacheSimpleStatistics::GetTotalBytesTransferred() const
{
  return (GetReadBytesTransferred() + GetWriteBytesTransferred());
}

void CacheSimpleStatistics::Print() const
{
  Logger::LogMessage(LogLevelMinimal, "================ Cache Statistics ================");
  Logger::BlankLine(LogLevelMinimal);

  // General statistics
  Logger::LogMessage(LogLevelMinimal, "- Total number of instructions: %d", GetTotalInstructionCount());
  Logger::LogMessage(LogLevelMinimal, "- Total number of hits: %d", GetTotalHits());
  Logger::LogMessage(LogLevelMinimal, "- Total number of misses: %d", GetTotalMisses());
  Logger::LogMessage(LogLevelMinimal, "- Total hit rate: %.2f%%", GetTotalHitRate() * 100.0);
  Logger::LogMessage(LogLevelMinimal, "- Total miss rate: %.2f%%", GetTotalMissRate() * 100.0);
  Logger::LogMessage(LogLevelMinimal, "- Total data transferred: %d bytes", GetTotalBytesTransferred());
  Logger::BlankLine(LogLevelMinimal);

  // Read instruction statistics
  Logger::LogMessage(LogLevelExtended, "- Number of read instructions: %d", GetReadInstructionCount());
  Logger::LogMessage(LogLevelExtended, "- Read data transferred: %d bytes", GetReadBytesTransferred());
  if (GetReadInstructionCount() > 0)
  {
    Logger::LogMessage(LogLevelExtended, "- Read hit rate: %.2f%%", GetReadHitRate() * 100.0);
    Logger::LogMessage(LogLevelExtended, "- Read miss rate: %.2f%%", GetReadMissRate() * 100.0);
  }
  Logger::BlankLine(LogLevelExtended);
  
  // Write instruction statistics
  Logger::LogMessage(LogLevelExtended, "- Number of write instructions: %d", GetWriteInstructionCount());
  Logger::LogMessage(LogLevelExtended, "- Write data transferred: %d bytes", GetWriteBytesTransferred());
  if (GetWriteInstructionCount() > 0)
  {
    Logger::LogMessage(LogLevelExtended, "- Write hit rate: %.2f%%", GetWriteHitRate() * 100.0);
    Logger::LogMessage(LogLevelExtended, "- Write miss rate: %.2f%%", GetWriteMissRate() * 100.0);
  }
  Logger::BlankLine(LogLevelExtended);
  Logger::LogMessage(LogLevelMinimal, "==================================================");
}