cmake_minimum_required(VERSION 3.10)
project(SKIP)

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

FetchContent_Declare(
  skip
  GIT_REPOSITORY https://github.com/SkillfulElectro/SKIP.git
  GIT_SHALLOW TRUE          
  GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(skip)

add_library(skip SHARED skip.c)

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark skip nlohmann_json::nlohmann_json pugixml)