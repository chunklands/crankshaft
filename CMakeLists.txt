cmake_minimum_required(VERSION 3.13)

set(CMAKE_BUILD_TYPE Debug)

project(crankshaft)

set(LIBCRANKSHAFT_TARGET crankshaft)
set(CRANKSHAFT_EXAMPLE_INIT crankshaft-example-init)

file(GLOB_RECURSE LIBCRANKSHAFT_TARGET_SRC src/*.c src/**/*.c)

add_library(
  ${LIBCRANKSHAFT_TARGET}
  SHARED
    ${LIBCRANKSHAFT_TARGET_SRC}
    deps/glfw/deps/glad_gl.c
)

target_compile_definitions(${LIBCRANKSHAFT_TARGET} PUBLIC LOG_USE_COLOR)

# GLIB
find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
add_definitions(${GLIB_CFLAGS_OTHER})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# OpenGL
find_package(OpenGL REQUIRED)

add_subdirectory(deps/glfw)

target_include_directories(
  ${LIBCRANKSHAFT_TARGET}
  PUBLIC
    include
  PRIVATE
    ${GLIB_INCLUDE_DIRS}
    deps/glfw/deps
    deps/cglm/include
    deps/stb
)

target_link_directories(
  ${LIBCRANKSHAFT_TARGET}
  PUBLIC
    ${GLIB_LIBRARY_DIRS}
)

target_link_libraries(
  ${LIBCRANKSHAFT_TARGET}
  ${GLIB_LIBRARIES}
  glfw
  OpenGL::GL
)












# target_include_directories(
#   ${LIBCRANKSHAFT_TARGET}
#   PUBLIC
#     include
#   PRIVATE
#     deps/glib-2.67.0
#     deps/glib-2.67.0/_build/glib
#     deps/glfw-3.3.2/include
#     deps/glfw-3.3.2/deps
#     deps/cglm-0.7.9/include
# )
# 
# find_library(
#   GLIB_LIBRARY
#   NAMES glib-2.0.0
#   PATHS deps/glib-2.67.0/_build/glib
# )
# find_library(
#   GLFW_LIBRARY
#   NAMES glfw.3
#   PATHS deps/glfw-3.3.2/build/src
# )
# find_library(
#   CGLM_LIBRARY
#   NAMES cglm.0.7.9
#   PATHS deps/cglm-0.7.9/build
# )
# target_link_libraries(
#   ${LIBCRANKSHAFT_TARGET}
#   ${GLIB_LIBRARY}
#   ${GLFW_LIBRARY}
#   ${CGLM_LIBRARY}
# )

# add_executable(${CRANKSHAFT_EXAMPLE_INIT}
#   examples/init/main.cxx
# )
# set_property(TARGET ${CRANKSHAFT_EXAMPLE_INIT} PROPERTY CXX_STANDARD 11)
# target_link_libraries(
#   ${CRANKSHAFT_EXAMPLE_INIT}
#   LINK_PUBLIC
#     ${LIBCRANKSHAFT_TARGET}
# )
