# Network src
SET(NetworkSrc NetworkMain Network Connection Request NetCallback)
SET(PA ../../bin)
SET(PA2 ../../../bin)
SET(PATH_SDL_NET ../dependencies/SDL_net/VisualC/Release)
find_library(SDL_LIB SDL PATHS ${PA})
find_library(SDLNET_LIB SDL_net PATHS ${PA} ${PATH_SDL_NET})
find_library(CORE_LIB Core2 PATHS ${PA})
SET(NetworkLibs ${SDL_LIB} Core2 Engine2 ${SDLNET_LIB})
#ENABLE_EXPORTS()
#DEFINE_SYMBOL(BUILD_DLL)
#IF(WIN32)
SET(NetworkSrc ${NetworkSrc} ../include/Network ../include/NetworkMain
							 ../include/Connection ../include/Request ../include/NetCallback)
#ENDIF(WIN32)

ADD_LIBRARY(Network SHARED ${NetworkSrc})
TARGET_LINK_LIBRARIES(Network ${NetworkLibs})

set_target_properties(Network PROPERTIES DEFINE_SYMBOL BUILD_DLL_NETWORK)
SET_TARGET_PROPERTIES(Network PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../lib)
SET_TARGET_PROPERTIES(Network PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../../bin)
