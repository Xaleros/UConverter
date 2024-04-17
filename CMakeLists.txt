cmake_minimum_required(VERSION 3.15)

project(UConverter)

set(UCONVERTER_SOURCES
	include/Common.h
	include/UnrealLodMesh.h
	include/UnrealPsk.h
	include/UnrealPsa.h
	include/idTechMdl.h
	include/idTechMd2.h
	src/UnrealLodMesh.cpp
	src/UnrealPsk.cpp
	src/UnrealPsa.cpp
	src/idTechMdl.cpp
	src/idTechmd2.cpp
	src/Main.cpp
	src/Model.cpp
)

source_group("include" REGULAR_EXPRESSION "${CMAKE_CURRENT_SOURCE_DIR}/include/.+")
source_group("src" REGULAR_EXPRESSION "${CMAKE_CURRENT_SOURCE_DIR}/src/.+")

include_directories(include)

add_executable(UConverter WIN32 MACOSX_BUNDLE ${UCONVERTER_SOURCES})
