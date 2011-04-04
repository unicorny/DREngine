# Core src
cmake_minimum_required(VERSION 2.6)

SET(CoreSrc 
	Core2Main.cpp DRFile.cpp DRFileManager.cpp DRHashList.cpp
	DRLogger.cpp DRUtils.cpp DRMatrix.cpp DRVector2.cpp DRDampingSpring.cpp DRIni.cpp)

	
#IF(WIN32)
SET(CoreSrc ${CoreSrc} ../include/Core2Main.h ../include/DRFile.h ../include/DRFileManager.h ../include/DRHash.hpp ../include/DRHashList.hpp
                       ../include/DRLogger.h ../include/DRUtils.h ../include/DRVector2.h ../include/DRVector3.h ../include/DRMatrix.h
					   ../include/DRColor.h ../include/DRPlane.h ../include/DRDampingSpring.h ../include/DRIni.h)
#ENDIF(WIN32)
ADD_LIBRARY(Core2 SHARED ${CoreSrc})

INSTALL(TARGETS Core2
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

set_target_properties(Core2 PROPERTIES DEFINE_SYMBOL BUILD_CORE_DLL)
SET_TARGET_PROPERTIES(Core2 PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY ../../bin)
SET_TARGET_PROPERTIES(Core2 PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin)
SET_TARGET_PROPERTIES(Core2 PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../../lib)
