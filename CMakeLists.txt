cmake_minimum_required(VERSION 3.10)
project(nr_opentracing_cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_PATH deps/include)

#includes
include_directories(SYSTEM deps/include include)

# Libraries
set(CMAKE_LIBRARY_PATH deps/lib)
set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-export-dynamic")
option(BUILD_SHARED_LIBS "" ON)

file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/deps/bin/)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/deps/lib/)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/deps/include/)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/deps/include/newrelic)

# Dependencies
include(ExternalProject)
ExternalProject_Add(newrelic-c-sdk
        INSTALL_DIR ${PROJECT_SOURCE_DIR}/deps
        #file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/deps/include/newrelic)
        GIT_REPOSITORY git@github.com:newrelic/c-sdk.git
        GIT_TAG v1.2.0
        CONFIGURE_COMMAND ""
        BUILD_IN_SOURCE true
        BUILD_COMMAND pwd && make
        INSTALL_COMMAND pwd && cp include/libnewrelic.h ${PROJECT_SOURCE_DIR}/deps/include/newrelic/ &&
            cp libnewrelic.a ${PROJECT_SOURCE_DIR}/deps/lib &&
            cp newrelic-daemon ${PROJECT_SOURCE_DIR}/deps/bin/
        )

ExternalProject_Add(opentracing-cpp
        INSTALL_DIR ${PROJECT_SOURCE_DIR}/deps
        GIT_REPOSITORY https://github.com/opentracing/opentracing-cpp
        GIT_TAG v1.5.1
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        )

# Dropped this from add_dependencies as it takes too much time every make
#ExternalProject_Add(nlohmann_json
#        INSTALL_DIR ${PROJECT_SOURCE_DIR}/deps
#        GIT_REPOSITORY git@github.com:nlohmann/json.git
#        GIT_TAG v3.7.3
#        CMAKE_ARGS
#        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
#        )

# Source
file(GLOB_RECURSE NR_OPENTRACING_SOURCES "src/*.cpp")
add_library(nr_opentracing SHARED ${NR_OPENTRACING_SOURCES} )
add_dependencies(nr_opentracing newrelic-c-sdk opentracing-cpp)

#file(GLOB_RECURSE ExternalLibs "deps/lib/libopentracing.so" "deps/lib/libnewrelic.a")
set(ExternalLibs libopentracing libnewrelic)
message(INFO " External libraries: ${ExternalLibs}")
target_link_libraries(nr_opentracing opentracing newrelic -L/home/mike/CLionProjects/nr-opentracing-cpp/deps/lib)

