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
    class Scene;

    namespace entity {

        class StaticProp;
        class Camera;
        class Sign;

    }

    namespace graphics {

        class Framebuffer;
        class Geometry;
        class GraphicsController;
        class GraphicsEventHandler;
        class ImageTexture;
        class RenderManager;
        class SimpleRenderManager;
        class Screen;
        class Shader;
        class ShaderProgram;
        class Texture;
        class TextTexture;

        #ifdef SE_ENABLE_QT_WIDGET_SUPPORT
        class QTSilhouetteWidget;
        #endif

    }

    namespace input {

        class InputController;

    }

    namespace logic {

        class LogicController;
        class Tickable;
        
    }

}

#endif