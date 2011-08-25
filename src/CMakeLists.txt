# Engine2 src
cmake_minimum_required(VERSION 2.6)

SET(Engine2Src Engine2Main.cpp DRInterface.cpp DRINetwork.cpp DRIvlc DRVideoConfig.cpp DRIImage.cpp DRTextur
               DRText DRGeometrie DRGeometrieSphere)
SET(PA ../dependencies/bin ../dependencies/lib ../dependencies/Core2/lib)
find_library(SDL_LIB SDL PATHS ${PA})

IF(WIN32)
SET(Engine2Libs ${SDL_LIB} opengl32 Core2 Winmm)
ELSE(WIN32)
SET(Engine2Libs ${SDL_LIB} GL Core2)
ENDIF(WIN32)
#ENABLE_EXPORTS()
#DEFINE_SYMBOL(BUILD_DLL)
#IF(WIN32)
SET(Engine2Src ${Engine2Src} ../include/Engine2Main.h ../include/DRInterface.h ../include/DRINetwork.h ../include/DRIvlc.h ../include/DRVideoConfig.h ../include/DRIImage.h ../include/DRTextur.h
                             ../include/DRText.h ../include/DRGeometrie.h ../include/DRGeometrieSphere.h)
#ENDIF(WIN32)
ADD_LIBRARY(Engine2 SHARED ${Engine2Src})
TARGET_LINK_LIBRARIES(Engine2 ${Engine2Libs})

INSTALL(TARGETS Engine2
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

set_target_properties(Engine2 PROPERTIES DEFINE_SYMBOL BUILD_ENGINE_DLL)
SET_TARGET_PROPERTIES(Engine2 PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)
SET_TARGET_PROPERTIES(Engine2 PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../lib)
