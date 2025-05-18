# Engine2
cmake_minimum_required(VERSION 3.29.2)

# download glew for windows
if(MSVC)
    message(STATUS "Downloading GLEW for Windows")
    include(FetchContent)
    FetchContent_Declare(
		glew
		URL     https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
		URL_MD5 e5f71e72b1e81bf85eb903e134b635690695c043
	)
	FetchContent_MakeAvailable(glew)
	message(STATUS "glew source: ${glew_SOURCE_DIR}")
	message(STATUS "glew binary: ${glew_BINARY_DIR}")
endif()

# for linux, assume that sdl will be installed
if(MSVC)
    message(STATUS "Download SDL2 Binary")
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
	message(STATUS "SDL2 source: ${sdl2_SOURCE_DIR}")
	message(STATUS "SDL2 binary: ${sdl2_BINARY_DIR}")
else()
	find_package(SDL2 REQUIRED)
endif()

PROJECT(DREngine2 CXX)

SUBDIRS(src test dependencies/Core2 dependencies/Network dependencies/Image)
#ADD_SUBDIRECTORY(dependencies/Core2 dependencies/Network)
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(lib)
INCLUDE_DIRECTORIES(bin)
INCLUDE_DIRECTORIES(dependencies/include)
INCLUDE_DIRECTORIES(dependencies/bin)

#CMAKE_LIBRARY_PATH()



#ADD_LIBRARY(Core2 SHARED core2main.cpp)
#ADD_EXECUTABLE(test ${testSrc})
