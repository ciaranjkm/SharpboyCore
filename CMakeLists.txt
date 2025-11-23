cmake_minimum_required(VERSION 3.31)
project(SharpboyCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(SharpboyCore STATIC
	"src/SharpboyCore.cpp"
	"src/ComponentManager.cpp"
	"src/Sync.cpp"  

	"src/Components/CPU/CPU.cpp" 
	"src/Components/CPU/bCPU.cpp"
	"src/Components/CPU/testCPU.cpp"
	"src/Components/CPU/CPUIntr.cpp" 

	"src/Components/Interrupts.cpp"
    "src/Components/Bus.cpp" 
	"src/Components/IMU.cpp" 
	"src/Components/PPU.cpp"
	"src/Components/Timer.cpp"

	"src/Components/Cartridges/CartMBC0.cpp" 
	"src/Components/Cartridges/Cartridge.cpp" 
	"src/Components/Cartridges/CartMBC1.cpp"

	"src/Utilities/SST.cpp" 
	"src/Utilities/FileReader.cpp")

target_include_directories(SharpboyCore PUBLIC "include")