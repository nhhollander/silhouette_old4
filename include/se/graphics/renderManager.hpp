/*!
 *  @file include/se/graphics/renderManager.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_RENDERMANAGER_H_
#define _SE_GRAPHICS_RENDERMANAGER_H_

namespace se::graphics {

    /*!
     *  Render manager template class.
     * 
     *  Any class seeking to claim the title of render manager shall implement
     *  the following methods.
     */
    class RenderManager {

        public:

            /*!
             *  Render a frame.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             */
            virtual void render_frame() = 0;

    };

}

#endif