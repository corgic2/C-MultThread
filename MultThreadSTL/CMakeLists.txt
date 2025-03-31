cmake_minimum_required(VERSION 3.10)
project(CustomCaculator)

set(TARGET_NAME CustomCaculator)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

# Qt，开启Qt代码自动生成
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# 设置Qt路径
set(QTDIR "D:/Qt/5.15.2/msvc2019_64")

# 将Qt路径添加到CMAKE_PREFIX_PATH
list(APPEND CMAKE_PREFIX_PATH ${QTDIR})

# 查找Qt5模块
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

# Enable debugging symbols
set(CMAKE_BUILD_TYPE Debug)

# Add compiler flags for debugging
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# 启用C++11
add_compile_options(-std=c++17)
# 解决节数超过对象文件格式限制
add_compile_options(-bigobj)

# 是否x86 还是x64，默认x64
if(CMAKE_CL_64)
    SET(PLATFORM_CPU "x64")
else()
    SET(PLATFORM_CPU "x86")
endif()
MESSAGE(STATUS "PLATFORM_CPU: ${PLATFORM_CPU}")

# 设置输出路径
if(${PLATFORM_CPU} STREQUAL "x64")
    SET(EXECUTE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/x64/)
else()
    SET(EXECUTE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/x86/)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTE_OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXECUTE_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${EXECUTE_OUTPUT_PATH})

# 收集文件 ---------------------------------------------------------
file(GLOB ROOT_SRCS "*.cpp" "*.h")  # 根目录文件（如果有）
file(GLOB MAIN_SRCS "Main/*.cpp" "Main/*.h")
file(GLOB UIMODULE_SRCS "UIModule/*.cpp" "UIModule/*.h" "UIModule/*.ui")

# 创建分组 ---------------------------------------------------------
source_group("Root" FILES ${ROOT_SRCS})
source_group("Main" FILES ${MAIN_SRCS})
source_group("UIModule" FILES ${UIMODULE_SRCS})

# 添加可执行文件 ---------------------------------------------------
add_executable(${TARGET_NAME}
    ${ROOT_SRCS}
    ${MAIN_SRCS}
    ${UIMODULE_SRCS}
)

# 链接 Qt 库 -------------------------------------------------------
target_link_libraries(${TARGET_NAME} Qt5::Core Qt5::Widgets)