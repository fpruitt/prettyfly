cmake_minimum_required(VERSION 3.20)
project(PrettyFly)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(PrettyFly
    src/main.cpp
    src/core/MeshLoader.cpp
 "src/core/Camera.h")

target_link_libraries(PrettyFly
    PRIVATE glfw
    PRIVATE glad::glad
    PRIVATE glm::glm
)

# Solution items
set(SOLUTION_ITEMS
    ../configure.ps1
    ../configure.sh
    ../build.ps1
    ../build.sh
    ../run.ps1
    ../run.sh
    ../setup.ps1
    ../setup.sh
    ../README.md
)

add_custom_target(Scripts SOURCES ${SOLUTION_ITEMS} "src/core/Camera.h")

# Preprocessor definitions

target_compile_definitions(PrettyFly PRIVATE
    $<$<CONFIG:Debug>:PF_DEBUG>
    $<$<CONFIG:Release>:PF_RELEASE>
)

# Set startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PrettyFly)
