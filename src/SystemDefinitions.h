#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

constexpr size_t MEMORY_ADDRESS_SIZE = 32;              // 32-bit memory addresses
constexpr size_t MAIN_MEMORY_SIZE = 4 * 1024 * 1024;    // 4MB main memory
constexpr size_t DATA_PACKET_SIZE = 4;                  // Standard read/write operation data packet (4 bytes)
constexpr size_t DATA_BLOCK_SIZE = 64;                  // Special read operations with large data block (64 bytes)

// A single data packet transfered during a read/write operation
typedef std::array<uint8_t, DATA_PACKET_SIZE> DataPacket;

// A single data block transfered during a cache miss event
typedef std::array<uint8_t, DATA_BLOCK_SIZE> DataBlock;