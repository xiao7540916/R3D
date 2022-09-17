cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)

#定义头文件路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/spdlog/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)

#添加编译路径
file(GLOB_RECURSE CORE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/core/*/*.cpp)
file(GLOB_RECURSE GLAD_FILES ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/glad.c)

#链接
set(GLFW_LINK ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/lib-vc2019/glfw3.lib)
set(ASSIMP_LINK ${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/lib/assimp-vc142-mtd.lib)
link_libraries(${OPENGL} ${GLFW_LINK} ${ASSIMP_LINK})

add_executable(R3DEngine ${CORE_FILES} ${GLAD_FILES} main.cpp)