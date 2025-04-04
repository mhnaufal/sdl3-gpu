cmake_minimum_required(VERSION 3.26)

project(
	bubuk
	VERSION 1.0.0
	DESCRIPTION "bubuk Game Engine | DON'T USE THIS PLEASE"
	LANGUAGES CXX
)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Your Build Type [Release/Debug]" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE BOOL "" FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)

option(USE_IMGUI "Build with ImGUI support" OFF)

# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(WINDOWS TRUE)
	add_definitions(-DWIN32 -D_WINDOWS)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		add_definitions(-D_WIN64)
	endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
	set(ANDROID TRUE)
	add_definitions(-DANDROID)
endif()

# Arch detection
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	add_definitions(-DARCH_64BIT)
else()
	add_definitions(-DARCH_32BIT)
endif()

# Configuration-specific settings
string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)
add_definitions(-DBUILD_TYPE_${BUILD_TYPE_UPPER})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG -D_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DNDEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    if(WINDOWS)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG:FULL /OPT:REF /OPT:ICF")
    endif()
endif()

# #######################
# # GLM
# #######################
set(glm_DIR ${CMAKE_SOURCE_DIR}/include/glm)

# #######################
# # FMOD
# #######################
set(FMOD_DIR "${CMAKE_SOURCE_DIR}/include/FMOD")
set(FMOD_INCLUDE_DIR "${FMOD_DIR}")

if(WINDOWS)
	set(FMOD_LIBRARY_DIR "${FMOD_DIR}/lib/win")
	set(FMOD_LIBRARY "${FMOD_LIBRARY_DIR}/fmod_vc.lib")
    set(FMOD_DEBUG_LIBRARY "${FMOD_LIBRARY_DIR}/fmodL_vc.lib")
    set(FMOD_RELEASE_LIBRARY "${FMOD_LIBRARY_DIR}/fmod_vc.lib")
    set(FMOD_DEBUG_DLL "${FMOD_LIBRARY_DIR}/fmodL.dll")
    set(FMOD_RELEASE_DLL "${FMOD_LIBRARY_DIR}/fmod.dll")
elseif(ANDROID)
	find_library(log-lib log)
	set(FMOD_LIBRARY_DIR "${FMOD_DIR}/lib/android")
	set(FMOD_LIBRARY "${FMOD_LIBRARY_DIR}/fmod_vc.lib")
    set(FMOD_DEBUG_LIBRARY "${FMOD_LIBRARY_DIR}/fmodL_vc.lib")
    set(FMOD_RELEASE_LIBRARY "${FMOD_LIBRARY_DIR}/fmod_vc.lib")
endif()

if(MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CMAKE_CXX_FLAGS "/Wall /W4 /sdl /EHsc /std:c++17 /MTd /analyze:quiet /O2")
	else()
		set(CMAKE_CXX_FLAGS "/Wall /W4 /sdl /EHsc /Qpar /std:c++17 /MT /analyze:quiet /Od")
	endif()
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

	if(WINDOWS)
		list(APPEND CPP_SOURCES
			include/imgui/imgui_impl_win32.cpp
		)
	elseif(ANDROID)
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

if(WINDOWS)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_directories(${PROJECT_NAME} PUBLIC ${FMOD_DIR}/core/lib/x64/debug)
    else()
        target_link_directories(${PROJECT_NAME} PUBLIC ${FMOD_DIR}/core/lib/x64/release)
    endif()
elseif(ANDROID)
	find_library(log-lib log)
endif()

if(WINDOWS)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 ${SDL3_LIBRARIES} ${FMOD_DEBUG_LIBRARY})
	else()
		target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 ${SDL3_LIBRARIES} ${FMOD_RELEASE_LIBRARY})
	endif()
elseif(ANDROID)
	target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 ${SDL3_LIBRARIES} ${FMOD_DEBUG_LIBRARY} ${log-lib})
endif()

# #######################
# # spdlog
# #######################
# find_package(spdlog REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog ws2_32)
# target_include_directories(${PROJECT_NAME} PRIVATE ${spdlog_INCLUDE_DIRS})
# add_compile_definitions(SPDLOG_FMT_EXTERNAL)

if(MSVC)
	target_link_libraries(${PROJECT_NAME} PRIVATE wsock32 ws2_32)
endif()

# Installation configuration
if(WINDOWS)
    install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin/Release
    )
    install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Debug
        RUNTIME DESTINATION bin/Debug
    )
endif()

if(WINDOWS)
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:SDL3::SDL3>
			$<TARGET_FILE_DIR:${PROJECT_NAME}>
			VERBATIM
	)

	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_custom_command(
			TARGET ${PROJECT_NAME} POST_BUILD COMMAND
			${CMAKE_COMMAND} -E copy_if_different
				${FMOD_DEBUG_DLL}
				$<TARGET_FILE_DIR:${PROJECT_NAME}>
				VERBATIM
		)
	else()
		add_custom_command(
			TARGET ${PROJECT_NAME} POST_BUILD COMMAND
			${CMAKE_COMMAND} -E copy_if_different
				${FMOD_RELEASE_DLL}
				$<TARGET_FILE_DIR:${PROJECT_NAME}>
				VERBATIM
		)
	endif()
endif()

file(COPY resource/ DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/resource)
file(COPY shaders/ DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/shaders)
