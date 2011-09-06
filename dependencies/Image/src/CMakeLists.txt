PROJECT(Image CXX)

cmake_minimum_required(VERSION 2.6)

# Image src
SET(ImageSrc Image ImageMain)

SET(PA ../../bin)
SET(PA2 ../dependencies/freeimage)
find_library(FREEIMAGE_STATIC1_LIB FreeImage PATHS ${PA2} REQUIRED)
SET(ImageLibs ${FREEIMAGE_STATIC1_LIB} Core2 Engine2 )
#ENABLE_EXPORTS()
#DEFINE_SYMBOL(BUILD_DLL_IMAGE)
#IF(WIN32)
SET(ImageSrc ${ImageSrc} ../include/Image.h ../include/ImageMain.h)
#ENDIF(WIN32)

ADD_LIBRARY(Image SHARED ${ImageSrc})
TARGET_LINK_LIBRARIES(Image ${ImageLibs})

set_target_properties(Image PROPERTIES DEFINE_SYMBOL BUILD_DLL_IMAGE)
SET_TARGET_PROPERTIES(Image PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../lib)
SET_TARGET_PROPERTIES(Image PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../../lib)
