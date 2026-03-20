#include "ReplacementAlgorithms.h"
#include "RandomNumberGenerator.h"
#include "Cache.h"

ReplacementAlgorithm::ReplacementAlgorithm(size_t wayCount) : m_wayCount(wayCount) {}

LeastRecentlyUsed::LeastRecentlyUsed(size_t wayCount) : ReplacementAlgorithm(wayCount) {}

LeastRecentlyUsed::~LeastRecentlyUsed() {}

void LeastRecentlyUsed::UpdateOnHit(uint32_t accessedWayIndex)
{
  // When a cache line is accessed (cache hit), its position in the list must be updated to be the most recently used
  // Find the location of the accessed way in the access order list
  std::list<uint32_t>::iterator accessedIterator = m_wayPositions[accessedWayIndex];

  // Move the accessed way to be at the front of the list (the most recently used cache line)
  m_accessOrder.splice(m_accessOrder.begin(), m_accessOrder, accessedIterator);
}

void LeastRecentlyUsed::UpdateOnMiss(uint32_t TargetWayIndex)
{
  // When a new line is brought in (cache miss), it becomes the new most recently used cache line
  // Push the target way index (where the new data is stored) to the front of the order list (MRU)
  m_accessOrder.push_front(TargetWayIndex);

  // Add or update the order list iterator in the way positions map
  m_wayPositions[TargetWayIndex] = m_accessOrder.begin();
}

uint32_t LeastRecentlyUsed::GetVictim()
{
  // Get the way index of the least recently used cache line (the victim)
  uint32_t victimIndex = m_accessOrder.back();

  // Remove the victim from the access order list
  m_accessOrder.pop_back();

  // Remove the victim from the way positions map
  m_wayPositions.erase(victimIndex);

  // Return the way index of the victim
  return victimIndex;
}

RandomReplacement::RandomReplacement(size_t wayCount) : ReplacementAlgorithm(wayCount) {}

RandomReplacement::~RandomReplacement() {}

void RandomReplacement::UpdateOnHit(uint32_t accessedWayIndex)
{
  // No action needed on hit
  (void)accessedWayIndex;
}

void RandomReplacement::UpdateOnMiss(uint32_t TargetWayIndex)
{
  // No action needed on miss
  (void)TargetWayIndex;
}

uint32_t RandomReplacement::GetVictim()
{
  return RandomNumberGenerator::GetRandom() % m_wayCount;
}