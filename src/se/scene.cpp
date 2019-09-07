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
#include "util/hash.hpp"
#include "util/log.hpp"

#include <fstream>

using namespace se;
using namespace se::entity;
using namespace nlohmann;

// =====================
// == PRIVATE MEMBERS ==
// =====================

void Scene::generate_default_wrapped_entity_constructors() {
    WrappedEntityConstructor static_prop = [](se::Engine* engine,
        se::Scene* scene, nlohmann::json::value_type attribs){
            std::string geom_name = attribs.value<std::string>("geometry","<missing>");
            std::string text_name = attribs.value<std::string>("texture","<missing>");
            std::string name =      attribs.value<std::string>("name","<sp_no_name>");
            if(geom_name == "<missing>" || text_name == "<missing>") {
                ERROR("Unable to construct Static Prop! A required attribute "
                    "is missing! [geometry: %s] [texture: %s]",
                    geom_name.c_str(), text_name.c_str());
                return (StaticProp*) nullptr;
            }
            StaticProp* sp = new StaticProp(engine, geom_name.c_str(), text_name.c_str());
            sp->set_name(name.c_str());
            return sp;
        };
    this->register_constructor("staticprop", static_prop);
}

// ====================
// == PUBLIC MEMBERS ==
// ====================

Scene::Scene(se::Engine* engine) {
    this->engine = engine;
    this->generate_default_wrapped_entity_constructors();
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
        // Get entity name
        std::string ename = entity.value<std::string>("name","<invalid>");
        std::string type = entity.value<std::string>("type","<invalid>");
        if(type == "<invalid>") {
            WARN("Missing entity type for entity [%s]", ename.c_str());
            continue;
        }
        uint32_t type_hash = util::hash::ejenkins("%s", type.c_str());
        auto find = this->constructors.find(type_hash);
        if(find == this->constructors.end()) {
            WARN("[%s] has unknown entity type [%s]", ename.c_str(), type.c_str());
            continue;
        }
        Entity* new_ent = find->second(this->engine, this, entity);
        if(new_ent == nullptr) {
            WARN("Failed to construct [%s] of type [%s]", ename.c_str(), type.c_str());
            continue;
        }
        try {
            /* Apply global options to entity */
            if(entity.find("pos") != entity.end()) {
                new_ent->x = entity["pos"].value("x", 0.0);
                new_ent->y = entity["pos"].value("y", 0.0);
                new_ent->z = entity["pos"].value("z", 0.0);
            }
            if(entity.find("rot") != entity.end()) {
                new_ent->rx = entity["rot"].value("x", 0.0);
                new_ent->ry = entity["rot"].value("y", 0.0);
                new_ent->rz = entity["rot"].value("z", 0.0);
            }
            if(entity.find("scale") != entity.end()) {
                new_ent->sx = entity["scale"].value("x", 1.0);
                new_ent->sy = entity["scale"].value("y", 1.0);
                new_ent->sz = entity["scale"].value("z", 1.0);
            }
        }
        catch(std::exception& e) {
            WARN("Failed to process position/rotation/scale information for "
                "entity [%s], got error [%s]", ename.c_str(), e.what());
        }
        // Add to the entity lists
        this->internally_loaded.push_back(new_ent);
        this->register_entity(new_ent);
    }
    
}

std::vector<se::Entity*>* Scene::get_renderables() {
    return &this->renderable_entities;
}

std::vector<se::Entity*>* Scene::get_tickables() {
    return &this->tickable_entities;
}

std::map<uint8_t, se::Entity*>* Scene::get_entities() {
    return &this->all_entities;
}

void Scene::register_entity(se::Entity* entity) {
    uint8_t entity_hash = util::hash::ejenkins("%s", entity->get_name());

    for(auto i : this->all_entities) {
        if(i.second == entity) {
            // TODO: More detail?
            WARN("Attempted to register duplicate entity [%p]", entity);
            return;
        }
    }

    this->all_entities.insert(std::pair(entity_hash, entity));

    if(entity->is_renderable()) {
        this->renderable_entities.push_back(entity);
    }

    if(entity->is_tickable()) {
        this->tickable_entities.push_back(entity);
    }
}

void Scene::deregister_entity(se::Entity* entity) {
    uint8_t entity_hash = util::hash::ejenkins("%s", entity->get_name());
    for(auto i : this->internally_loaded) {
        if(i == entity) {
            WARN("Attempted to deregister an internally managed entity");
            return;
        }
    }
    // Delete from all entities list
    auto iter = this->all_entities.find(entity_hash);
    if(iter != this->all_entities.end()) {
        this->all_entities.erase(iter);
    }
    // Delete from other lists
    for(size_t i = 0; i < this->renderable_entities.size(); i++) {
        if(this->renderable_entities[i] == entity) {
            this->renderable_entities.erase(this->renderable_entities.begin() + i);
        }
    }
    for(size_t i = 0; i < this->tickable_entities.size(); i++) {
        if(this->tickable_entities[i] == entity) {
            this->tickable_entities.erase(this->tickable_entities.begin() + i);
        }
    }
}

void Scene::register_constructor(const char* type, WrappedEntityConstructor constructor) {
    uint32_t hash = util::hash::ejenkins("%s", type);
    auto search = this->constructors.find(hash);
    if(search != this->constructors.end()) {
        WARN("Attempted to register duplicate constructor for type [%s]", type);
        return;
    }
    this->constructors.insert(std::pair(hash, constructor));
}

se::Entity* Scene::get_entity(const char* name) {
    uint32_t hash = util::hash::ejenkins("%s", name);
    DEBUG("Hash is [%#08X]", hash);
    auto find = this->all_entities.find(hash);
    if(find == this->all_entities.end()) {
        return nullptr;
    } else {
        return find->second;
    }
}