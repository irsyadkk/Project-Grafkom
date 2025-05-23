cmake_minimum_required(VERSION 3.10)
project(MyImGuiProject)

set(CMAKE_CXX_STANDARD 11)

# Tambahkan definisi agar GLEW tahu kita pakai versi static
add_definitions(-DGLEW_STATIC)

# Include path
include_directories(
    ${PROJECT_SOURCE_DIR}/lib/glfw/include
    ${PROJECT_SOURCE_DIR}/lib/glew/include
    ${PROJECT_SOURCE_DIR}/lib/imgui
    ${PROJECT_SOURCE_DIR}/lib/imgui/backends
)

# Library path
link_directories(
    ${PROJECT_SOURCE_DIR}/lib/glew/lib/Release/x64
)

# Tambah GLFW sebagai subdirectory (pastikan ada CMakeLists.txt di dalam lib/glfw)
add_subdirectory(lib/glfw)

# Tambah executable
add_executable(project
    main.cpp
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
)

# Link library
target_link_libraries(project
    glfw
    glew32s
    opengl32
)
