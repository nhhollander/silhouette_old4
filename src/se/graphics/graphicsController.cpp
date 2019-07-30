/*!
 *  @file src/se/graphics/graphicsController.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/graphicsController.hpp"

#include "se/engine.hpp"
#include "se/entity.hpp"
#include "se/entity/camera.hpp"

#include "util/config.hpp"
#include "util/log.hpp"

#include <chrono>
#include <typeinfo>
#include <math.h>

using namespace se::graphics;

// =====================
// == PRIVATE MEMBERS ==
// =====================

#define SE_GL_DEBUG_TYPE(n) case(GL_DEBUG_TYPE_##n): \
    type_str = #n; break;
#define SE_GL_DEBUG_SEVERITY(n) case(GL_DEBUG_SEVERITY_##n): \
    severity_str = #n; break;
void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, const GLchar* message, const void* uparam) {

    const char* type_str;
    switch(type) {
        SE_GL_DEBUG_TYPE(DEPRECATED_BEHAVIOR)
        SE_GL_DEBUG_TYPE(ERROR);
        SE_GL_DEBUG_TYPE(MARKER)
        SE_GL_DEBUG_TYPE(OTHER)
        SE_GL_DEBUG_TYPE(PERFORMANCE)
        SE_GL_DEBUG_TYPE(POP_GROUP)
        SE_GL_DEBUG_TYPE(PORTABILITY)
        SE_GL_DEBUG_TYPE(PUSH_GROUP)
        SE_GL_DEBUG_TYPE(UNDEFINED_BEHAVIOR)
        default: type_str = "\033[1;31mINVALID\033[0m";
    }

    const char* severity_str;
    switch(severity) {
        SE_GL_DEBUG_SEVERITY(HIGH)
        SE_GL_DEBUG_SEVERITY(MEDIUM)
        SE_GL_DEBUG_SEVERITY(NOTIFICATION)
        SE_GL_DEBUG_SEVERITY(LOW)
        default: severity_str = "\033[1;31mINVALID\033[0m";
    }

    printf("[\033[1;38;5;93mOPENGL\033[0m] Type: [%s] Severity: [%s] ID: %u\n    %s\n",
        type_str, severity_str, id, message);
}

void GraphicsController::graphics_thread_main() {
    util::log::set_thread_name("RENDER");
    INFO("Hello from the render thread!");
    util::ConfigChangeHandler handler = CREATE_LOCAL_CHANGE_HANDLER(GraphicsController::recalculate_fps_limit);
    this->engine->config->get("render.fpscap")->add_change_handler(handler);

    // Initialize SDL2 in OpenGL mode
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        FATAL("Failed to initialize SDL2 [%s]", SDL_GetError());
        return;
    }

    // Configure the OpenGL version
    int gl_major = this->engine->config->get_int("render.gl.major", -1);
    int gl_minor = this->engine->config->get_int("render.gl.minor", -1);
    if(gl_major == -1 || gl_minor == -1) {
        FATAL("Opengl version missing! [%i.%i]", gl_major, gl_minor);
        return;
    }
    DEBUG("Using OpenGL %i.%i", gl_major, gl_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create the window
    const char* title = this->engine->config->get_cstring("window.title");
    int dimx = this->engine->config->get_int("window.dimx", -1);
    int dimy = this->engine->config->get_int("window.dimy", -1);
    if(title == nullptr || dimx == -1 || dimy == -1) {
        FATAL("Window properties missing! [%s:%i:%i]", title, dimx, dimy);
        return;
    }
    this->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dimx, dimy, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(this->window == nullptr) {
        FATAL("Failed to create SDL2 window [%s]", SDL_GetError());
        return;
    }

    // Create the OpenGL context
    this->glcontext = SDL_GL_CreateContext(this->window);
    if(this->glcontext == nullptr) {
        FATAL("Failed to create OpenGL context [%s]", SDL_GetError());
        return;
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if(glewError != GLEW_OK) {
        FATAL("Failed to initialize GLEW! [%s]", glewGetErrorString(glewError));
        return;
    }

    // Set up the error callback system
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_message_callback, this);

    // Configure the OpenGL instance
    bool vsync = this->engine->config->get_bool("render.vsync",false);
    if(SDL_GL_SetSwapInterval(vsync ? 1 : 0) < 0) {
        ERROR("Failed to configure vsync [%s]", SDL_GetError());
    }
    // 58 111 166
    glClearColor(0.227, 0.434, 0.648, 1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    INFO("Graphics initialization complete");

    // Set up the FPS cap
    int initial_fps_cap = this->engine->config->get_int("render.fpscap");
    this->target_frame_time = 1000000000 / initial_fps_cap;
    auto frame_start = std::chrono::system_clock::now();
    // Variables for benchmarking
    auto bm_render_start_time = std::chrono::system_clock::now();
    uint32_t bm_total_frame_count = 0;
    uint64_t bm_total_frame_time_ns = 0; // 2^64ns = ~585 years
    uint32_t bm_late_frames = 0;

    // Main render loop
    while(this->engine->threads_run) {

        frame_start = std::chrono::system_clock::now();

        this->process_tasks();

        this->render();

        auto duration_std = std::chrono::system_clock::now() - frame_start;
        int duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration_std).count();
        bm_total_frame_time_ns += duration_ns;
        bm_total_frame_count++;
        if(this->target_frame_time > 0) {  // Negative fps caps mean disabled
            auto wait_time = (target_frame_time - duration_ns);
            if(wait_time > 0) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(wait_time));
            } else {
                WARN("Dropping frames!");
                bm_late_frames++;
            }
        }
    }

    // Calculate and print benchmarking values
    if(bm_total_frame_count == 0) {
        WARN("No frames rendered! Benchmarking results are INVALID!");
        bm_total_frame_count++;
    }
    float bm_total_frame_time_ms = bm_total_frame_time_ns / 1000000.0;
    uint64_t bm_total_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - bm_render_start_time).count();
    double bm_total_time_ms = bm_total_time_ns / 1000000.0;
    uint64_t bm_average_frame_time_ns = bm_total_frame_time_ns / bm_total_frame_count;
    double bm_average_frame_time_ms = bm_average_frame_time_ns / 1000000.0;
    float bm_average_render_load = (((float) bm_total_frame_time_ns) / (bm_total_time_ns) * 100);
    float bm_average_fps = bm_total_frame_count / (bm_total_time_ms / 1000.0);
    INFO("Total Frames Rendered: %u", bm_total_frame_count);
    INFO("Time spent rendering: %uns (%.3fms)", bm_total_frame_time_ns, bm_total_frame_time_ms);
    INFO("Total time elapsed: %uns (%.3fms)", bm_total_time_ns, bm_total_time_ms);
    INFO("Average frame time: %uns (%.3fms)", bm_average_frame_time_ns, bm_average_frame_time_ms);
    INFO("Average render load: %.2f%%", bm_average_render_load);
    INFO("Late Frames: %u", bm_late_frames);
    INFO("Average FPS: %.3f", bm_average_fps);


    DEBUG("Render thread terminated");

}

void GraphicsController::render() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the VP matrix from the active camera
    glm::mat4 camera_matrix = this->active_camera->get_camera_matrix();

    for(se::Entity* entity : this->renderables) {
        
        entity->render(camera_matrix);

    }

    SDL_GL_SwapWindow(this->window);

}

void GraphicsController::graphics_support_thread_main() {
    util::log::set_thread_name("RSUPPORT");
    INFO("Hello from the graphics support thread");

    // Variables for sorting benchmarking
    uint64_t bm_sort_total_time = 0;
    uint32_t bm_sort_total_count = 0;
    uint64_t bm_sort_total_entity_count = 0;


    while(this->engine->threads_run) {

        // Debug code for calculating sort time
        auto bm_sort_start = std::chrono::system_clock::now();

        // Sort renderables
        this->sort_renderables();

        auto bm_sort_duration = std::chrono::system_clock::now() - bm_sort_start;
        uint64_t bm_sort_duration_ns = 
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                bm_sort_duration).count();
        float bm_sort_duration_ms = ((float) bm_sort_duration_ns) / 1000000;
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

    DEBUG("Render support thread terminated");
}

void GraphicsController::recalculate_fps_limit(util::ConfigurationValue* value, util::Configuration* config) {
    INFO("FPS limit changed to %i", value->int_);
    int fps_cap = value->int_;
    this->target_frame_time = 1000000000 / fps_cap;
}

void GraphicsController::process_tasks() {
    /* Tasks are executed one at a time to allow a frame to render in between.
     * While this could potentially increase the time it takes for all pending
     * tasks to complete, it prevents the graphics thread from becoming
     * frozen if there is a surge of jobs. */
    if(this->tasks.size() > 0) {
        GraphicsTask task = this->tasks.front();
        task();
        this->tasks.pop();
    }
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

void GraphicsController::sort_renderables() {
    // Wrap the sort comparison function in a lambda
    auto sort_fun = [this](se::Entity* e1, se::Entity* e2){
        return renderable_sort_check(e1, e2, this->active_camera);
    };

    std::sort(this->renderables.begin(), this->renderables.end(), sort_fun);
}

// ====================
// == PUBLIC MEMBERS ==
// ====================

GraphicsController::GraphicsController(se::Engine* engine) {
    DEBUG("Initializing new graphics controller");
    this->engine = engine;

    /* Create a default camera to use as the viewpoint until an actual camera is
    loaded.  This eliminates the requirement to check if the current camera is
    null before rendering each frame. */
    this->active_camera = new se::entity::Camera(this->engine);
    this->default_camera = this->active_camera;

    // Start the graphics thread
    this->graphics_thread = std::thread(&GraphicsController::graphics_thread_main, this);
    // Start the graphics support thread
    this->graphics_support_thread = std::thread(&GraphicsController::graphics_support_thread_main, this);
}

GraphicsController::~GraphicsController() {
    
    if(this->graphics_thread.joinable()) {
        DEBUG("Waiting for graphics thread to exit");
        this->graphics_thread.join();
    }

    if(this->graphics_support_thread.joinable()) {
        DEBUG("Wating for graphics support thread to exit");
        this->graphics_support_thread.join();
    }

    /* Probably should deal with at some point maybe?  I dunno. */
    //delete this->default_camera;

}

void GraphicsController::submit_graphics_task(GraphicsTask task) {
    this->tasks.push(task);
}

void GraphicsController::add_renderable(se::Entity* entity) {
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

void GraphicsController::remove_renderable(se::Entity* entity) {
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

void GraphicsController::set_active_camera(se::entity::Camera* camera) {
    this->active_camera = camera;
}

void GraphicsController::use_default_camera() {
    this->active_camera = this->default_camera;
}