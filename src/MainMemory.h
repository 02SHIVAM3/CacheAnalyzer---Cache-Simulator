#pragma once

#include "SystemDefinitions.h"
#include <memory>

class MainMemory
{
public:
  MainMemory();
  ~MainMemory();

  DataPacket Read(uint32_t startAddress);
  DataBlock ReadBlock(uint32_t startAddress);
  void Write(uint32_t startAddress, const DataPacket& sourceData);

private:
  std::unique_ptr<std::array<uint8_t, MAIN_MEMORY_SIZE>> m_memory;
};