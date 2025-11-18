cmake_minimum_required(VERSION 3.14)
project(MemoryResourceProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)
add_executable(main_app 
    main.cpp 
    block_memory_resource.cpp
)

add_executable(run_tests 
    test.cpp 
    block_memory_resource.cpp
)
target_link_libraries(run_tests gtest_main gmock)
target_include_directories(main_app PRIVATE .)
target_include_directories(run_tests PRIVATE .)
