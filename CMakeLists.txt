cmake_minimum_required(VERSION 3.6)

set(CMAKE_BUILD_TYPE Debug)

project(crankshaft)

set(LIBCRANKSHAFT_TARGET crankshaft)
set(CRANKSHAFT_EXAMPLE_INIT crankshaft-example-init)

file(GLOB_RECURSE LIBCRANKSHAFT_TARGET_SRC src/*.c src/**/*.c)

add_library(
  ${LIBCRANKSHAFT_TARGET}
  SHARED
    deps/glfw-3.3.2/deps/glad_gl.c
    ${LIBCRANKSHAFT_TARGET_SRC}
)

target_include_directories(
  ${LIBCRANKSHAFT_TARGET}
  PUBLIC
    include
  PRIVATE
    deps/glib-2.67.0
    deps/glib-2.67.0/_build/glib
    deps/glfw-3.3.2/include
    deps/glfw-3.3.2/deps
)

find_library(
  GLIB_LIBRARY
  NAMES glib-2.0.0
  PATHS deps/glib-2.67.0/_build/glib
)
find_library(
  GLFW_LIBRARY
  NAMES glfw.3
  PATHS deps/glfw-3.3.2/build/src
)
target_link_libraries(
  ${LIBCRANKSHAFT_TARGET}
  ${GLIB_LIBRARY}
  ${GLFW_LIBRARY}
)



add_executable(${CRANKSHAFT_EXAMPLE_INIT}
  examples/init/main.cxx
)
set_property(TARGET ${CRANKSHAFT_EXAMPLE_INIT} PROPERTY CXX_STANDARD 11)
target_link_libraries(
  ${CRANKSHAFT_EXAMPLE_INIT}
  LINK_PUBLIC
    ${LIBCRANKSHAFT_TARGET}
)
