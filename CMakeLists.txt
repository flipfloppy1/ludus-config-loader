
cmake_minimum_required(VERSION 3.11)
include(FetchContent)
project("LCL")

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03 # 3.11.3
)

FetchContent_Declare(
  LLG
  GIT_REPOSITORY https://github.com/flipfloppy1/ludus-log-generator.git
  GIT_TAG        main
)

FetchContent_MakeAvailable(json LLG)

add_library(LCL "Config.h")

target_link_libraries(LCL PUBLIC json)
target_link_libraries(LCL PUBLIC LLG)

set_property(TARGET LCL PROPERTY LINKER_LANGUAGE CXX)
