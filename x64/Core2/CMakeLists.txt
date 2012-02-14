# Core
cmake_minimum_required(VERSION 2.6)

PROJECT(Core2 CXX)

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
SET(ARCHIVE_OUTPUT_DIRECTORY ../lib)

INCLUDE_DIRECTORIES(../../dependencies/Core2/include)
INCLUDE_DIRECTORIES(../../dependencies/Core2/lib)
INCLUDE_DIRECTORIES(../../dependencies/Core2/bin)

# Core src

SET(CoreSrc 
	../../dependencies/Core2/src/Core2Main
	../../dependencies/Core2/src/DRFile 
	../../dependencies/Core2/src/DRFileManager
	../../dependencies/Core2/src/DRHashList
	../../dependencies/Core2/src/DRLogger
    ../../dependencies/Core2/src/DRUtils 
	../../dependencies/Core2/src/DRMatrix 
	../../dependencies/Core2/src/DRVector2 
	../../dependencies/Core2/src/DRDampingSpring
	../../dependencies/Core2/src/DRIni
    ../../dependencies/Core2/src/DRVector3
	../../dependencies/Core2/src/DRObjekt 
	../../dependencies/Core2/src/DRFrustumCulling 
	../../dependencies/Core2/src/DRPlane 
	../../dependencies/Core2/src/DRIndexReferenzHolder)

	
#IF(WIN32)
SET(CoreSrc ${CoreSrc} ../../dependencies/Core2/include/Core2Main.h
					   ../../dependencies/Core2/include/DRFile.h 
					   ../../dependencies/Core2/include/DRFileManager.h 
					   ../../dependencies/Core2/include/DRHash.hpp 
					   ../../dependencies/Core2/include/DRHashList.hpp
                       ../../dependencies/Core2/include/DRLogger.h 
					   ../../dependencies/Core2/include/DRUtils.h 
					   ../../dependencies/Core2/include/DRVector2.h
					   ../../dependencies/Core2/include/DRVector3.h 
					   ../../dependencies/Core2/include/DRMatrix.h
					   ../../dependencies/Core2/include/DRColor.h 
					   ../../dependencies/Core2/include/DRPlane.h 
					   ../../dependencies/Core2/include/DRDampingSpring.h 
					   ../../dependencies/Core2/include/DRIni.h
                       ../../dependencies/Core2/include/DRObjekt.h 
					   ../../dependencies/Core2/include/DRFrustumCulling.h 
					   ../../dependencies/Core2/include/DRIndexReferenzHolder.h)
#ENDIF(WIN32)
ADD_LIBRARY(Core2 SHARED ${CoreSrc})

INSTALL(TARGETS Core2
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

set_target_properties(Core2 PROPERTIES DEFINE_SYMBOL BUILD_CORE_DLL)
SET_TARGET_PROPERTIES(Core2 PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY ../bin)
SET_TARGET_PROPERTIES(Core2 PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
SET_TARGET_PROPERTIES(Core2 PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../lib)
