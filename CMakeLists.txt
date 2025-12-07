cmake_minimum_required(VERSION 3.24)

project(Concurrent_Heap)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)

FetchContent_MakeAvailable(Catch2)

add_library(project_multithreadedHeap src/leftistHeap.cpp src/concurrentHeap.cpp)

add_executable(functionality_tests tests/functionality_tests.cpp)
add_executable(test_dijkstra_leftistHeap tests/test_dijkstra_leftistHeap.cpp)
add_executable(compare_dijkstra_leftist_concurrent tests/compare_dijkstra_leftist_concurrent.cpp)
add_executable(pq_test tests/pq_test.cpp)
add_executable(pq_test_multithreaded tests/pq_test_multithreaded.cpp)

add_executable(leftistHeap leftistHeap_main.cpp)
add_executable(concurrentHeap concurrentHeap_main.cpp)
add_executable(dijkstra_leftistHeap dijkstra_leftistHeap.cpp)
add_executable(dijkstra_concurrentHeap dijkstra_concurrentHeap.cpp)

target_include_directories(functionality_tests PRIVATE src)
target_include_directories(leftistHeap PRIVATE src)
target_include_directories(concurrentHeap PRIVATE src)
target_include_directories(dijkstra_leftistHeap PRIVATE src)
target_include_directories(dijkstra_concurrentHeap PRIVATE src)


target_link_libraries(leftistHeap PRIVATE project_multithreadedHeap)
target_link_libraries(concurrentHeap PRIVATE project_multithreadedHeap)
target_link_libraries(dijkstra_leftistHeap PRIVATE project_multithreadedHeap)
target_link_libraries(dijkstra_concurrentHeap PRIVATE project_multithreadedHeap)

target_link_libraries(functionality_tests PRIVATE Catch2::Catch2WithMain project_multithreadedHeap)
target_link_libraries(test_dijkstra_leftistHeap PRIVATE Catch2::Catch2WithMain project_multithreadedHeap)
target_link_libraries(compare_dijkstra_leftist_concurrent PRIVATE Catch2::Catch2WithMain project_multithreadedHeap)
target_link_libraries(pq_test PRIVATE Catch2::Catch2WithMain project_multithreadedHeap)
target_link_libraries(pq_test_multithreaded PRIVATE Catch2::Catch2WithMain project_multithreadedHeap)
