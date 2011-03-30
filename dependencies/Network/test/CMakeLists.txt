SET(testSrc main.cpp)
ADD_EXECUTABLE(test ${testSrc})
TARGET_LINK_LIBRARIES(test Core2)
