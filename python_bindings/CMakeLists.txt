cmake_minimum_required(VERSION 3.22) 
project(pybertini)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_PHOENIX_STL_TUPLE_H_")


message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include(CMakePrintHelpers) # so we can print cmake variable values

# add_compile_definitions(-DBMP_EXPRESSION_TEMPLATES=1)

add_compile_definitions(
    $<$<CONFIG:Debug>:-DPYBERTINI_ENABLE_CPP_SIGNATURES>
)

add_compile_options(
    # put things for debug compile here
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Debug>:-Wall>

    # and things for release here
    $<$<CONFIG:Release>:-O3>  
    # using O2 or O3 cuts runtime by over half. 
)  

#sometimes you need to see a lot, and this lets you see ... so much
#include(CMakePrintHelpers)


# note about building using conda-provided tools:
# In order to find conda, run 'conda activate' and then use 'cmake .. -DCMAKE_PREFIX_PATH=$CONDA_PREFIX' when cmaking

# All source files to be compiled
# We can either explicitly list all files or use glob, we chose to explicitly list files and not to glob
#file(GLOB SOURCES src/*.cpp)



#Builds a C++ library and the python bindings around it

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

find_package(GMP REQUIRED)
find_package(MPFR REQUIRED)
find_package(MPC REQUIRED)

include_directories(${GMP_INCLUDES})
include_directories(${MPC_INCLUDES})




find_package(eigenpy 3.3 REQUIRED CONFIG)
# Eigenpy finds Boostpython, having our call to find boost libraries before the eigenpy call erases our call, so our call for boost libraries is after this line





# Find python and Boost - both are required dependencies
# so that we can `python_add_library`
find_package(Python 3.13 REQUIRED COMPONENTS Interpreter Development.Module NumPy)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)



if (PROJECT_IS_TOP_LEVEL)
    find_package(bertini2 REQUIRED)
endif()

find_package(Boost 1.82 REQUIRED
                COMPONENTS 
                    serialization
                    wserialization
                    unit_test_framework
                    filesystem
                    system
                    chrono
                    regex
                    timer
                    log
                    thread
                    log_setup
                    python${Python_VERSION_MAJOR}${Python_VERSION_MINOR} # uses the versions found by find_package(Python3 ...) above.
)








#################  done finding things.  now we list our sources


set(PYBERTINI_HEADERS
    include/bertini_python.hpp
    include/eigenpy_interaction.hpp
    include/function_tree_export.hpp
    include/mpfr_export.hpp
    include/random_export.hpp
    include/node_export.hpp
    include/symbol_export.hpp
    include/operator_export.hpp
    include/root_export.hpp
    include/system_export.hpp
    include/tracker_export.hpp
    include/endgame_export.hpp
    include/parser_export.hpp
    include/generic_observer.hpp
    include/generic_observable.hpp
    include/tracker_observers.hpp
    include/endgame_observers.hpp
    include/detail.hpp
    include/logging.hpp
)

set(PYBERTINI_SOURCES
    src/eigenpy_interaction.cpp
    src/logging.cpp
    src/detail.cpp
    src/containers.cpp
    src/tracker_export.cpp
    src/endgame_export.cpp
    src/random_export.cpp
    src/mpfr_export.cpp
    src/node_export.cpp
    src/symbol_export.cpp
    src/operator_export.cpp
    src/root_export.cpp
    src/system_export.cpp
    src/parser_export.cpp
    src/generic_observable.cpp
    src/generic_observer.cpp
    src/tracker_observers.cpp
    src/endgame_observers.cpp
    src/zero_dim_export.cpp
    src/bertini_python.cpp
)



######   set up some stuff about the project



if(PROJECT_IS_TOP_LEVEL)
    python_add_library(_pybertini MODULE WITH_SOABI) 
endif()

target_include_directories(_pybertini PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(_pybertini PRIVATE ${PYBERTINI_SOURCES} ${PYBERTINI_HEADERS})

if(NOT PROJECT_IS_TOP_LEVEL)
    target_link_libraries(_pybertini PUBLIC bertini2)
else()
    target_link_libraries(_pybertini PUBLIC ${Bertini2_LIBRARIES})
endif()

# which libraries are needed
target_link_libraries(_pybertini PUBLIC ${GMP_LIBRARIES})
target_link_libraries(_pybertini PUBLIC ${MPFR_LIBRARIES})
target_link_libraries(_pybertini PUBLIC ${MPC_LIBRARIES})


# which directories contain headers we need
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${Python3_NumPy_INCLUDE_DIRS})
include_directories(${Bertini2_INCLUDES})

# which libraries do we need to link to
target_link_libraries(_pybertini PUBLIC Eigen3::Eigen)
target_link_libraries(_pybertini PUBLIC eigenpy::eigenpy)
target_link_libraries(_pybertini PUBLIC ${Boost_LIBRARIES})

cmake_print_variables(SKBUILD)
if(${SKBUILD})
    # see https://stackoverflow.com/questions/1242904/finding-python-site-packages-directory-with-cmake
    install(TARGETS _pybertini LIBRARY DESTINATION .)
else()
    install(TARGETS _pybertini LIBRARY DESTINATION ${Python_SITELIB}/bertini)
endif()

