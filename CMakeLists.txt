# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

get_filename_component(TARGET_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)

project (${TARGET_NAME})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  string(REGEX REPLACE "\\/O(.+/s)" "/Ol1 " CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
  string(REGEX REPLACE "\\/EH(.+/s)" "/EHS-" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
  string(REGEX REPLACE "\\/GR(.*/s)" "/GR-" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
  add_compile_options(
    "$<$<CONFIG:RELEASE>:/FAs>"
    "$<$<CONFIG:RELEASE>:/GL>"
    "$<$<CONFIG:RELEASE>:/GS->"
  )
  if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    add_compile_options("/d2noftol3")
  endif()
  add_link_options("/MAP:out.map")
endif()

# Include sub-projects.
add_subdirectory ("extern")

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

set(RES_DIR ${CMAKE_CURRENT_LIST_DIR}/resources)

set(PRIVATE_HEADER_FILES
  ${SRC_DIR}/asmmath2.h
  ${SRC_DIR}/AsmMath4.h
  ${SRC_DIR}/CoolPrint.h
  ${SRC_DIR}/GenTex.h
  ${SRC_DIR}/GLFont.h
  ${SRC_DIR}/Globals.h
  ${SRC_DIR}/GLTexture.h
  ${SRC_DIR}/noise.h
  ${SRC_DIR}/ParticleSystem.h
  ${SRC_DIR}/PKLogo.h
  ${SRC_DIR}/SphereEffect.h
  ${SRC_DIR}/utils.h
)

set(SRC_FILES
  ${SRC_DIR}/CoolPrint.cpp
  ${SRC_DIR}/GenTex.cpp
  ${SRC_DIR}/GLFont.cpp
  ${SRC_DIR}/GLTexture.cpp
  ${SRC_DIR}/Main.cpp
  ${SRC_DIR}/noise.cpp
  ${SRC_DIR}/ParticleSystem.cpp
  ${SRC_DIR}/SphereEffect.cpp
  ${SRC_DIR}/utils.cpp
)

if (!WIN32)
  list(APPEND PRIVATE_HEADER_FILES ${SRC_DIR}/glf.h)
  list(APPEND SRC_FILES ${SRC_DIR}/glf.cpp)
endif()

set(RES_FILES
  ${RES_DIR}/windows/resource.rc
)

# Add source to this project's executable.
add_executable(${TARGET_NAME} WIN32 ${PRIVATE_HEADER_FILES} ${SRC_FILES} ${RES_FILES})
target_include_directories(${TARGET_NAME} PRIVATE ${RES_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC minifmod OpenGL::GL OpenGL::GLU)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  target_compile_options(${TARGET_NAME} PRIVATE "/WX")
endif()
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
