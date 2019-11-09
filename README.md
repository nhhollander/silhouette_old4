# Silhouette Engine

Silhouette is a basic 3D engine written in OpenGL.  I started work on Silhouette in the summer of 2017 with a series of experiments in Vulkan and OpenGL, from which I leared a considerable amount of my knowledge about 3D graphics.  Since the original inception of this project, it has been cut down and re-built from the ground up several times, generally as a result of fundamental issues that could not be solved without re-writing a majority of the codebase.

## Dependencies

Silhouette depends on the following packages.  They can be automatically installed by running `setup_linux.sh`.

 * `git`
 * `libgl-dev` 
 * `libglu1-mesa-dev` 
 * `libpng-dev`
 * `libglm-dev`
 * `nlohmann-json-dev` 
 * `make`
 * `cmake`
 * `build-essential`
 * `libglew-dev`
 * `libsdl2-dev`

## Building

### Building on Linux

Ensure all dependencies are installed manually, or by running the `setup_linux.sh` script.

To build, enter the root directory of the project and run `make`.

### Buildling on other platforms

At this time, building is not supported on other platforms, but it should still be possible if you are able to manually install the dependencies.

## License

[This project is licensed under the MIT license](LICENSE)