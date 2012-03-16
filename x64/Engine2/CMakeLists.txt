# Engine2 src
cmake_minimum_required(VERSION 2.6)

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
SET(ARCHIVE_OUTPUT_DIRECTORY ../lib)

SET(Engine2Src 
	../../src/Engine2Main 
	../../src/DRInterface 
	../../src/DRINetwork 
	../../src/DRIvlc 
	../../src/DRVideoConfig 
	../../src/DRIImage 
	../../src/DRTextur
    ../../src/DRText 
	../../src/DRGeometrie 
	../../src/DRGeometrieSphere 
	../../src/DRGeometrieIcoSphere 
	../../src/glew)
	
SET(PA ../bin ../lib )
find_library(SDL_LIB SDL PATHS ${PA})

if(WIN32)
SET(Engine2Libs ${SDL_LIB} opengl32 Core2 Winmm)
elseif(UNIX)
SET(Engine2Libs ${SDL_LIB} GL Core2)
endif(WIN32)
#ENABLE_EXPORTS()
#DEFINE_SYMBOL(BUILD_DLL)
#IF(WIN32)
SET(Engine2Src ${Engine2Src} ../../include/Engine2Main.h
							 ../../include/DRInterface.h 
							 ../../include/DRINetwork.h 
							 ../../include/DRIvlc.h 
							 ../../include/DRVideoConfig.h 
							 ../../include/DRIImage.h 
							 ../../include/DRTextur.h
                             ../../include/DRText.h 
							 ../../include/DRGeometrie.h 
							 ../../include/DRGeometrieSphere.h 
							 ../../include/DRGeometrieIcoSphere.h 
							 ../../include/glew.h 
							 ../../include/wglew.h)
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
