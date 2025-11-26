cmake_minimum_required(VERSION 3.10)
project(SKIP)

enable_testing()

include(FetchContent)

# Fetch nlohmann/json
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# Fetch pugixml
FetchContent_Declare(
  pugixml
  GIT_REPOSITORY https://github.com/zeux/pugixml.git
  GIT_TAG v1.12
)
FetchContent_MakeAvailable(pugixml)

add_library(skip SHARED skip.c)

add_executable(tests main.cpp)
target_link_libraries(tests skip)

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark skip nlohmann_json::nlohmann_json pugixml)

find_package(PythonInterp REQUIRED)
add_test(NAME python_example COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python/example.py)
set_tests_properties(python_example PROPERTIES DEPENDS tests)

find_program(GO_EXECUTABLE go)
if(GO_EXECUTABLE)
    add_test(NAME go_example COMMAND ${GO_EXECUTABLE} run ${CMAKE_CURRENT_SOURCE_DIR}/go/example.go)
    set_tests_properties(go_example PROPERTIES
        DEPENDS tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/go
        ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}")
endif()



find_package(Java REQUIRED)
find_program(MVN_EXECUTABLE mvn)
if(MVN_EXECUTABLE AND JAVA_FOUND)
    add_custom_target(java_build ALL
        COMMAND ${MVN_EXECUTABLE} package
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/java
    )
    add_test(NAME java_example
             COMMAND ${MVN_EXECUTABLE} exec:java)
    set_tests_properties(java_example PROPERTIES
        DEPENDS "java_build;tests"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/java
        ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}")
endif()


find_program(CARGO_EXECUTABLE cargo)
if(CARGO_EXECUTABLE)
    add_test(NAME rust_example
             COMMAND ${CARGO_EXECUTABLE} run --manifest-path ${CMAKE_CURRENT_SOURCE_DIR}/rust/Cargo.toml --example simple
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(rust_example PROPERTIES
        DEPENDS tests
        ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}")
endif()
