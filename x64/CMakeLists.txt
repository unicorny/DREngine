# Engine2
cmake_minimum_required(VERSION 2.6)

PROJECT(Engine2 CXX)

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)


SUBDIRS(Core2 Engine2 Image)
#ADD_SUBDIRECTORY(dependencies/Core2 dependencies/Network)
INCLUDE_DIRECTORIES(../include)
INCLUDE_DIRECTORIES(../lib)
INCLUDE_DIRECTORIES(../bin)
INCLUDE_DIRECTORIES(../dependencies/include)
INCLUDE_DIRECTORIES(../dependencies/bin)

#CMAKE_LIBRARY_PATH()



#ADD_LIBRARY(Core2 SHARED core2main.cpp)
#ADD_EXECUTABLE(test ${testSrc})
