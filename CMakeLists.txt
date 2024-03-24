cmake_minimum_required(VERSION 3.27.0...3.28 FATAL_ERROR)

project(flattervogel
        VERSION 0.0.1
        DESCRIPTION "Flattervogel - 'Next-Get AI' spielt Flappybird."
        LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES "src/*.hpp" "src/*.cpp")

add_subdirectory("lib/raylib/")

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        UNICODE _UNICODE
        _USE_MATH_DEFINES
        BUILD_SHARED_LIBS=ON
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        "src/"
        "lib/raylib/src/"
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
        raylib
)

# Systems
if (WIN32) # Windows
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PS_PLATFORM_WINDOWS)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE opengl32)
endif ()

if (UNIX AND NOT APPLE) # Linux
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
            PS_PLATFORM_LINUX
            USE_WAYLAND=ON
    )
endif ()

if (APPLE) # Apple...
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PS_PLATFORM_MACOSX)
endif ()

# Compiler
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    # Windows Subsystem -> WinMain Entry-Point
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PS_TOOLSET_MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})

elseif (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    if (WIN32)
        # Windows Subsystem
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,windows")
    endif ()

    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PS_TOOLSET_CLANG)

    # Copy compile_commands.json to ${CMAKE_SOURCE_DIR}
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR})

elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    if (WIN32)
        # Windows Subsystem
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
    endif ()

    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PS_TOOLSET_GNU)

endif ()

# Build Type
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PS_DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PS_RELEASE)
else ()
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PS_DEPLOY)
endif ()
