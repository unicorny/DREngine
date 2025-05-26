# Engine2
cmake_minimum_required(VERSION 3.29.2)

project(DREngine VERSION 2.1)
enable_language(CXX)

# Option zum Ein-/Ausschalten von Plugins
option(BUILD_PLUGINS "Build plugin modules" ON)
option(DRCore2_SHARED_LIBS "Build DRCore2 as a shared library" ON)
option(DREngine_SHARED_LIBS "Build DREngine as a shared library" ON)

# Basis-Ausgabeordner setzen
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")       # Für .exe und .dll
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")       # Für .so und .dll (wenn als SHARED lib gebaut)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")       # Für .lib und .a (statische Libs)

find_package(OpenGL REQUIRED)

# download glew for windows
if(MSVC)
    message(STATUS "Make GLEW Available for Windows")
    include(FetchContent)
    FetchContent_Declare(
		glew
		URL     https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
		URL_MD5 1feddfe8696c192fa46a0df8eac7d4bf
	)
	FetchContent_MakeAvailable(glew)
    add_library(glew STATIC IMPORTED GLOBAL)
    set(glew_LIB_PATH "${glew_SOURCE_DIR}/lib/Release")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(glew_LIB_PATH "${glew_LIB_PATH}/x64")
    else()
        set(glew_LIB_PATH "${glew_LIB_PATH}/Win32")
    endif()
    if(DREngine_SHARED_LIBS)
        set(glew_LIB_FULL "${glew_LIB_PATH}/glew32.lib")
    else()
        set(glew_LIB_FULL "${glew_LIB_PATH}/glew32s.lib")
    endif()
    set_target_properties(glew PROPERTIES
        IMPORTED_LOCATION "${glew_LIB_FULL}"
        INTERFACE_INCLUDE_DIRECTORIES "${glew_SOURCE_DIR}/include"
    )
endif()

# for linux, assume that sdl will be installed
if(MSVC)
    message(STATUS "Make SDL2 Available for Windows")
	# Fetch Content Module
	include(FetchContent)

	# Visual C++
	## SDL2
	FetchContent_Declare(
		sdl2
		URL     https://www.libsdl.org/release/SDL2-devel-2.30.2-VC.zip
		URL_MD5 375d933452c6cf623aedf9a4acc58c33
	)
	FetchContent_MakeAvailable(sdl2)
    add_library(SDL2 STATIC IMPORTED GLOBAL)
    set(SDL2_LIB_PATH "${sdl2_SOURCE_DIR}/lib")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SDL2_LIB_PATH "${SDL2_LIB_PATH}/x64")
    else()
        set(SDL2_LIB_PATH "${SDL2_LIB_PATH}/x86")
    endif()
    set_target_properties(SDL2 PROPERTIES
        IMPORTED_LOCATION "${SDL2_LIB_PATH}/SDL2.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${sdl2_SOURCE_DIR}/include"
    )
else()
	find_package(SDL2 REQUIRED)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

if(MSVC)
    #build with multicore support when using visual studio
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

	# Suppress warning C4003 for Visual Studio Compiler
	add_compile_options(/wd4003)
	# Ensure correct architecture is defined
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_compile_definitions(_AMD64_)
    else()
        add_compile_definitions(_X86_)
    endif()
endif()

if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
    NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

if (DEFINED DREngine_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${DREngine_SHARED_LIBS}")
endif ()

include_directories(
	"dependencies/DRCore2/include"
    "${CMAKE_BINARY_DIR}/dependencies/DRCore2/include"
	"dependencies/magic_enum/include"
	"${sdl2_SOURCE_DIR}/include"
    "${glew_SOURCE_DIR}/include"
    "include"
)

FILE(GLOB SRC "src/*.cpp")
FILE(GLOB HEADERS "include/DREngine/*.h")

SET(DREngine_SOURCES ${SRC} ${HEADERS})

add_subdirectory(dependencies/DRCore2)
add_library(DREngine ${DREngine_SOURCES})
add_library(DREngine::DREngine ALIAS DREngine)

if(BUILD_SHARED_LIBS)
    target_link_libraries(DREngine DRCore2 SDL2 glew OpenGL::GL)
    message(STATUS "Building DREngine as a shared library")
else()
    add_definitions(-DGLEW_STATIC)
    message(STATUS "Building DREngine as a static library")
endif()


set_target_properties(DREngine PROPERTIES
                      VERSION ${DREngine_VERSION}
                      SOVERSION ${DREngine_VERSION_MAJOR})
target_include_directories(
    DREngine PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
target_compile_features(DREngine PUBLIC cxx_std_17)

include(GenerateExportHeader)
generate_export_header(DREngine EXPORT_FILE_NAME include/DREngine/export.h)
target_compile_definitions(
    DREngine PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:DREngine_STATIC_DEFINE>")
target_include_directories(
    DREngine PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)
option(DREngine_INCLUDE_PACKAGING "Include packaging rules for DREngine" "${is_top_level}")
if (DREngine_INCLUDE_PACKAGING)
    #add_subdirectory(packaging)
endif ()

# Plugin-Support
if(BUILD_PLUGINS)
    add_subdirectory(plugins)
endif()