# Image
PROJECT(DRImage CXX)

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../../bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../lib)

SUBDIRS(src)
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(../include)
INCLUDE_DIRECTORIES(../bin)
INCLUDE_DIRECTORIES(../../include)
INCLUDE_DIRECTORIES(../../bin)
if(WIN32)
INCLUDE_DIRECTORIES(dependencies/freeimage)
endif(WIN32)

cmake_minimum_required(VERSION 2.6)


#ADD_LIBRARY(Core2 SHARED core2main.cpp)
#ADD_EXECUTABLE(test ${testSrc})
