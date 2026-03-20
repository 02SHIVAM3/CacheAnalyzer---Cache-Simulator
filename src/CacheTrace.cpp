#include "CacheTrace.h"
#include "Logger.h"
#include <iostream>
#include <format>
#include <fstream>

CacheTrace::CacheTrace(std::ifstream& traceFile)
{
  std::string currentInstruction;
  bool errorsFound = false;

  while (std::getline(traceFile, currentInstruction))
  {
    // Skip blank lines
    if (currentInstruction.empty()) { continue; }

    // Check if the trace reached maximum capacity
    if (m_instructionCount == MAX_INSTRUCTION_COUNT)
    {
      Logger::LogMessage(LogLevelFull, "Error: Trace file is too long.");
      Logger::LogMessage(LogLevelFull, "Maximum number of entries allowed: %d", MAX_INSTRUCTION_COUNT);
      Logger::LogMessage(LogLevelFull, "The simulator will continue with the first %d instructions", MAX_INSTRUCTION_COUNT);
      errorsFound = true;

      break;    // Stop parsing and exit the loop
    }

    if (currentInstruction.size() != ENTRY_LENGTH)
    {
      Logger::LogMessage(LogLevelFull, "Error: Invalid entry (unexpected entry length) was found while parsing the trace.");
      Logger::LogMessage(LogLevelFull, "Entry: %d, Length: %d", m_instructionCount + 1, currentInstruction.size());
      errorsFound = true;

      continue;   // Skip the invalid entry
    }

    // Try parsing the line
    // Find the instruction letter from the trace in the instruction names mapping
    EInstructionType instructionType = InstructionTypeUnknown;
    char InstructionName = currentInstruction[ENTRY_TYPE_POSITION];

    // Check if the instruction name from the trace is supported
    auto instructionIndex = s_instructionNames.find(InstructionName);
    if (instructionIndex != s_instructionNames.end())
    {
      // Instruction was found in the list of supported instructions
      instructionType = instructionIndex->second;
    }
    else
    {
      Logger::LogMessage(LogLevelFull, "Error: Invalid instruction name found while parsing the trace.");
      Logger::LogMessage(LogLevelFull, "Entry: %d, Instruction: %c", m_instructionCount + 1, InstructionName);
      errorsFound = true;

      continue;   // Skip the invalid instruction
    }

    // Try extracting the address from the line
    uint32_t memoryAddress = 0x00000000;
    std::string addressString = currentInstruction.substr(
      ENTRY_ADDRESS_POSITION, ENTRY_ADDRESS_LENGTH);

    try
    {
      // Try converting the address string to a base 16 integer
      memoryAddress = std::stoul(addressString, nullptr, 16);
    }
    catch (const std::invalid_argument& e)
    {
      Logger::LogMessage(LogLevelFull, "Error: Invalid memory address (incorrect format) was found while parsing the trace.");
      Logger::LogMessage(LogLevelFull, "Entry: %d, Address: %s", m_instructionCount + 1, addressString.c_str());
      errorsFound = true;

      continue;   // Skip the invalid instruction
    }
    catch (const std::out_of_range& e)
    {
      Logger::LogMessage(LogLevelFull, "Error: Invalid memory address (address is too large) was found while parsing the trace.");
      Logger::LogMessage(LogLevelFull, "Entry: %d, Address: %s", m_instructionCount + 1, addressString.c_str());
      errorsFound = true;

      continue;   // Skip the invalid instruction
    }
    catch (...)
    {
      Logger::LogMessage(LogLevelFull, "Error: Invalid memory address was found while parsing the trace.");
      Logger::LogMessage(LogLevelFull, "Entry: %d, Address: %s", m_instructionCount + 1, addressString.c_str());
      errorsFound = true;

      continue;   // Skip the invalid instruction
    }

    // Create a TraceInstruction instance with the extracted information
    m_instructions.emplace_back(instructionType, memoryAddress);

    // Increment the instruction counter
    m_instructionCount++;
  }

  if (errorsFound)
  {
    // If errors were found - stop and wait for user confirmation
    WaitForConfirmation();
  }
}

CacheTrace::~CacheTrace()
{

}

const TraceInstruction& CacheTrace::Get(size_t index) const
{
  return m_instructions[index];
}

uint32_t CacheTrace::Size() const
{
  return m_instructionCount;
}

void CacheTrace::WaitForConfirmation()
{
  Logger::LogMessage(LogLevelFull, "Press ENTER to start simulation ...");
  Logger::Flush();

  // Wait for the user to input a single character
  std::cin.get();
}