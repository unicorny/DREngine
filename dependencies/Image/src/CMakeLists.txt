# Image src
SET(ImageSrc Image ImageMain)
SET(PA ../../bin)
SET(PA2 ../dependencies/freeimage)
find_library(FREEIMAGE_LIB freeimage PATHS ${PA2})
find_library(CORE_LIB Core2 PATHS ${PA})
SET(ImageLibs Core2 Engine2 ${FREEIMAGE_LIB})
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
