cmake_minimum_required(VERSION 3.5)
set(ModName HitScoreVisualizer)

project(${ModName} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${ModName} SHARED 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mod.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/il2cpp/il2cpp_context.cpp
)

include(GenerateExportHeader)
generate_export_header(${ModName})

target_include_directories(${ModName} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/shared")

set_target_properties(${ModName} PROPERTIES
    IMPORTED_IMPLIB "${CMAKE_CURRENT_BINARY_DIR}/${ModName}.lib"
    IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/${ModName}.dll"
)
