add_executable(mesh_simplification main.cpp
                                   geometry/face.cpp
                                   geometry/half_edge_mesh.cpp
                                   geometry/mesh_simplifier.cpp
                                   graphics/arcball.cpp
                                   graphics/mesh.cpp
                                   graphics/obj_loader.cpp
                                   graphics/scene.cpp
                                   graphics/shader_program.cpp
                                   graphics/window.cpp)

find_package(OpenGL REQUIRED)
find_package(gl3w CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(mesh_simplification PRIVATE OpenGL::GL
                                                  common_dbg_asan
                                                  common_glm_definitions
                                                  common_warnings
                                                  glfw
                                                  glm::glm
                                                  unofficial::gl3w::gl3w)

target_include_directories(mesh_simplification PRIVATE .)

# Copy assets to the current binary directory so they're available at runtime
add_custom_target(copy_assets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets
                                                                         ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(mesh_simplification copy_assets)
