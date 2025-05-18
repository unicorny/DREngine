cmake_minimum_required(VERSION 3.29.2)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if (NOT DEFINED DREngine_INSTALL_CMAKEDIR)
   set(DREngine_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/DREngine"
       CACHE STRING "Path to DREngine CMake files")
endif ()

install(TARGETS DREngine EXPORT DREngine_Targets
    RUNTIME COMPONENT DREngine_Runtime
    LIBRARY COMPONENT DREngine_Runtime
    NAMELINK_COMPONENT DREngine_Development
    ARCHIVE COMPONENT DREngine_Development
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(DIRECTORY "${DREngine_SOURCE_DIR}/include/" "${DREngine_BINARY_DIR}/include/"
    TYPE INCLUDE
    COMPONENT DREngine_Development)

if (BUILD_SHARED_LIBS)
    set(type shared)
else ()
    set(type static)
endif ()

install(EXPORT DREngine_Targets
    DESTINATION "${DREngine_INSTALL_CMAKEDIR}"
    NAMESPACE DREngine::
    FILE DREngine-${type}-targets.cmake
    COMPONENT DREngine_Development)

write_basic_package_version_file(
    DREngineConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/DREngineConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DREngineConfigVersion.cmake"
    DESTINATION "${DREngine_INSTALL_CMAKEDIR}"
    COMPONENT DREngine_Development)