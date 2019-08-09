/*!
 *  @file include/se/fwd.hpp
 * 
 *  Forward Declarations for all silhouette engine types.
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_FWD_H_
#define _SE_FWD_H_

namespace se {
    
    class Engine;
    class Entity;

    namespace entity {

        class StaticProp;
        class Camera;

    }

    namespace graphics {

        class Framebuffer;
        class Geometry;
        class GraphicsController;
        class GraphicsEventHandler;
        class GraphicsResource;
        class RenderManager;
        class SimpleRenderManager;
        class Screen;
        class Shader;
        class ShaderProgram;
        class Texture;

    }

    namespace input {

        class InputController;

    }

}

#endif