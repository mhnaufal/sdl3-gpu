cmake_minimum_required(VERSION 3.26)

project(
	bubuk
	VERSION 1.0.0
	DESCRIPTION "bubuk Game Engine | DON'T USE THIS PLEASE"
	LANGUAGES CXX
)

option(USE_IMGUI "Build with ImGUI support" OFF)

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

		include/SDL3/include/SDL3/

		include/glm/
)

if(USE_IMGUI)
	add_compile_definitions(USE_IMGUI)

	list(APPEND HEADER_SOURCES
		include/imgui/
	)

	list(APPEND CPP_SOURCES
		include/imgui/imgui_draw.cpp
		include/imgui/imgui_impl_sdl3.cpp
		include/imgui/imgui_impl_sdlgpu3.cpp
		include/imgui/imgui_impl_sdlrenderer3.cpp
		include/imgui/imgui_stdlib.cpp
		include/imgui/imgui_tables.cpp
		include/imgui/imgui_widgets.cpp
		include/imgui/imgui.cpp
		include/imgui/imgui_widgets.cpp
	)

	if(WIN32)
		list(APPEND CPP_SOURCES
			include/imgui/imgui_impl_win32.cpp
		)
	endif()

	if(ANDROID)
		list(APPEND CPP_SOURCES
			include/imgui/imgui_impl_android.cpp
		)
	endif()
endif()

add_subdirectory(include/SDL3 EXCLUDE_FROM_ALL)
add_subdirectory(include/glm EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} ${HEADER_SOURCES} ${CPP_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/SDL3/include/SDL3/)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/FMOD)

if(USE_IMGUI)
	target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 ${SDL3_LIBRARIES} ${FMOD_DEBUG_LIBRARY})

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
