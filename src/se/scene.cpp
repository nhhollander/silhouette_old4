/*!
 *  @file src/se/scene.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/scene.hpp"

#include "se/entity/staticProp.hpp"

#include "util/dirs.hpp"
#include "util/log.hpp"

#include <nlohmann/json.hpp>
#include <fstream>

using namespace se;
using namespace se::entity;
using namespace nlohmann;

// =====================
// == PRIVATE MEMBERS ==
// =====================

// ====================
// == PUBLIC MEMBERS ==
// ====================

Scene::Scene(se::Engine* engine) {
    this->engine = engine;
}

Scene::~Scene() {
    // Remove everything from the active lists before deleting
    this->renderable_entities.clear();
    this->tickable_entities.clear();
    this->all_entities.clear();
    // Delete the entities
    for(auto entity : this->internally_loaded) {
        delete entity;
    }
    this->internally_loaded.clear();
}

void Scene::load_scene(const char* fname) {
    // Get the file path
    std::string fpath;
    fpath += util::dirs::app_data();
    fpath += "/scenes/";
    fpath += fname;
    fpath += ".scene";
    DEBUG("Loading scene from file [%s]", fpath.c_str());
    // Open the file
    std::ifstream input_file(fpath);
    if(!input_file.is_open()) {
        ERROR("Failed to open [%s]", fpath.c_str());
        return;
    }
    // Parse the json and then interpret the scene data
    json scene_data = json::parse(input_file);
    // Load entities
    DEBUG("Scene contains %i entities", scene_data["entities"].size());
    for(auto entity : scene_data["entities"]) {
        std::string geometry = entity.value<std::string>("geometry", "");
        std::string texture  = entity.value<std::string>("texture", "");
        if(geometry == "" || texture == "") {
            WARN("Malformed entity entry! [geom: %s, text: %s]",
                (geometry == "") ? "INVALID" : "OK",
                (texture == "")  ? "INVALID" : "OK");
            continue;
        }
        /* Right now scenes only support static props.  This will need to be
        expanded in the future to also include useful things. */
        StaticProp* sp = new StaticProp(this->engine, geometry.c_str(), texture.c_str());
        sp->x =  entity.value<float>("pos.x", 0.0);
        sp->y =  entity.value<float>("pos.y", 0.0);
        sp->z =  entity.value<float>("pos.z", 0.0);
        sp->rx = entity.value<float>("rot.x", 0.0);
        sp->ry = entity.value<float>("rot.y", 0.0);
        sp->rz = entity.value<float>("rot.z", 0.0);
        sp->sx = entity.value<float>("scale.x", 0.0);
        sp->sy = entity.value<float>("scale.y", 0.0);
        sp->sz = entity.value<float>("scale.z", 0.0);
        // Add to the entity lists
        this->internally_loaded.push_back(sp);
        this->register_entity(sp);
    }
    
}

std::vector<se::Entity*>* Scene::get_renderables() {
    return &this->renderable_entities;
}

std::vector<se::Entity*>* Scene::get_tickables() {
    return &this->tickable_entities;
}

void Scene::register_entity(se::Entity* entity) {
    for(auto i : this->all_entities) {
        if(i == entity) {
            // TODO: More detail?
            WARN("Attempted to register duplicate entity [%p]", entity);
            return;
        }
    }

    this->all_entities.push_back(entity);

    if(entity->is_renderable()) {
        this->renderable_entities.push_back(entity);
    }

    if(entity->is_tickable()) {
        this->tickable_entities.push_back(entity);
    }
}