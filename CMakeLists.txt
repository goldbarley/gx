cmake_minimum_required(VERSION 3.15.7)
project(gx C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif() # CMAKE_BUILD_TYPE

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT GX_PLATFORM)
	message(FATAL_ERROR "Error: Platform not specified.")
endif() # GX_PLATFORM

string(TOUPPER "${GX_PLATFORM}" GX_PLATFORM)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	set(GX_IS_TOP_LVL ON)
else()
	set(GX_IS_TOP_LVL OFF)
endif() # GX_IS_TOP_LVL

if (GX_IS_TOP_LVL)
	set(GX_DEFAULT_LIB_TYPE "EXEC")
else()
	set(GX_DEFAULT_LIB_TYPE "STATIC")
endif() # GX_IS_TOP_LVL

set(GX_LIB_TYPE "${GX_DEFAULT_LIB_TYPE}" CACHE STRING "Library type.")

if (GX_LIB_TYPE STREQUAL "SHARED")
	add_library(${PROJECT_NAME} SHARED)
	target_compile_definitions(${PROJECT_NAME} PUBLIC GX_LIB_SHARED)
elseif (GX_LIB_TYPE STREQUAL "EXEC")
	add_executable(${PROJECT_NAME}
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test.c)
	target_compile_definitions(${PROJECT_NAME} PUBLIC GX_LIB_EXEC)
else ()
	add_library(${PROJECT_NAME} STATIC)
	target_compile_definitions(${PROJECT_NAME} PUBLIC GX_LIB_STATIC)
endif() # GX_LIB_TYPE

if (GX_PLATFORM STREQUAL "X11")
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		GX_PLATFORM_X11
		VK_USE_PLATFORM_XCB_KHR)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(XCB REQUIRED xcb)
elseif (GX_PLATFORM STREQUAL "WAYLAND")
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		GX_PLATFORM_WAYLAND
		VK_USE_PLATFORM_WAYLAND_KHR)
endif() # GX_PLATFORM

if (WIN32)
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		GX_PLATFORM_WIN32
		VK_USE_PLATFORM_WIN32_KHR)
endif() # WIN32

if (GX_INCLUDE_TESTS)
	target_include_directories(${PROJECT_NAME} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/tests)
	target_sources(${PROJECT_NAME} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/tests/test.c)
endif() # GX_INCLUDE_TESTS

# Core files
target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include)

target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/gx/linux.c)

if (GX_PLATFORM STREQUAL "X11")
	target_link_libraries(${PROJECT_NAME} PRIVATE ${XCB_LIBRARIES})

	target_include_directories(${PROJECT_NAME} PRIVATE ${XCB_INCLUDE_DIRS})

	target_sources(${PROJECT_NAME} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/gxx/util.c
		${CMAKE_CURRENT_SOURCE_DIR}/src/gxx/vulkan.c
		${CMAKE_CURRENT_SOURCE_DIR}/src/gxx/window.c)
endif () # Platform X11

target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
