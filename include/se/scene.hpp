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

#include <nlohmann/json.hpp>
#include <vector>
#include <map>

namespace se {

    /*!
     *  Wrapped Entity Constructor Type.
     * 
     *  The wrapped entity constructor is responsible for creating and returning
     *  new instances of an arbitrary entity type.  When loading a scene file,
     *  the the scene loader will call this function for any matching registered
     *  type.
     * 
     *  This function should return a pointer to a completely constructed entity
     *  if possible, however in the event of a failure it should return
     *  `nullptr`.
     */
    typedef std::function<
        se::Entity*(
            se::Engine* engine,
            se::Scene* scene,
            nlohmann::json::value_type attribs
        )
    > WrappedEntityConstructor;

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
            std::map<uint8_t, Entity*> all_entities;

            /// Renderable entities
            std::vector<Entity*> renderable_entities;

            /// Tickable entities
            std::vector<Entity*> tickable_entities;

            /// Construction functiosn
            std::map<uint32_t, WrappedEntityConstructor> constructors;

            /*!
             *  Internally Loaded Entities.
             * 
             *  Entities which were constructed by a call to `load_scene()` and
             *  must be destroyed with the scene.
             */
            std::vector<Entity*> internally_loaded;

            /// Generate default wrapped entity constructors.
            void generate_default_wrapped_entity_constructors();

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
             *  Returns a pointer to the tickable entity vector.
             * 
             *  **Do not add or remove entities from this vector**
             */
            std::vector<se::Entity*>* get_tickables();

            /*!
             *  Get All Entities.
             * 
             *  Returns a pointer to the entity map.
             * 
             *  **Do not add or remove entities from this vector**
             */
            std::map<uint8_t, se::Entity*>* get_entities();

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

            /*!
             *  Register a constructor.
             * 
             *  Adds a constructor to the map for the registered type.  If there
             *  is a type collision, a warning will be generated and the new
             *  constructor will *not* be registered.
             * 
             *  @param type Name of the type to register the constructor for.
             *  @param constructor  Constructor to register.
             */
            void register_constructor(
                const char* type, WrappedEntityConstructor constructor);

            /*!
             *  Get an entity by name.
             * 
             *  Attempts to find the requested entity in the list of all
             *  registered entities.  If it can not be found, a null pointer
             *  will be returned.
             */
            se::Entity* get_entity(const char* name);

    };

}

#endif