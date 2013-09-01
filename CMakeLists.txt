
cmake_minimum_required (VERSION 2.8.8)

project(ibus-engine-varnam)
message ("Generating project ${PROJECT_NAME}")

set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install dir prefix")
set(IBUS_VER 1.3)
set(VARNAM_VER 3.0.0)

##################################
# Finding required packages
#################################

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(IBUS REQUIRED ibus-1.0>=${IBUS_VER})
PKG_CHECK_MODULES(VARNAM REQUIRED varnamstatic>=${VARNAM_VER})

IF(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "-Wall")
    set(CMAKE_C_FLAGS_DEBUG "-g")
    set(CMAKE_C_FLAGS_RELEASE "-O2")
ENDIF()

IF(MSVC)
        set(CMAKE_C_FLAGS "/Wall /wd\"4255;4100\"")
        # We don't want to use MSVC's more secure versions of functions. So suppressing the warnings
        add_definitions(-DWIN32 -D_CRT_SECURE_NO_WARNINGS)
ENDIF()

add_subdirectory(src)

