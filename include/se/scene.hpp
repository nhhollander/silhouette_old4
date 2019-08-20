/*!
 *  @file include/se/scene.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_SCENE_H_
#define _SE_SCENE_H_

#include "se/fwd.hpp"

#include <vector>

namespace se {

    /*!
     *  Scene.
     * 
     *  In addition to being a collection of entities, scenes are responsible
     *  for directing render and tick events to the appropriate subset of
     *  entities.
     */
    class Scene {

        private:

            /// Parent Engine
            se::Engine* engine;

            /// All Entities
            std::vector<Entity*> all_entities;

            /// Renderable entities
            std::vector<Entity*> renderable_entities;

            /// Tickable entities
            std::vector<Entity*> tickable_entities;

            /*!
             *  Internally Loaded Entities.
             * 
             *  Entities which were constructed by a call to `load_scene()` and
             *  must be destroyed with the scene.
             */
            std::vector<Entity*> internally_loaded;

        public:

            /// Create a new (empty) scene.
            Scene(Engine* engine);

            /// Destroy this scene
            ~Scene();

            /*!
             *  Load a scene file.
             * 
             *  Scenes will be loaded from
             *  `<application data>/scenes/<scene>.scene`.
             */
            void load_scene(const char* fname);

            /*!
             *  Get Renderable Entities.
             * 
             *  Returns a pointer to the renderable entity vector.
             * 
             *  **Do not add or remove entities from this vector**
             */
            std::vector<se::Entity*>* get_renderables();

            /*!
             *  Get Tickable Entities.
             * 
             *  Returns apointer to the tickable entity vector.
             * 
             *  **Do not add or remove entities from this vector**
             */
            std::vector<se::Entity*>* get_tickables();

            /*!
             *  Register an entity.
             * 
             *  Tickable and renderable entities will be atuomatically assigned
             *  to the appropriate entity list.
             */
            void register_entity(se::Entity* entity);

            /*!
             *  Deregister an entity.
             */
            void deregister_entity(se::Entity* entity);

    };

}

#endif