/*!
 *  @file src/se/graphics/graphicsController.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/graphicsController.hpp"

#include "se/engine.hpp"

#include "util/config.hpp"
#include "util/log.hpp"

#include <chrono>
#include <typeinfo>

se::graphics::GraphicsController::GraphicsController(se::Engine* engine) {
    DEBUG("Initializing new graphics controller");
    this->engine = engine;

    // Start the graphics thread
    this->graphics_thread = std::thread(&se::graphics::GraphicsController::graphics_thread_main, this);
}

se::graphics::GraphicsController::~GraphicsController() {
    
    if(this->graphics_thread.joinable()) {
        DEBUG("Waiting for graphics thread to exit");
        this->graphics_thread.join();
    }

}

void se::graphics::GraphicsController::graphics_thread_main() {
    util::log::set_thread_name("RENDER");
    INFO("Hello from the render thread!");
    util::ConfigChangeHandler handler = CREATE_LOCAL_CHANGE_HANDLER(se::graphics::GraphicsController::recalculate_fps_limit);
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

    // Configure the OpenGL instance
    bool vsync = this->engine->config->get_bool("render.vsync",false);
    if(SDL_GL_SetSwapInterval(vsync ? 1 : 0) < 0) {
        ERROR("Failed to configure vsync [%s]", SDL_GetError());
    }
    // 58 111 166
    glClearColor(0.227, 0.434, 0.648, 1.0);
    
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

void se::graphics::GraphicsController::render() {

    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(this->window);

}

void se::graphics::GraphicsController::recalculate_fps_limit(util::ConfigurationValue* value, util::Configuration* config) {
    INFO("FPS limit changed to %i", value->int_);
    int fps_cap = value->int_;
    this->target_frame_time = 1000000000 / fps_cap;
}

void se::graphics::GraphicsController::process_tasks() {
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

void se::graphics::GraphicsController::submit_graphics_task(GraphicsTask task) {
    this->tasks.push(task);
}