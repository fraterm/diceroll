cmake_minimum_required(VERSION 3.22.1)
# project name and version
project(diceroll VERSION 0.1)

# specifying C++ standard 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the cpp source files
set(SOURCE_FILES 
	diceroll.cpp
	)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
