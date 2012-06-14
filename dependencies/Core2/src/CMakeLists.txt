PROJECT(scripts/DRCore2 CXX)

# Core src
cmake_minimum_required(VERSION 2.6)

SET(CoreSrc 
	Core2Main.cpp DRFile.cpp DRFileManager.cpp DRHashList.cpp
	DRLogger.cpp DRUtils.cpp DRMatrix.cpp DRVector2.cpp DRVector2i DRDampingSpring.cpp DRIni.cpp
    DRVector3.cpp DRObjekt.cpp DRFrustumCulling DRPlane DRIndexReferenzHolder DRRandom
	DRResourcePtr DRResourcePtrHolder DRGameStateManager)


	
#IF(WIN32)
SET(CoreSrc ${CoreSrc} ../include/Core2Main.h ../include/DRFile.h ../include/DRFileManager.h ../include/DRHash.hpp ../include/DRHashList.hpp
                       ../include/DRLogger.h ../include/DRUtils.h 
					   ../include/DRVector2.h ../include/DRVector2i.h ../include/DRVector3.h ../include/DRMatrix.h
					   ../include/DRColor.h ../include/DRPlane.h ../include/DRDampingSpring.h ../include/DRIni.h
                       ../include/DRObjekt.h ../include/DRFrustumCulling.h ../include/DRIndexReferenzHolder.h
					   ../include/DRRandom.h ../include/DRGameStateManager.h
					   ../include/DRResourcePtr.h ../include/DRResourcePtrHolder.h ../include/DRIResource.h)
#ENDIF(WIN32)
ADD_LIBRARY(DRCore2 SHARED ${CoreSrc})

INSTALL(TARGETS DRCore2
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

set_target_properties(DRCore2 PROPERTIES DEFINE_SYMBOL BUILD_CORE2_DLL)
SET_TARGET_PROPERTIES(DRCore2 PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../bin)
SET_TARGET_PROPERTIES(DRCore2 PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin)
SET_TARGET_PROPERTIES(DRCore2 PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../../lib)
