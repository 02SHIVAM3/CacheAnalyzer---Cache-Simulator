#include "MainMemory.h"
#include "Logger.h"
#include <array>
#include <iostream>
#include <format>
#include <string>

MainMemory::MainMemory()
{
  // Allocate memory region
  m_memory = std::make_unique<std::array<uint8_t, MAIN_MEMORY_SIZE>>();

  // Fill main memory with zeros
  m_memory.get()->fill(0x00);
}

MainMemory::~MainMemory()
{

}

DataPacket MainMemory::Read(uint32_t startAddress)
{
  DataPacket dataPacket = {};

  // Copy the bytes from main memory into the data packet
  std::copy(m_memory->begin() + startAddress,
    m_memory->begin() + startAddress + DATA_PACKET_SIZE, dataPacket.begin());

  return dataPacket;
}

DataBlock MainMemory::ReadBlock(uint32_t startAddress)
{
  DataBlock dataBlock = {};

  // Copy the bytes from main memory into the data block
  std::copy(m_memory->begin() + startAddress,
    m_memory->begin() + startAddress + DATA_BLOCK_SIZE, dataBlock.begin());

  return dataBlock;
}

void MainMemory::Write(uint32_t startAddress, const DataPacket& source)
{
  // Copy the bytes from the source to main memory
  std::copy(source.begin(), source.end(), m_memory->begin() + startAddress);
}