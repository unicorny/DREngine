# Image
PROJECT(Image CXX)

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib)

INCLUDE_DIRECTORIES(../../dependencies/Image/include)
INCLUDE_DIRECTORIES(../../dependencies/Image/dependencies/freeimage)

cmake_minimum_required(VERSION 2.6)

# Image src
SET(ImageSrc 
	../../dependencies/Image/src/Image 
	../../dependencies/Image/src/ImageMain)

SET(PA ../bin)
SET(PA2 ../bin ../lib)
find_library(FREEIMAGE_STATIC2_LIB FreeImage PATHS ${PA2} REQUIRED)
SET(ImageLibs ${FREEIMAGE_STATIC2_LIB} Core2 Engine2 )
#ENABLE_EXPORTS()
#DEFINE_SYMBOL(BUILD_DLL_IMAGE)
#IF(WIN32)
SET(ImageSrc ${ImageSrc} 
	../../dependencies/Image/include/Image.h 
	../../dependencies/Image/include/ImageMain.h)
#ENDIF(WIN32)

ADD_LIBRARY(Image SHARED ${ImageSrc})
TARGET_LINK_LIBRARIES(Image ${ImageLibs})

set_target_properties(Image PROPERTIES DEFINE_SYMBOL BUILD_DLL_IMAGE)
SET_TARGET_PROPERTIES(Image PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)
SET_TARGET_PROPERTIES(Image PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../lib)

