#pragma once

#include <string>
#include <array>
#include <unordered_map>
#include <algorithm>

constexpr size_t INVALID_WAY_INDEX = 0xFFFFFFFF;
constexpr size_t REPLACEMENT_ALGORITHMS_COUNT = 2;

enum ECacheAccessType { CacheAccessTypeUnknown, CacheAccessTypeRead, CacheAccessTypeWrite };
enum ECacheAccessResult { CacheAccessResultUnknown, CacheAccessResultHit, CacheAccessResultMiss };

// Mapping between the cache access type and its string name
static const std::unordered_map<ECacheAccessType, std::string> s_accessTypeNames = {
  {CacheAccessTypeUnknown, "Unknown"}, {CacheAccessTypeRead, "Read "}, {CacheAccessTypeWrite, "Write"}
};

// Mapping between the cache access result and its string name
static const std::unordered_map<ECacheAccessResult, std::string> s_accessResultNames = {
  {CacheAccessResultUnknown, "Unknown"}, {CacheAccessResultHit, "Hit "}, {CacheAccessResultMiss, "Miss"}
};

static const std::array<std::string, REPLACEMENT_ALGORITHMS_COUNT> s_replacementAlgoNames = {
  "RR",   // Random Replacement algorithm
  "LRU"   // Full Least Recently Used algorithm
};

// Describes a single bit field within a cache memory address
struct CacheAddressField
{
  size_t start = 0;     // Start bit location of the field in the address
  size_t length = 0;    // Length of the field in terms of bits

  CacheAddressField() {}
  CacheAddressField(size_t fieldStart, size_t fieldLength) : start(fieldStart), length(fieldLength) {}
};

// Describes the fields within a cache memory address
struct CacheAddressConfiguration
{
  CacheAddressField tag{};
  CacheAddressField setIndex{};
  CacheAddressField byteOffset{};
};

// Describes a cache memory address and its internal fields
struct CacheAddressFields
{
  uint32_t address = 0x00000000;
  uint32_t tag = 0;
  uint32_t setIndex = 0;
  uint32_t byteOffset = 0;

  CacheAddressFields() {}
  CacheAddressFields(uint32_t cacheAddress, uint32_t cacheTag, uint32_t cacheSetIndex, uint32_t cacheByteOffset)
    : address(cacheAddress), tag(cacheTag), setIndex(cacheSetIndex), byteOffset(cacheByteOffset) {}
};

// A data structure that holds the configuration of the cache
struct CacheConfiguration
{
  CacheConfiguration(size_t cacheSizeKB, size_t cacheLineSize,
    size_t cacheAssociativity, std::string& cacheReplacementAlgorithm)
    : cacheSize(cacheSizeKB), wayCount(cacheAssociativity),
    lineSize(cacheLineSize), replacementAlgorithm(cacheReplacementAlgorithm)
  {
    // Calculate the number of sets in the cache
    setCount = (cacheSize * 1024) / (wayCount * cacheLineSize);
  }

  size_t cacheSize = 0;                       // Size of the cache (KB)
  size_t wayCount = 0;                        // Number of ways in the cache
  size_t setCount = 0;                        // Number of sets in the cache
  size_t lineSize = 0;                        // Size of a single cache line
  std::string replacementAlgorithm = {};      // Cache replacement algorithm name
};