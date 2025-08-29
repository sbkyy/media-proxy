cmake_minimum_required(VERSION 3.14)
project(mediaProxy CXX)

set(CMAKE_CXX_STANDARD 11)

include(FetchContent)

# 下载 libhv
FetchContent_Declare(
  libhv
  GIT_REPOSITORY https://github.com/ithewei/libhv.git
  GIT_TAG master   # 或指定版本号 v1.3.2
)

FetchContent_MakeAvailable(libhv)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

# 链接 libhv
target_link_libraries(${PROJECT_NAME} PRIVATE hv)
