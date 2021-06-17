# Mesh Simplification

In computer graphics, performance is often directly correlated to the complexity of the scene. One technique for dealing with performance bottlenecks is to simplify a polygon mesh by reducing the number of triangles that compose it. This project presents an efficient algorithm to achieve this, first presented by Garland-Heckbert in a seminal research paper entitled [Surface Simplification Using Quadric Error Metrics](https://github.com/matthew-rister/mesh_simplification/blob/main/MeshSimplification/docs/surface_simplification.pdf). 

The core idea of the algorithm is to compute the optimal vertex position to collapse an edge into that minimizes how much the mesh changes using quadric error metrics. It then iteratively removes edges with the lowest cost using a priority queue to collapses edges until the mesh is sufficiently simplified. To facilitate the implementation of this algorithm, a data structure known as a [half-edge mesh](https://github.com/matthew-rister/mesh_simplification/blob/main/MeshSimplification/geometry/half_edge_mesh.h) is employed to efficiently traverse and modify edges in the mesh.

## Results

The following images present a comparison of reducing a triangle mesh consisting of nearly 70,000 triangles (left) by 95% (right). Although fidelity is somewhat reduced, the mesh retains an overall high-quality appearance that nicely approximates the original shape the mesh. 

Original Mesh (69630 Triangles)           |  Simplified Mesh (3480 Triangles)
:-------------------------:|:-------------------------:
![](https://github.com/matthew-rister/mesh_simplification/blob/main/bunny_original_69630.PNG)  |  ![](https://github.com/matthew-rister/mesh_simplification/blob/main/bunny_simplified_3480.PNG)

## How To Run
This project was implemented using Visual Studio in C++20 and OpenGL 4.6. To build it, you will need a version of Visual Studio that supports the latest language standard (e.g., 16.10.0). Additionally, this project uses [vcpkg](https://vcpkg.io/en/index.html) to manage 3rd party dependencies. To get started, clone the [vcpkg repository](https://github.com/microsoft/vcpkg) on Github and run `bootstrap-vcpkg.bat` followed by `vcpkg integrate install`. After this, you should be ready to build and run the project using Visual Studio which will install dependencies from the [vcpkg.json](https://github.com/matthew-rister/mesh_simplification/blob/main/vcpkg.json) package manifest on first build. _At this time, only x64 architecture is supported._

Once the program is running, you can rotate the mesh about an arbitrary axis by dragging the cursor across the screen (implemented using an [arcball interface](https://github.com/matthew-rister/mesh_simplification/blob/main/MeshSimplification/docs/arcball.pdf)) in addition to translating the mesh using the standard `W,D,X,A` keys. The mesh can also be uniformly scaled using the `+,-` keys. Finally, to simplify the mesh, press the `S` key.
