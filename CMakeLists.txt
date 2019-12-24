cmake_minimum_required(VERSION 3.10)
project(nr_opentracing_cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_PATH deps/include)

#includes
include_directories(SYSTEM deps/include include)

# Libraries
set(CMAKE_LIBRARY_PATH deps/lib)

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
        INSTALL_COMMAND pwd && cp include/libnewrelic.h ${PROJECT_SOURCE_DIR}/deps/include/newrelic/
        )

ExternalProject_Add(opentracing-cpp
        INSTALL_DIR ${PROJECT_SOURCE_DIR}/deps
        GIT_REPOSITORY https://github.com/opentracing/opentracing-cpp
        GIT_TAG v1.6.0
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        )

#ExternalProject_Add(rapidjson
#        INSTALL_DIR ${PROJECT_SOURCE_DIR}/deps
#        GIT_REPOSITORY git@github.com:Tencent/rapidjson.git
#        GIT_TAG  v1.1.0
#        CMAKE_ARGS
#        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
#        )


# Dropped this from add_dependencies as it takes too much time every make
ExternalProject_Add(nlohmann_json
        INSTALL_DIR ${PROJECT_SOURCE_DIR}/deps
        GIT_REPOSITORY git@github.com:nlohmann/json.git
        GIT_TAG v3.7.3
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        )


# Source
file(GLOB_RECURSE NR_OPENTRACING_SOURCES "src/*.cpp")
add_library(nr_opentracing SHARED ${NR_OPENTRACING_SOURCES} )

file(GLOB_RECURSE ExternalLibs "deps/lib/libopentracing.dylib")
message(${ExternalLibs})
target_link_libraries(nr_opentracing ${ExternalLibraries})

