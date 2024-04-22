cmake_minimum_required(VERSION 3.15)

project(UConverter)

set(UCONVERTER_SOURCES
	include/Common.h
	include/UnrealLodMesh.h
	include/UnrealSkeletalMesh.h
	include/idTechMdl.h
	include/idTechMd2.h
	src/UnrealLodMesh.cpp
	src/UnrealSkeletalMesh.cpp
	src/idTechMdl.cpp
	src/idTechmd2.cpp
	src/Main.cpp
	src/Model.cpp
)

source_group("include" REGULAR_EXPRESSION "${CMAKE_CURRENT_SOURCE_DIR}/include/.+")
source_group("src" REGULAR_EXPRESSION "${CMAKE_CURRENT_SOURCE_DIR}/src/.+")

include_directories(include)

if (WIN32)
    set(SUBSYSTEM_LINKER_OPTIONS "/SUBSYSTEM:CONSOLE")
endif()

add_executable(UConverter WIN32 MACOSX_BUNDLE ${UCONVERTER_SOURCES})
target_link_options(UConverter PRIVATE ${SUBSYSTEM_LINKER_OPTIONS})