cmake_minimum_required(VERSION 3.26)

project(
	pikumarazter
	VERSION 1.0.0
	DESCRIPTION "pikumarazter"
	LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE BOOL "" FORCE)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
	set(CMAKE_CXX_FLAGS "/Wall /std:c++17 /W4 /sdl /EHsc")
endif()

if(CMAKE_GNU_COMPILER_ID)
	set(CMAKE_CXX_FLAGS "-Wextra -Wall -Wpedantic")
endif()

set(CPP_SOURCES
		main.cpp
)
set(HEADER_SOURCES
		include/main.h
		include/SDL3/include/SDL3/
)

add_subdirectory(include/SDL3 EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} ${HEADER_SOURCES} ${CPP_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/SDL3/include/SDL3/)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 ${SDL3_LIBRARIES})

# if (WIN32)
# 	target_link_libraries(
# 		${PROJECT_NAME} PRIVATE SDL3main
# 	)
# endif()

# #######################
# # Flecs
# #######################
# find_package(flecs REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE flecs::flecs_static)
# target_include_directories(${PROJECT_NAME} PRIVATE ${flecs_INCLUDE_DIRS})

# #######################
# # spdlog
# #######################
# find_package(spdlog REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog ws2_32)
# target_include_directories(${PROJECT_NAME} PRIVATE ${spdlog_INCLUDE_DIRS})
# add_compile_definitions(SPDLOG_FMT_EXTERNAL)

#######################
# MSVC
#######################
if(MSVC)
	target_link_libraries(${PROJECT_NAME} PRIVATE wsock32 ws2_32)
endif()

add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD COMMAND
	${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:SDL3::SDL3>
		$<TARGET_FILE_DIR:${PROJECT_NAME}>
		VERBATIM
)
