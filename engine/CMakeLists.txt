cmake_minimum_required(VERSION 3.20)
project(PrettyFly)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(PrettyFly src/main.cpp)

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

add_custom_target(Scripts SOURCES ${SOLUTION_ITEMS})