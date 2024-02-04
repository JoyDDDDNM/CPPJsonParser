cmake_minimum_required(VERSION 3.27.4)

project(JParser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(SOURCE main.cpp JObject.cpp Parser.cpp)

add_executable(${PROJECT_NAME} ${SOURCE})