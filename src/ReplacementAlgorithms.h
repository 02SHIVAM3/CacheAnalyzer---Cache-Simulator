#pragma once

#include <cstdint>
#include <unordered_map>
#include <list>

class ReplacementAlgorithm
{
public:
  virtual ~ReplacementAlgorithm() = default;
  virtual void UpdateOnHit(uint32_t accessedWayIndex) = 0;
  virtual void UpdateOnMiss(uint32_t TargetWayIndex) = 0;
  virtual uint32_t GetVictim() = 0;

protected:
  ReplacementAlgorithm(size_t wayCount);
  size_t m_wayCount = 0;                  // Number of ways in the cache
};

class LeastRecentlyUsed : public ReplacementAlgorithm
{
public:
  LeastRecentlyUsed(size_t wayCount);
  ~LeastRecentlyUsed();

  void UpdateOnHit(uint32_t accessedWayIndex) override;
  void UpdateOnMiss(uint32_t TargetWayIndex) override;
  uint32_t GetVictim() override;

private:

  // Doubly-Linked List that stores the order of the accessed cache lines
  std::list<uint32_t> m_accessOrder;

  // Mapping between the cache way index and its position in the list
  std::unordered_map<uint32_t, std::list<uint32_t>::iterator> m_wayPositions;
};

class RandomReplacement : public ReplacementAlgorithm
{
public:
  RandomReplacement(size_t wayCount);
  ~RandomReplacement();

  void UpdateOnHit(uint32_t accessedWayIndex) override;
  void UpdateOnMiss(uint32_t TargetWayIndex) override;
  uint32_t GetVictim() override;
};