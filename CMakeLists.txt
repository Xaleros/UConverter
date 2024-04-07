cmake_minimum_required(VERSION 3.15)

project(UConverter)

set(UCONVERTER_SOURCES
	include/common.h
	include/u_v3d.h
	include/u_psk.h
	include/u_psa.h
	include/q_mdl.h
	include/q_md2.h
	src/u_v3d.cpp
	src/u_psk.cpp
	src/u_psa.cpp
	src/q_mdl.cpp
	src/q_md2.cpp
	src/main.cpp
)

source_group("include" REGULAR_EXPRESSION "${CMAKE_CURRENT_SOURCE_DIR}/include/.+")
source_group("src" REGULAR_EXPRESSION "${CMAKE_CURRENT_SOURCE_DIR}/src/.+")

include_directories(include)

add_executable(UConverter WIN32 MACOSX_BUNDLE ${UCONVERTER_SOURCES})
