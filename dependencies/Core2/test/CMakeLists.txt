# text (Core test)
cmake_minimum_required(VERSION 2.6)

SET(testSrc main.cpp)
ADD_EXECUTABLE(test ${testSrc})
TARGET_LINK_LIBRARIES(test Core2)
