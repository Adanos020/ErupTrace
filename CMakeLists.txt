cmake_minimum_required(VERSION 3.7...3.13)

project(ErupTrace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("include")
file(GLOB_RECURSE SOURCES
    "src/*.cpp" "src/*/*.cpp"
    "include/*.hpp" "include/*/*.hpp")

add_executable(ErupTrace ${SOURCES})

if (UNIX)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	find_package(Vulkan REQUIRED)
	target_link_libraries(ErupTrace PUBLIC
		Threads::Threads
		Vulkan::Vulkan
		shaderc_shared)
elseif (WIN32)
	find_package(glm CONFIG REQUIRED)
	find_package(Vulkan REQUIRED)
	find_library(SHADERC shaderc_combined)
	target_link_libraries(ErupTrace PRIVATE
		glm
		Vulkan::Vulkan
		${SHADERC})
endif()