# Mesh Simplification

## Introduction
This project presents an algorithm for reducing the number of triangles in polygon mesh first described in Garland-Heckbert's seminal research paper entitled [Surface Simplification Using Quadric Error Metrics](https://github.com/matthew-rister/mesh_simplification/blob/main/MeshSimplification/docs/surface_simplification.pdf). In Computer Graphics applications, performance is often directly tied to the total number of triangles rendered in the scene. Therefore, this algorithm can help mitigate performance bottlenecks by simplifying a complex triangle mesh while preserving the original shape of the model as much as possible.

The core idea of the algorithm is to compute the optimal vertex position to collapse an edge into by measuring how much the mesh changes after the edge has been collapsed. It then iteratively removes edges with the lowest cost using a priority queue to collapses edges until a user-provided stopping criterion is met. To facilitate the implementation of this algorithm, a data structure known as a [half-edge mesh](https://github.com/matthew-rister/mesh_simplification/blob/main/MeshSimplification/geometry/half_edge_mesh.cpp) is employed to efficiently traverse and modify edges in the mesh.

## Results

The following images present a comparison of reducing a triangle mesh consisting of nearly 70,000 triangles (left) by 75% (right). Although fidelity is somewhat reduced, the mesh retains an overall high-quality appearance that nicely approximates the original shape the mesh. 

Original Mesh (69630 Triangles)           |  Simplified Mesh (13924 Triangles) | Simplified Mesh (2784 Triangles)
:-------------------------:|:-------------------------: | :-------------------------:
![](https://github.com/matthew-rister/mesh_simplification/blob/main/MeshSimplification/img/bunny_original_69630.PNG)  |  ![](https://github.com/matthew-rister/mesh_simplification/blob/main/MeshSimplification/img/bunny_simplified1_13924.PNG) | ![](https://github.com/matthew-rister/mesh_simplification/blob/main/MeshSimplification/img/bunny_simplified2_2784.PNG)

## How To Run
This project was building using C++20 and OpenGL 4.6. To build it, you will need a version of Visual Studio that supports the latest language standard (e.g., Visual Studio 2019 v16.10.0). Additionally, this project uses [vcpkg](https://vcpkg.io/en/index.html) to manage 3rd party dependencies. To get started, clone the [vcpkg repository](https://github.com/microsoft/vcpkg) on Github and run `bootstrap-vcpkg.bat` followed by `vcpkg integrate install`. After this, you should be ready to build and run the project using Visual Studio which will install dependencies from the [vcpkg.json](https://github.com/matthew-rister/mesh_simplification/blob/main/vcpkg.json) package manifest on first build. _At this time, only x64 architecture is supported._

Once the program is running, you can rotate the mesh about an arbitrary axis by dragging the cursor across the screen (implemented using an [arcball interface](https://github.com/matthew-rister/mesh_simplification/blob/main/MeshSimplification/docs/arcball.pdf)) in addition to translating the mesh using the standard `W,D,X,A` keys. The mesh can also be uniformly scaled using the `+,-` keys. Finally, to simplify the mesh, press the `S` key.
