PROJECT(DRImage CXX)

cmake_minimum_required(VERSION 2.6)

# Image src
SET(ImageSrc DRImage DRImageMain)

SET(PA ../../bin)

if(WIN32)
SET(PA2 ../dependencies/freeimage)
find_library(FREEIMAGE_STATIC2_LIB FreeImage PATHS ${PA2} REQUIRED)
SET(ImageLibs ${FREEIMAGE_STATIC2_LIB} DRCore2 DREngine2 )
else(UNIX)
find_library(FREEIMAGE_LIB freeimage REQUIRED)
SET(ImageLibs ${FREEIMAGE_LIB} DRCore2 DREngine2 )
endif(WIN32)
#ENABLE_EXPORTS()
#DEFINE_SYMBOL(BUILD_DLL_IMAGE)
if(WIN32)
SET(ImageSrc ${ImageSrc} ../include/DRImage.h ../include/DRImageMain.h)
endif(WIN32)

ADD_LIBRARY(DRImage SHARED ${ImageSrc})
TARGET_LINK_LIBRARIES(DRImage ${ImageLibs})

set_target_properties(DRImage PROPERTIES DEFINE_SYMBOL BUILD_DLL_IMAGE)
SET_TARGET_PROPERTIES(DRImage PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../lib)
SET_TARGET_PROPERTIES(DRImage PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../../lib)
