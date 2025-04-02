cmake_minimum_required(VERSION 3.26)

project(
	bubuk
	VERSION 1.0.0
	DESCRIPTION "bubuk Game Engine | DON'T USE THIS PLEASE"
	LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE BOOL "" FORCE)
set(CMAKE_CXX_STANDARD 17)

# #######################
# # GLM
# #######################
set(glm_DIR ${CMAKE_SOURCE_DIR}/include/glm)

# #######################
# # FMOD
# #######################
set(FMOD_DIR "${CMAKE_SOURCE_DIR}/include/FMOD")
set(FMOD_INCLUDE_DIR "${FMOD_DIR}")
set(FMOD_LIBRARY_DIR "${FMOD_DIR}")

if(WIN32)
	set(FMOD_LIBRARY "${FMOD_LIBRARY_DIR}/fmod_vc.lib")
    set(FMOD_DEBUG_LIBRARY "${FMOD_LIBRARY_DIR}/fmodL_vc.lib")
endif()

# #######################
# # Noesis
# #######################
set(Noesis_DIR "${CMAKE_SOURCE_DIR}/include/Noesis")
set(Noesis_INCLUDE_DIR "${Noesis_DIR}")
set(Noesis_LIBRARY_DIR "${Noesis_DIR}")

if(MSVC)
	set(CMAKE_CXX_FLAGS "/Wall /W4 /sdl /EHsc")
endif()

if(CMAKE_GNU_COMPILER_ID)
	set(CMAKE_CXX_FLAGS "-Wextra -Wall -Wpedantic")
endif()

set(CPP_SOURCES
		main.cpp
)
set(HEADER_SOURCES
		include/main_audio.h
		include/main_global.h
		include/main_gui.h
		include/main_helper.h
		include/main_render.h
		include/SDL3/include/SDL3
		include/glm
)

# #######################
# # SDL3 & GLM (cmake)
# #######################
add_subdirectory(include/SDL3 EXCLUDE_FROM_ALL)
add_subdirectory(include/glm EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} ${HEADER_SOURCES} ${CPP_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/SDL3/include/SDL3/)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/FMOD)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/Noesis)

target_link_libraries(${PROJECT_NAME} PRIVATE 
	SDL3::SDL3 ${SDL3_LIBRARIES} 
	${FMOD_DEBUG_LIBRARY} 
	${Noesis_DIR}/Noesis.lib ${Noesis_DIR}/NoesisApp.lib
)

# if (WIN32)
# 	target_link_libraries(
# 		${PROJECT_NAME} PRIVATE SDL3main
# 	)
# endif()

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

if(WIN32)
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:SDL3::SDL3>
			$<TARGET_FILE_DIR:${PROJECT_NAME}>
			VERBATIM
	)

	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			"${FMOD_DIR}/fmodL.dll"
			$<TARGET_FILE_DIR:${PROJECT_NAME}>
			VERBATIM
	)
endif()

file(COPY resource/ DESTINATION ${CMAKE_BINARY_DIR}/resource)

file(COPY resource/MainWindow.xaml DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

file(GLOB NOESIS_DLLS ${Noesis_DIR}/Noesis.dll ${Noesis_DIR}/NoesisApp.dll)
file(COPY ${NOESIS_DLLS} DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})