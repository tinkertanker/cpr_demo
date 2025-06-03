# Although this project uses CMake, you should use Makefile for your
# project. The reason is to teach you fundamentals of how C++ code can
# be modularised. 
#
# After understanding Makefile, it will be a lot easier to understand 
# the other build systems.
#
# (https://stackoverflow.com/questions/406004/call-cmake-from-make-to-create-makefiles)
# (https://www.perplexity.ai/search/is-cmake-in-general-slower-tha-QKJ9XTkZSc2UTw4cLRCnFw#1)
#
# In general, when adding, removing, or renaming a file in a multi-file C++ project,
# the CMake configure build tree section takes a significant amount of time.
cmake_minimum_required(VERSION 3.28)
project(cpr_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -fsanitize=address")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -ggdb -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")

include(FetchContent)

# Used for REST api, which is above the HTTP protocol, which is above the
# TCP protocol, which is above the IP protocol
FetchContent_Declare(
  cpr
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
  GIT_TAG 1.10.5
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(cpr)

# cpr does not support websockets
# uncomment this if you need websocket support
# set(USE_TLS TRUE)
# FetchContent_Declare(
#  ixwebsocket
#  GIT_REPOSITORY https://github.com/machinezone/IXWebSocket
#  GIT_TAG v11.0.2
#  GIT_SHALLOW TRUE)
# FetchContent_MakeAvailable(ixwebsocket)

# Used to parse JSON
FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG v2.6.8
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glaze)

# Used for logging
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG tags/v1.14.1
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(spdlog)

add_executable(chat_client cpr_test.cc)

target_link_libraries(chat_client PRIVATE cpr::cpr spdlog::spdlog_header_only ixwebsocket glaze::glaze)
