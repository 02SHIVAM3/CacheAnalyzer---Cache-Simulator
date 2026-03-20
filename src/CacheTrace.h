#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

constexpr size_t MAX_INSTRUCTION_COUNT = 1048576;				// Maximum number of instructions in the trace
constexpr size_t ENTRY_LENGTH = 12;											// The expected number of characters in a single trace entry
constexpr size_t ENTRY_TYPE_POSITION = 0;								// Position of the instruction type in the trace entry
constexpr size_t ENTRY_ADDRESS_POSITION = 2;						// Position of the memory address in the trace entry
constexpr size_t ENTRY_ADDRESS_LENGTH = 10;							// Length of the memory address in the trace entry

// A list of supported instruction types
enum EInstructionType { InstructionTypeUnknown, InstructionTypeRead, InstructionTypeWrite, InstructionTypeFlush };

// Mapping between the instruction name and the supported instruction type (enum)
static std::unordered_map<char, EInstructionType> s_instructionNames = {
	{'R', InstructionTypeRead}, {'W', InstructionTypeWrite}, {'F', InstructionTypeFlush}
};

struct TraceInstruction
{
	TraceInstruction(EInstructionType type, uint32_t address)
		: instructionType(type), memoryAddress(address) {}
	~TraceInstruction() {}

	EInstructionType instructionType = InstructionTypeUnknown;
	uint32_t memoryAddress = 0x00000000;
};

class CacheTrace
{
public:
	CacheTrace(std::ifstream& traceFile);
	~CacheTrace();

	const TraceInstruction& Get(size_t index) const;
	uint32_t Size() const;

private:
	std::vector<TraceInstruction> m_instructions;			// List of instructions read from the trace file
	uint32_t m_instructionCount = 0;									// Number of instructions in the trace so far

	// Wait for user confirmation if errors were found
	void WaitForConfirmation();
};