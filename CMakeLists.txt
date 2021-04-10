cmake_minimum_required(VERSION 3.0.1)
project(example4)
# c++ runtime 버전 설정
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "-pthread")

# openCV 패키지 찾기
find_package(OpenCV REQUIRED)
find_package(libwebsockets REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${LIBWEBSOCKETS_INCLUDE_DIRS})

message(STATUS "success find libwebsockets Package Path \"${PROJECT_DIR}\" - found")

add_executable(${PROJECT_NAME} main.cpp source/webcam.cpp source/socketServer.cpp source/sensorData.cpp source/base64.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} ${LIBWEBSOCKETS_LIBRARIES})