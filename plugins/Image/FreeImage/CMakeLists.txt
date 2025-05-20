# Image
PROJECT(DRImage CXX)

add_subdirectory(dependencies/FreeImage)

add_library(DRImage SHARED 
    src/DRImage.cpp 
    src/DRImageMain.cpp 
    include/DRImage.h 
    include/DRImageMain.h
    include/export.h
)
target_link_libraries(DRImage FreeImage)


SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../../bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../../lib)

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(../../include)
INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/include")
INCLUDE_DIRECTORIES(dependencies/FreeImage/Source)
set_target_properties(DRImage PROPERTIES DEFINE_SYMBOL BUILD_DLL_IMAGE)