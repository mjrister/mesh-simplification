cmake_minimum_required(VERSION 3.22)
project(mesh_simplification VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# interface targets to ease reuse of common compiler configurations
add_library(common_dbg_asan INTERFACE)
add_library(common_glm_definitions INTERFACE)
add_library(common_warnings INTERFACE)

target_compile_definitions(common_glm_definitions INTERFACE GLM_ENABLE_EXPERIMENTAL # required for <glm/gtx/hash.hpp>
                                                            GLM_FORCE_XYZW_ONLY)    # required for cppcoreguidelines-pro-type-union-access

if(MSVC)
  target_compile_options(common_warnings INTERFACE /W4 /WX)
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_options(common_dbg_asan INTERFACE /fsanitize=address)
  endif()
else()
  target_compile_options(common_warnings INTERFACE -Wall -Werror -Wextra -Wpedantic)
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_options(common_dbg_asan INTERFACE -fsanitize=address)
    target_link_options(common_dbg_asan INTERFACE -fsanitize=address)
  endif()
endif()

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
