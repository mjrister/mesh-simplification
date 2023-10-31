add_executable(mesh_simplification_tests main.cpp
                                         geometry/face_test.cpp
                                         geometry/half_edge_mesh_test.cpp
                                         geometry/half_edge_test.cpp
                                         geometry/vertex_test.cpp
                                         graphics/arcball_test.cpp
                                         graphics/mesh_test.cpp
                                         graphics/obj_loader_test.cpp)

find_package(GTest CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(gl3w CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(mesh_simplification_tests PRIVATE GTest::gtest_main
                                                        OpenGL::GL
                                                        common_dbg_asan
                                                        common_glm_definitions
                                                        common_warnings
                                                        glfw
                                                        glm::glm
                                                        unofficial::gl3w::gl3w)

target_include_directories(mesh_simplification_tests PRIVATE ../src)

include(GoogleTest)
gtest_discover_tests(mesh_simplification_tests)
