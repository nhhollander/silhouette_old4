/*!
 *  @file src/se/graphics/simpleRenderManager.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/simpleRenderManager.hpp"

#include "se/entity/camera.hpp"
#include "se/graphics/screen.hpp"

#include "util/log.hpp"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <algorithm>
#include <math.h>

using namespace se::graphics;

// =====================
// == PRIVATE MEMBERS ==
// =====================

void SimpleRenderManager::support_thread_main() {

    util::log::set_thread_name("SRMSUP");
    INFO("Hello from the simple render manager support thread");

    // Variables for sorting benchmarking
    uint64_t bm_sort_total_time = 0;
    uint32_t bm_sort_total_count = 0;
    uint64_t bm_sort_total_entity_count = 0;

    while(this->run) {

        // Debug code for calculating sort time
        auto bm_sort_start = std::chrono::system_clock::now();

        this->sort_renderables();

        auto bm_sort_duration = std::chrono::system_clock::now() - bm_sort_start;
        uint64_t bm_sort_duration_ns = 
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                bm_sort_duration).count();
        //float bm_sort_duration_ms = ((float) bm_sort_duration_ns) / 1000000;
        //DEBUG("Sorted %i renderable entities in %.3fms (%uns)",
        //    this->renderables.size(), bm_sort_duration_ms, bm_sort_duration_ns);
        bm_sort_total_time += bm_sort_duration_ns;
        bm_sort_total_entity_count += this->renderables.size();
        bm_sort_total_count += 1;

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    // Benchmarking information for entity sorting operation
    if(bm_sort_total_count == 0 || bm_sort_total_entity_count == 0) {
        WARN("No entities were sorted - skipping sorting benchmarks");
    } else {
        uint64_t average_time = bm_sort_total_time / bm_sort_total_count;
        uint64_t average_entity_time = bm_sort_total_time / bm_sort_total_entity_count;
        float total_time_ms = bm_sort_total_time / 1000000.0;
        float average_time_ms = average_time / 1000000.0;
        float averate_entity_time_ms = average_entity_time / 1000000.0;
        INFO("Total sort operations: %u", bm_sort_total_count);
        INFO("Total entities sorted: %u", bm_sort_total_entity_count);
        INFO("Total time %.3fms (%uns)", total_time_ms, bm_sort_total_time);
        INFO("Average time %.3fms (%uns)", average_time_ms, average_time);
        INFO("Average entity time %.3fms (%uns)", averate_entity_time_ms, average_entity_time);
    }

    DEBUG("Simple render manager support thread terminated");
}

bool renderable_sort_check(se::Entity* e1, se::Entity* e2, se::entity::Camera* camera) {
    /* In order to sort renderable entities, we want to calculate their
    distances to/from the currently active camera. It might be worth revisiting
    this method in the future with a faster or more suitable algorithm. */
    float dist_e1 = 0.0;
    float dist_e2 = 0.0;
    { // Distance calculation for entity 1
        float dx = e1->x - camera->x;
        float dy = e1->y - camera->y;
        float dz = e1->z - camera->z;
        dx = dx*dx;
        dy = dy*dy;
        dz = dz*dz;
        dist_e1 = sqrt(dx + dy + dz);
    }
    { // Distance calculation for entity 2
        float dx = e2->x - camera->x;
        float dy = e2->y - camera->y;
        float dz = e2->z - camera->z;
        dx = dx*dx;
        dy = dy*dy;
        dz = dz*dz;
        dist_e2 = sqrt(dx + dy + dz);
    }

    return dist_e1 < dist_e2;
}

void SimpleRenderManager::sort_renderables() {
    auto sort_fun = [this](se::Entity* e1, se::Entity* e2){
        return renderable_sort_check(e1, e2, this->active_camera);
    };

    std::sort(this->renderables.begin(), this->renderables.end(), sort_fun);
}

// ====================
// == PUBLIC MEMBERS ==
// ====================

SimpleRenderManager::SimpleRenderManager(se::Engine* engine) {
    DEBUG("Initializing new SimpleRenderManager");
    this->engine = engine;
    this->active_camera = new se::entity::Camera(this->engine);
    this->default_camera = this->active_camera;

    this->support_thread = std::thread(&SimpleRenderManager::support_thread_main, this);

    this->screen = new se::graphics::Screen(engine);
}

SimpleRenderManager::~SimpleRenderManager() {
    this->run = false;
    if(this->support_thread.joinable()) {
        DEBUG("Waiting for the support thread to exit");
        this->support_thread.join();
    }

    //delete this->active_camera;
    delete this->screen;
}

void SimpleRenderManager::render_frame() {

    this->screen->activate_framebuffer();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 camera_matrix = this->active_camera->get_camera_matrix();

    glEnable(GL_DEPTH_TEST);

    for(auto entity : this->renderables) {
        entity->render(camera_matrix);
    }

    this->screen->render();
    

}

void SimpleRenderManager::add_renderable(se::Entity* entity) {
    if(!entity->is_renderable()) {
        WARN("Attempted to add non-renderable entity to render list!");
        return;
    }
    for(auto ent : this->renderables) {
        if(ent == entity) {
            WARN("Attempted to add duplicate entity to render list!");
            return;
        }
    }
    // Add the entity to the renderable list
    this->renderables.push_back(entity);
}

void SimpleRenderManager::remove_renderable(se::Entity* entity) {
    int counter = 0;
    for(auto ent : this->renderables) {
        if(ent == entity) {
            this->renderables.erase(this->renderables.begin() + counter);
            return;
        }
        counter++;
    }
    // TODO: Add some sort of identifying information
    WARN("Attempted to remove non-exisent entity from render list!");
}

void SimpleRenderManager::set_active_camera(se::entity::Camera* camera) {
    this->active_camera = camera;
}

void SimpleRenderManager::use_default_camera() {
    this->active_camera = this->default_camera;
}