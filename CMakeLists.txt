cmake_minimum_required(VERSION 3.7)
project(FuturePromise)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pedantic -g -fsanitize=thread")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++14 -O2 -g")


set(SOURCE_FILES main.cpp future.h promise.h shared_object.h promise_future_exception.h flatten.h ThreadPool.cpp ThreadPool.h tuple.h)
add_executable(FuturePromise ${SOURCE_FILES})