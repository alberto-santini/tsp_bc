cmake_minimum_required(VERSION 3.14)
project(tsp_bc LANGUAGES CXX)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(tsp_bc   src/main.cpp
                        src/CplexSolver.cpp
                        src/CplexSolver.h
                        src/SELazyConstraint.cpp
                        src/SELazyConstraint.h
                        src/SEUserCut.cpp
                        src/SEUserCut.h
                        src/Graph.h
                        src/Graph.cpp)

set(WARNING_OPTIONS             -Wall -Wextra -Werror)
set(GCC_WARNING_OPTIONS         -Wduplicated-cond -Wlogical-op -Wuseless-cast -Wrestrict)
set(ARCH_OPTIONS                -march=native)
set(RELEASE_OPTIONS             -O3)
set(DEBUG_OPTIONS               -O0 -g)
set(GDB_DEBUG_OPTIONS           -ggdb)
set(GCC_DEBUG_DEFINES           _GLIBCXX_DEBUG)
set(LINKER_OPTIONS              -flto -Wl,--no-as-needed)

find_package(Cplex REQUIRED)
find_package(Boost REQUIRED)

target_compile_features(    tsp_bc PRIVATE cxx_std_17)
target_compile_definitions( tsp_bc PRIVATE $<$<AND:$<CONFIG:DEBUG>,$<CXX_COMPILER_ID:GNU>>:${GCC_DEBUG_DEFINES}>)
target_compile_options(     tsp_bc PRIVATE ${WARNING_OPTIONS} ${ARCH_OPTIONS})
target_compile_options(     tsp_bc PRIVATE $<$<CXX_COMPILER_ID:GNU>:${GCC_WARNING_OPTIONS}>)
target_compile_options(     tsp_bc PRIVATE $<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>)
target_compile_options(     tsp_bc PRIVATE $<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>)
target_compile_options(     tsp_bc PRIVATE $<$<AND:$<CONFIG:DEBUG>,$<CXX_COMPILER_ID:GNU>>:${GDB_DEBUG_OPTIONS}>)
target_link_libraries(      tsp_bc PRIVATE ${LINKER_OPTIONS})
target_link_libraries(      tsp_bc PRIVATE $<$<CXX_COMPILER_ID:GNU>:stdc++fs>)
target_link_libraries(      tsp_bc PRIVATE $<$<CXX_COMPILER_ID:Clang>:c++fs>)
target_link_libraries(      tsp_bc PRIVATE Cplex::cplex)
target_link_libraries(      tsp_bc PRIVATE pthread)
target_link_libraries(      tsp_bc PRIVATE dl)
target_include_directories( tsp_bc PRIVATE ${CPLEX_INCLUDE_DIRS})
target_include_directories( tsp_bc PRIVATE ${Boost_INCLUDE_DIRS})
