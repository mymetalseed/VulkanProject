cmake_minimum_required(VERSION 3.12)
project(vulkanLearning)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIRSRCS)

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})
include_directories(${Vulkan_INCLUDE_DIR}/../Third-Party/Include)
include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/glm/include
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/GLFW3/include
)

link_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/GLFW3/lib
)

add_subdirectory(vulkanWrapper)

add_executable(vulkanLearning ${DIRSRCS})
target_link_libraries(vulkanLearning vulkanLib ${Vulkan_LIBRARY} glfw3.lib)