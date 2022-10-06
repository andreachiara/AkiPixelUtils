# This CMakeList.txt currently only supports compilation of
# a program on windows or linux, but only with the dependancies
# for oclPixelGameEngine.h

# NOTE: THIS CMAKELIST WILL NOT INSTALL DEPENDANCIES, IT WILL JUST FIND THEM AND
# COMPILE / LINK THEM RESPECTIVELY, YOU NEED TO INSTALL THEM YOURSELF

# Any problems? submit an issue, or contact the author, "Ben (plane000)#8618" on Discord

# Currently linked / compiled by default is:
# Threads (pthread), OpenGL, GLX, libPNG, X11


cmake_minimum_required(VERSION 3.7)
project(akiPixelUtilsExample)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} CMakeFiles/)

set(Executable output)
set(SourceDir src/) # Change src/ to your source directory
set(OpenGL_GL_PREFERENCE GLVND)

add_executable(${Executable}
#${SourceFiles}
src/Example.cpp
)

# STATIC stuff (Windows)
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(BUILD_FOR_WIN TRUE)
endif()
option(STATIC_BUILD "Build a static binary." ${BUILD_FOR_WIN})

if (STATIC_BUILD)
        set(CMAKE_EXE_LINKER_FLAGS "-static")
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" CONFIG)
        set(BUILD_SHARED_LIBS OFF)
endif()

set(IncludeDir include/) # Change include/ to your include directory
target_include_directories(${Executable} PRIVATE ${IncludeDir})
target_include_directories(${Executable} PRIVATE  include/)
target_include_directories(${Executable} PRIVATE  soloud/include)
file(GLOB_RECURSE SourceFiles
        ${SourceDir}/*.cpp
        #olcExampleProgram.cpp # Remove this
        )

set(THREADS_PREFER_PTHREAD_FLAD ON)
find_package(Threads REQUIRED)

find_package(OpenGL REQUIRED)

find_package(X11 REQUIRED)
find_package(PNG REQUIRED)

#find_package(Freetype REQUIRED)
#include(FindFreetype)

add_subdirectory(json)
add_subdirectory(FreeType2)
add_subdirectory(soloud)


target_include_directories(${Executable} PRIVATE ${PNG_INCLUDE_DIR})
target_include_directories(${Executable} PRIVATE FreeType2/include/)


target_include_directories(${Executable} PRIVATE ${FREETYPE_INCLUDE_DIRS})

target_link_libraries(${Executable} ${FREETYPE_LIBRARIES})
target_link_libraries(${Executable} soloud)

link_libraries(${Executable}
        Threads::Threads
        OpenGL::OpenGL
        OpenGL::GL
        OpenGL::GLX
        )

target_link_libraries(${Executable} stdc++fs)
target_link_libraries(${Executable} GL)
target_link_libraries(${Executable} png)

set(CMAKE_THREAD_LIBS_INIT "-lpthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3 -Wall")

set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(SFML_STATIC_LIBRARIES FALSE)
target_link_libraries(${Executable}
        ${X11_LIBRARIES}
        PNG::PNG
        ${FREETYPE_LIBRARIES}
        freetype
        )