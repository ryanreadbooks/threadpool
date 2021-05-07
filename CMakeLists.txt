cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME simple_threadpool)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

add_executable(${PROJECT_NAME} main.cpp ${sources} ${headers})

target_link_libraries(${PROJECT_NAME} pthread)  # 连接pthread库