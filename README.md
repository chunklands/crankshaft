# CRANKSHAFT

![crankshaft logo](./logo.png)

## install glib

dependencies: meson, ninja

install:

1. download glib https://github.com/GNOME/glib/releases/tag/2.67.0 to `/deps/glib-2.67.0`
1. `cd deps/glib-2.67.0`
1. `meson _build`
1. `ninja -C _build`

## install glfw

dependencies: cmake

install:

1. download glfw https://github.com/glfw/glfw/archive/3.3.2.tar.gz to `/deps/glfw-3.3.2`
1. `cd glfw-3.3.2`
1. `mkdir build && cd build`
1. `cmake -DBUILD_SHARED_LIBS=ON .. && make`