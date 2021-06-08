# Mesh Simplification

## Introduction
This project implements an algorithm for reducing the number of triangles in polygon mesh first presented by Garland-Heckbert's seminal research paper entitled [Surface Simplification Using Quadric Error Metrics](https://github.com/matthew-rister/mesh_simplification/blob/main/MeshSimplification/docs/surface_simplification.pdf). In Computer Graphics applications, performance latency is often directly proportional to the total number of triangles rendered in the scene. Therefore, this algorithm can help mitigate performance bottlenecks by simplifying complex triangle meshes while preserving the shape of the model as much as possible.

The core idea of the algorithm is to compute the optimal vertex position to collapse an edge onto using a quadric error metric which measures how much the mesh changes after an edge has been collapsed. It then iteratively removes edges with the lowest cost using a standard priority queue to collapses edges until a user-provided stopping criterion is met. To facilitate the implementation of this algorithm, a data structure known as a [half-edge mesh](https://github.com/matthew-rister/mesh_simplification/blob/main/MeshSimplification/geometry/half_edge_mesh.cpp) is employed to efficiently traverse and modify edges in the mesh.

## Results

The following images presents the results of reducing a triangle mesh consisting of nearly 70,000 triangles (left) by 75% (right). Although fidelity is somewhat reduced, the mesh retains an overall high quality appearance that nicely approximates the original shape the mesh. Furthermore, this complex mesh was simplified in just under 3.5 seconds on a Release build using an 7th generation Intel quad-core processor.

Original Mesh (69630 Triangles)           |  Simplified Mesh (17406 Triangles)
:-------------------------:|:-------------------------:
![](https://github.com/matthew-rister/mesh_simplification/blob/main/bunny_original_69630.PNG)  |  ![](https://github.com/matthew-rister/mesh_simplification/blob/main/bunny_simplified_17406.PNG)

## Additional Features

- An arcball interface for rotating the mesh by dragging the cursor across the screen.
- An [implementation](https://github.com/matthew-rister/mesh_simplification/blob/main/MeshSimplification/graphics/obj_loader.cpp) for loading [Wavefront .obj](https://en.wikipedia.org/wiki/Wavefront_.obj_file) files.
- Lighting using the Phong reflection model

## How To Build
This project was building using C++20 and OpenGL 4.6. To build it, you will need a version of Visual Studio that supports the latest language standard (e.g., Visual Studio 2019 v16.10.0). Additionally, this project uses vcpkg to manage 3rd party dependencies. To initialize the repository clone the repository from [vcpkg Github](https://github.com/microsoft/vcpkg) and run `bootstrap-vcpkg.bat` followed by `vcpkg integrate install`. Once this has been done, you should be ready to build the project using Visual Studio which will read and install dependencies from the `vcpkg.json` manifest on first build. At this time only x64 architecture is supported otherwise the program may run into an infinite loop due to unhandled hash collisions.