# Pixel Core
This repository contains Pixel Core, a lightweight rendering engine built using OpenGL.

## Requirements
* [Git](https://git-scm.com): Required for version control.
* [CMake](https://cmake.org): Used for generating platform-specific build files.
* [OpenGL](https://www.opengl.org/): The underlying graphics API.
* C++ Compiler: A modern C++ compiler compatible with C++20 or later.
    
## Getting started
<ins>**1. Downloading the repository:**</ins>
Clone the repository recursively to ensure all submodules are downloaded:
```
git clone --recursive [Repository URL]
```
and access the directory of the cloned repository.

To clone the necessary submodules, if the repository was cloned non-recursively, use:
```
git submodule update --recursive --init
```

<ins>**2. Build the project solution:**</ins>
The project solution can be generated using CMake:

* **For Visual Studio**:
	```
	cmake -B VisualStudio
	```
	The Visual Studio solution `pixel-core.sln` can be found in the `VisualStudio/` directory.

* **For XCode**:
	```
	cmake -G Xcode -B XCode
	```
	The XCode project solution `pixel-core.xcodeproj` can be found in `XCode/` directory.

## Third party libraries
The external libraries needed in this project have been added as git submodules. These can be found in the `3rdparty/` directory.
* [Assimp](https://github.com/assimp/assimp): Asset loading library.
* [glew](https://github.com/nigels-com/glew): OpenGL extension wrangler library.
* [glfw](https://github.com/glfw/glfw): Window management and input handling.
* [glm](https://github.com/g-truc/glm): OpenGL mathematics.
* [ImGui](https://github.com/ocornut/imgui): Dear ImGui, a bloat-free graphical user interface library.
* [spdlog](https://github.com/gabime/spdlog): Fast C++ logging library.
* [stb](https://github.com/nothings/stb): Image loading library from the stb single-file public domain libraries.

## References
This project draws inspiration and utilizes code from the following resources:
* [Hazel Engine](https://github.com/TheCherno/Hazel) from Yan Chernokov.
* [Learn OpenGL](https://github.com/JoeyDeVries/LearnOpenGL) from Joey de Vries.
* [OpenGL Tutorial](https://github.com/VictorGordan/opengl-tutorials) from Victor Gordan.
