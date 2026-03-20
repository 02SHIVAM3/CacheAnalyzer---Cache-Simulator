#pragma once

#include <random>

constexpr size_t DEFAULT_SEED_NUMBER = 1234;        // Maximum output detail level

class RandomNumberGenerator
{
public:
  static uint32_t GetRandom();
  static uint32_t SetSeed(uint32_t seed);

private:

  // A random number generator
  static std::mt19937 s_randomGenerator;

  // Define distribution to generate random numbers
  static std::uniform_int_distribution<uint32_t> s_distribution;

  // A constant seed number needed for reproducability
  static uint32_t s_Seed;
};