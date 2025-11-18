cmake_minimum_required(VERSION 3.31)
project(SharpboyCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(SharpboyCore STATIC
	"src/SharpboyCore.cpp"

	"src/Utilities/SST.cpp" 
	"src/Utilities/FileReader.cpp"
	
	"src/CPU/CPU.cpp" 
	"src/CPU/bCPU.cpp"
	"src/CPU/testCPU.cpp"
	"src/CPU/CPUIntr.cpp" 

    "src/Bus.cpp" 
	"src/Sync.cpp"  
	"src/IMU.cpp" 
	"src/PPU.cpp"
	"src/Timer.cpp"

	"src/Cartridges/CartMBC0.cpp" 
	"src/Cartridges/Cartridge.cpp" 

 "include/InterruptDefs.h" "include/ComponentManager.h" "src/ComponentManager.cpp" "include/Interrupts.h" "src/Interrupts.cpp")

target_include_directories(SharpboyCore PUBLIC "include")