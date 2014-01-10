cmake_minimum_required(VERSION 2.6)

project(DiffBotClient)

set(DiffBotClientInclude   ./src PARENT_SCOPE)

file(GLOB Sources ./src/*.cpp)

add_executable(DiffBotClient ${Sources})

include_directories(${DiffBotClientInclude})

target_link_libraries(DiffBotClient curl)