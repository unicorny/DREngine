# Engine2 src
cmake_minimum_required(VERSION 2.6)

SET(Engine2Src Engine2Main DRInterface DRINetwork DRIvlc DRVideoConfig 
			   DRIImage DRTexture DRSaveTexture DRTextureManager				
               DRText DRGeometrie DRGeometrieSphere DRGeometrieIcoSphere
			   DREngineLogger DRThread)

SET(PA ../dependencies/bin ../dependencies/lib ../dependencies/Core2/lib)
find_library(SDL2_LIB SDL2 PATHS ${PA})

if(WIN32)
SET(Engine2Libs ${SDL2_LIB} opengl32 DRCore2 Winmm)
elseif(UNIX)
#SET(Engine2Libs SDL2 GL GLEW DRCore2)
endif(WIN32)
#ENABLE_EXPORTS()
#DEFINE_SYMBOL(BUILD_DLL)
if(WIN32)
SET(Engine2Src ${Engine2Src} glew ../include/Engine2Main.h ../include/DRInterface.h ../include/DRINetwork.h ../include/DRIvlc.h ../include/DRVideoConfig.h 
							 ../include/DRIImage.h ../include/DRTexture.h ../include/DRSaveTexture.h ../include/DRTextureManager.h
                             ../include/DRText.h ../include/DRGeometrie.h ../include/DRGeometrieSphere.h ../include/DRGeometrieIcoSphere.h ../include/glew.h ../include/wglew.h
							 ../include/DREngineLogger.h ../include/DRThread.h)
endif(WIN32)
ADD_LIBRARY(DREngine2 SHARED ${Engine2Src})
TARGET_LINK_LIBRARIES(DREngine2 ${Engine2Libs})

INSTALL(TARGETS DREngine2
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

set_target_properties(DREngine2 PROPERTIES DEFINE_SYMBOL BUILD_ENGINE2_DLL)
SET_TARGET_PROPERTIES(DREngine2 PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)
SET_TARGET_PROPERTIES(DREngine2 PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../lib)
