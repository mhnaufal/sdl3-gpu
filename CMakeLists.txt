cmake_minimum_required(VERSION 3.30.3)

project(
	bubuk
	VERSION 1.0.0
	DESCRIPTION "bubuk Game Engine | DON'T USE THIS PLEASE"
	LANGUAGES CXX C
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
	add_definitions(-DWIN64 -DWINDOWS)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		add_definitions(-DWIN64)
	endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
	set(ANDROID TRUE)
	add_definitions(-DANDROID)
	set(SDL_VULKAN ON)
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
# # SDL3
# #######################
set(SDL3_DIR "${CMAKE_SOURCE_DIR}/include/SDL3")
set(SDL3_INCLUDE_DIR "${SDL3_DIR}/include")
# set(SDL3_LIBRARY_DIR "${SDL3_DIR}/lib/x64")

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
	find_library(android-lib android)
	set(FMOD_LIBRARY_DIR "${FMOD_DIR}/lib/android")
    set(FMOD_DEBUG_LIBRARY "${FMOD_LIBRARY_DIR}/libfmodL.so")
    set(FMOD_RELEASE_LIBRARY "${FMOD_LIBRARY_DIR}/libfmod.so")
endif()

# #######################
# # PhysX
# #######################
set(PHYSX_DIR "${CMAKE_SOURCE_DIR}/include/physx")
set(PHYSX_INCLUDE_DIR "${PHYSX_DIR}/include")
set(PHYSX_DEBUG_LIBRARY_DIR "${PHYSX_DIR}/lib/debug")
set(PHYSX_RELEASE_LIBRARY_DIR "${PHYSX_DIR}/lib/release")

# #######################
# # NVRHI
# #######################
set(NVRHI_DIR "${CMAKE_SOURCE_DIR}/include/nvrhi")
set(NVRHI_INCLUDE_DIR "${NVRHI_DIR}/include")
set(NVRHI_DEBUG_LIBRARY_DIR "${NVRHI_DIR}/lib/debug")
set(NVRHI_RELEASE_LIBRARY_DIR "${NVRHI_DIR}/lib/release")

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

		include/stb_image.h

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

if(ANDROID)
	add_library(${PROJECT_NAME} SHARED ${CPP_SOURCES})
else()
	add_executable(${PROJECT_NAME} ${HEADER_SOURCES} ${CPP_SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${SDL_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/include/glm
	${FMOD_INCLUDE_DIR}
	${PHYSX_INCLUDE_DIR}
	${NVRHI_INCLUDE_DIR}
)

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
	find_library(android-lib android)
endif()

if(WINDOWS)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_link_libraries(${PROJECT_NAME} PRIVATE 
			SDL3::SDL3 ${SDL3_LIBRARIES} 
			${FMOD_DEBUG_LIBRARY}
			${NVRHI_DEBUG_LIBRARY_DIR}/nvrhi.lib
		)
	else()
		target_link_libraries(${PROJECT_NAME} PRIVATE 
			SDL3::SDL3 ${SDL3_LIBRARIES} 
			${FMOD_RELEASE_LIBRARY}
			${NVRHI_RELEASE_LIBRARY_DIR}/nvrhi.lib
		)
	endif()
elseif(ANDROID)
	target_link_libraries(${PROJECT_NAME} PRIVATE 
		SDL3::SDL3
		# ${FMOD_DEBUG_LIBRARY} 
		${log-lib}
		${android-lib}
	)
endif()

if(WINDOWS)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_link_libraries(${PROJECT_NAME} PRIVATE
			${PHYSX_DEBUG_LIBRARY_DIR}/PhysX_64.lib
			${PHYSX_DEBUG_LIBRARY_DIR}/PhysXExtensions_static_64.lib
			${PHYSX_DEBUG_LIBRARY_DIR}/PhysXPvdSDK_static_64.lib
			${PHYSX_DEBUG_LIBRARY_DIR}/PhysXCommon_64.lib
			${PHYSX_DEBUG_LIBRARY_DIR}/PhysXFoundation_64.lib
		)
	else()
		target_link_libraries(${PROJECT_NAME} PRIVATE
			${PHYSX_RELEASE_LIBRARY_DIR}/PhysX_64.lib
			${PHYSX_RELEASE_LIBRARY_DIR}/PhysXExtensions_static_64.lib
			${PHYSX_RELEASE_LIBRARY_DIR}/PhysXPvdSDK_static_64.lib
			${PHYSX_RELEASE_LIBRARY_DIR}/PhysXCommon_64.lib
			${PHYSX_RELEASE_LIBRARY_DIR}/PhysXFoundation_64.lib
		)
	endif()
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
	# file(GLOB SDL3_DLLS "${SDL3_LIBRARY_DIR}/*.dll")
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD COMMAND
		${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:SDL3::SDL3>
			$<TARGET_FILE_DIR:${PROJECT_NAME}>
			VERBATIM
			COMMENT "Copy all SDL3 DLLs to executable directory\n"
	)

	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_custom_command(
			TARGET ${PROJECT_NAME} POST_BUILD COMMAND
			${CMAKE_COMMAND} -E copy_if_different
				${FMOD_DEBUG_DLL}
				$<TARGET_FILE_DIR:${PROJECT_NAME}>
				VERBATIM
				COMMENT "Copy all FMOD DLLs Debug to executable directory\n"
		)
	else()
		add_custom_command(
			TARGET ${PROJECT_NAME} POST_BUILD COMMAND
			${CMAKE_COMMAND} -E copy_if_different
				${FMOD_RELEASE_DLL}
				$<TARGET_FILE_DIR:${PROJECT_NAME}>
				VERBATIM
				COMMENT "Copy all FMOD DLLs Release to executable directory\n"
		)
	endif()

	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		file(GLOB PHYSX_DLLS "${PHYSX_DEBUG_LIBRARY_DIR}/*.dll")
		add_custom_command(
			TARGET ${PROJECT_NAME} POST_BUILD COMMAND
			${CMAKE_COMMAND} -E copy_if_different
				${PHYSX_DLLS}
				$<TARGET_FILE_DIR:${PROJECT_NAME}>
				VERBATIM
				COMMENT "Copy all PhysX DLLs Debug to executable directory\n"
		)
	else()
		file(GLOB PHYSX_DLLS "${PHYSX_RELEASE_LIBRARY_DIR}/*.dll")
		add_custom_command(
			TARGET ${PROJECT_NAME} POST_BUILD COMMAND
			${CMAKE_COMMAND} -E copy_if_different
				${PHYSX_DLLS}
				$<TARGET_FILE_DIR:${PROJECT_NAME}>
				VERBATIM
				COMMENT "Copy all PhysX DLLs Release to executable directory\n"
		)
	endif()
endif()

file(COPY resource/ DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/resource)
file(COPY shaders/ DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/shaders)
