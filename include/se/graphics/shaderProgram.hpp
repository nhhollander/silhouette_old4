/*!
 *  @file include/se/graphics/shaderProgram.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_SHADERPROGRAM_H_
#define _SE_GRAPHICS_SHADERPROGRAM_H_

#include "se/fwd.hpp"

#include <map>

namespace se::graphics {
    
    /*!
     *  Program State.
     */
    enum class ShaderProgramState {
        LOADING,
        READY,
        CHILD_ERROR,
        ERROR
    };

    /*!
     *  Get the name of a shader program state.
     */
    const char* shader_program_state_name(ShaderProgramState state);

    /*!
     *  Shader Program Class.
     * 
     *  This class represents a usable shader program.
     */
    class ShaderProgram {

        private:

            /*!
             *  Create a new shader program.
             * 
             *  Where possible, it is preferred to retrieve programs from the
             *  cache instead of creating multiple instances.
             * 
             *  @param parent   The parent `se::Engine` instance.
             *  @param vshader  Name of the vertex shader.
             *  @param vdefines Definititions for the vertex shader.
             *  @param fshader  Name of the fragment shader.
             *  @param fdefines Definitions for the fragment shader.
             */
            ShaderProgram(se::Engine* engine,
                const char* vshader, const char* vdefines,
                const char* fshader, const char* fdefines);
            
            /*!
             *  Destroy this program.
             */
            ~ShaderProgram();

            /// Parent Engine
            se::Engine* engine;
            /// Vertex shader name
            const char* vsname;
            /// Fragment shader name
            const char* fsname;
            /// Shader Name
            std::string name;
            
            /// OpenGL program ID
            unsigned int gl_program = -1;

            /// Vertex Shader
            Shader* vshader = nullptr;
            /// Fragment Shader
            Shader* fshader = nullptr;

            /// Shader Object State
            volatile ShaderProgramState state = ShaderProgramState::LOADING;

            /*!
             *  Linking Method.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             */
            void link();

            /*!
             *  Static Program Cache.
             * 
             *  Linked programs live here.
             */
            static std::map<uint32_t, ShaderProgram*> cache;

            /*!
             *  Current Program for this render thread.
             * 
             *  I don't know if this has any actual performance benefit.
             */
            static thread_local unsigned int current_program;

        public:

            /*!
             *  Get a program.
             * 
             *  This method attempts to retrieve a program by performing the
             *  following steps.
             * 
             *  1. Check the program cache
             *  2. Construct a new program
             * 
             *  @param engine   Parent engine.
             *  @param vsname   Name of the vertex shader.
             *  @param vdefines Definitions for the vertex shader.
             *  @param fsname   Name of the fragment shader.
             *  @param fdefines Definitions for the fragment shader.
             */
            static ShaderProgram* get_program(se::Engine* engine,
                const char* vsname, const char* vdefines,
                const char* fsname, const char* fdefines);

            /*!
             *  Get the State.
             * 
             *  Returne the current state of the program object.
             */
            ShaderProgramState get_state();

            /*!
             *  Wait for loading to complete.
             * 
             *  **Warning:** Make sure to check the state after loading is
             * complete, as it may have resulted in an error state.
             * 
             *  This method will block until this program is no longer in the
             *  loading state, at which point it will return the new state.
             * 
             *  This method checks for load completion every 10ms.
             * 
             *  @return The state the program is in after loading.
             */
            ShaderProgramState wait_for_loading();

            /*!
             *  Use this Program.
             * 
             *  **Warning:** This method must only be called from the render
             *  thread.
             * 
             *  Makes this program the active OpenGL shader program.  If
             *  this program isn't ready or is in an error state, this call does
             *  nothing and will probably cause some weird rendering glitches
             *  but oh well that's not my problem.
             */
            void use_program();
    };

}

#endif