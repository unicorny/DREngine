#Etext (Engine Test)
cmake_minimum_required(VERSION 2.6)

SET(testSrc main.cpp)
SET(PA ../dependencies/bin ../dependencies/lib)
SET(PA2 ../bin ../lib)
#find_library(CORE_LIB Core2 PATHS ${PA})
#find_library(ENGINE_LIB Engine2 PATHS ${PA2})
find_library(SDL_LIB SDL PATHS ${PA})
SET(Engine2Libs Core2 Engine2 ${SDL_LIB})
ADD_EXECUTABLE(Etest ${testSrc})
TARGET_LINK_LIBRARIES(Etest ${Engine2Libs})

SET_TARGET_PROPERTIES(Etest PROPERTIES EXECUTABLE_OUTPUT_PATH ../import/bin)

