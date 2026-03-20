#include "RandomNumberGenerator.h"

// Initialize default seed number
uint32_t RandomNumberGenerator::s_Seed = DEFAULT_SEED_NUMBER;

// Initialize the random number generator
std::mt19937 RandomNumberGenerator::s_randomGenerator { RandomNumberGenerator::s_Seed };

// Initialize random distributions
std::uniform_int_distribution<uint32_t> RandomNumberGenerator::s_distribution { 0, UINT32_MAX };

uint32_t RandomNumberGenerator::GetRandom()
{
  // Generate a random 32-bit number with uniform distribution
  return s_distribution(s_randomGenerator);
}

uint32_t RandomNumberGenerator::SetSeed(uint32_t seed)
{
  return s_Seed = seed;
}
