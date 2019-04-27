cmake_minimum_required(VERSION 3.12)

function(expand_files FILES SOURCE)
    file (GLOB SOURCES ${SOURCE})
    set(${FILES} ${SOURCES} PARENT_SCOPE)
endfunction(expand_files)

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

set(ProjectName TestRandom)
project(${ProjectName})

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR})

expand_files(HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
expand_files(SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")

add_executable(${ProjectName} ${HEADERS} ${SOURCES})

if(MSVC)
    set(DEFAULT_CXX_FLAGS "/DWIN32 /D_WINDOWS /D_MBCS /DLGFX_USE_WIN32 /W4 /WX- /nologo /fp:precise /arch:AVX2 /Zc:wchar_t /TP /Gd")
    if("1800" VERSION_LESS MSVC_VERSION)
        set(DEFAULT_CXX_FLAGS "${DEFAULT_CXX_FLAGS} /EHsc")
    endif()

    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1 /Gy /GR- /GS /Gm-")
    set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /GL /GR- /DNDEBUG")
    target_link_libraries(${ProjectName} "winmm.lib")
elseif(UNIX)
    set(DEFAULT_CXX_FLAGS "-Wall -std=c++11 -march=native")
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O2")

elseif(APPLE)
endif()
