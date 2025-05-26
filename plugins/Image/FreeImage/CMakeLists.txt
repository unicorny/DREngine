# Image
PROJECT(DRImage CXX)

set(BUILD_SHARED_LIBS_ORIG ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS off)
add_subdirectory(dependencies/FreeImage)

add_library(DRImage SHARED 
    src/DRImage.cpp 
    src/DRImageMain.cpp 
    include/DRImage.h 
    include/DRImageMain.h    
)
target_link_libraries(DRImage FreeImage DRCore2 DREngine)

include(GenerateExportHeader)
generate_export_header(DRImage EXPORT_FILE_NAME include/DRImage/export.h)
target_compile_definitions(
    DRImage PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:DRIMAGE_STATIC_DEFINE>")
target_include_directories(
    DRImage PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(../../include)
INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/include")
INCLUDE_DIRECTORIES(dependencies/FreeImage/Source)

set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_ORIG})