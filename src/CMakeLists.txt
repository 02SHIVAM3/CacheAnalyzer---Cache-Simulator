# Define Source Files
set(SOURCE_FILES
	Cache.cpp
	CacheStatistics.cpp
	CacheTrace.cpp
	Logger.cpp
	MainMemory.cpp
	MemorySystem.cpp
	RandomNumberGenerator.cpp
	ReplacementAlgorithms.cpp
	CacheIQ.cpp
)

# Create Executable Target
add_executable(CacheIQ ${SOURCE_FILES})

# Add Header Files to the Project Structure
target_include_directories(CacheIQ PRIVATE ${CMAKE_CURRENT_LIST_DIR})