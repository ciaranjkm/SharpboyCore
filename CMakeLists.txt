cmake_minimum_required(VERSION 3.31)
project(SharpboyCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(SharpboyCore STATIC
	"src/SharpboyCore.cpp" "include/Utilities/Common.h" "include/Cartridges/Cartridge.h"  "include/Cartridges/SST_Cartridge.h" "src/Cartridges/SST_Cartridge.cpp" "include/Cartridges/CartDefs.h" "include/Utilities/SST_Defs.h" "include/Utilities/NlohmannJSON/json.hpp" "include/Utilities/Testing.h" "src/Testing.cpp" "include/CPU.h" "src/CPU.cpp" "include/Bus.h" "src/Bus.cpp" "src/Timing.cpp")

target_include_directories(SharpboyCore PUBLIC "include")