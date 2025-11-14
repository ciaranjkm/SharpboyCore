cmake_minimum_required(VERSION 3.31)
project(SharpboyCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(SharpboyCore STATIC
	"src/SharpboyCore.cpp"
	  
	"src/Utilities/Testing.cpp" 
	
	"src/CPU.cpp" 
    "src/Bus.cpp" 
	"src/Timing.cpp"  "src/CPUIntr.cpp" "include/Utilities/Logger.h" "include/bCPU.h" "src/bCPU.cpp" "include/testCPU.h" "src/testCPU.cpp")

target_include_directories(SharpboyCore PUBLIC "include")