# Introduction to the Shading System

This document provides a brief overview of the OpenGL shader system and its
implementation within the Silhouette Engine.

## Shaders

A shader is a GLSL program designed to perform a single stage of the render
process.  You can find out more about shaders on the
[Khronos Wiki](https://www.khronos.org/opengl/wiki/Shader)

Shaders are implemented in `se::graphics::Shader`.  This class provides systems
for loading, compiling, and caching shaders.

## Shader Programs

A shader program is a system comprised of mutiple shaders which converts lists
of points (geometry) into colored pixels on the screen.  At a minimum, shader
programs must have a vertex shader and a fragment shader for positioning and
rendering polygons respectively.

Shader programs are implemented in `se::graphics::ShaderProgram`.  This class
provides systems for loading, compiling, and caching shaders.

## Texture

Unless you're insane enough to develop procedural shading for every object in
your scene, you're going to want to apply a texture at some point.  Because of
the way that OpenGL is, you can only have a finite number of textures loaded
onto the rendering device at any given time.

Textures are also used in multi-stage rendering, where the result of one render
operation needs to be re-used at a later time.

Textures are implemented in `se::graphics::Texture`.  This class provides
systems for loading, decoding, and caching shaders.