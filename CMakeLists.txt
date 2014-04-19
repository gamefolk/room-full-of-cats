set(CMAKE_SYSTEM_NAME GameBoy)

include(CMakeForceCompiler)
cmake_force_c_compiler(lcc "lcc")

cmake_minimum_required(VERSION 2.6)

project(room_full_of_cats)

include_directories(
    "include"
    "tiles"
    "music"
)

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(room_full_of_cats ${SOURCES})
set_target_properties(room_full_of_cats PROPERTIES OUTPUT_NAME cats.gb)
