cmake_minimum_required(VERSION 3.20)
project(polytype_puzzle_solver)
set(HEADER_DIR include)
set(SOURCE_DIR src)
include_directories(${HEADER_DIR})
set(SOURCES
    ${SOURCE_DIR}/csv_parser.cpp 
    ${SOURCE_DIR}/solvers.cpp 
    main.cpp
)
set(HEADERS
    ${HEADER_DIR}/csv_parser.hpp
    ${HEADER_DIR}/custom_templates.hpp
    ${HEADER_DIR}/json.hpp
    ${HEADER_DIR}/solvers.hpp
)
add_executable(${PROJECT_NAME} ${SOURCES})