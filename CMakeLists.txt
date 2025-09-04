cmake_minimum_required(VERSION 3.20)
project(NBVS CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Options
option(NBVS_BUILD_SANITIZERS "Enable address/undefined sanitizers" OFF)



# Dependencies: GLFW (windowing)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.9
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Dependencies: Dear ImGui (core + backends)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.91.5-docking
)
FetchContent_Populate(imgui)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)

add_library(imgui STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
  ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
  ${IMGUI_DIR}
  ${IMGUI_BACKENDS_DIR}
)
target_link_libraries(imgui PUBLIC glfw)

# OpenGL and loader (GLEW on macOS via Homebrew)
find_package(OpenGL REQUIRED)

# ImGui's OpenGL3 backend needs a loader (GLAD/GLEW/etc). To keep the repo lean, use system GLEW.
find_package(GLEW)
if(NOT GLEW_FOUND)
  message(FATAL_ERROR "GLEW not found. On macOS, install it via: brew install glew")
endif()

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(imgui PUBLIC OpenGL::GL GLEW::GLEW)

# Dependencies: ImNodes (node editor for ImGui)
FetchContent_Declare(
  imnodes
  GIT_REPOSITORY https://github.com/Nelarius/imnodes.git
  GIT_TAG master
)
FetchContent_Populate(imnodes)
add_library(imnodes STATIC
  ${imnodes_SOURCE_DIR}/imnodes.cpp
)
target_include_directories(imnodes PUBLIC ${imnodes_SOURCE_DIR})
target_link_libraries(imnodes PUBLIC imgui)

# Sanitizers (optional)
if(NBVS_BUILD_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

# App
file(GLOB NBVS_SOURCES CONFIGURE_DEPENDS src/*.cpp src/**/*.cpp)
add_executable(nbvs ${NBVS_SOURCES})
target_include_directories(nbvs PRIVATE src)
target_link_libraries(nbvs PRIVATE imgui imnodes glfw OpenGL::GL GLEW::GLEW)

if(APPLE)
  # Required frameworks typically handled by GLFW, but keep for safety
  target_link_libraries(nbvs PRIVATE
    "-framework Cocoa" "-framework IOKit" "-framework CoreVideo"
  )
endif()
