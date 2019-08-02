/*!
 *  @file src/se/input/inputController.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/input/inputController.hpp"

#include "se/engine.hpp"

#include "util/config.hpp"
#include "util/log.hpp"

#include <chrono>

using namespace se::input;

// =====================
// == PRIVATE MEMBERS ==
// =====================

void InputController::input_thread_main() {
    util::log::set_thread_name("INPUT");
    INFO("Hello from the input thread!");

    // Set up the ips cap
    int initial_ips_cap = this->engine->config->get_int("input.ips");
    this->target_ips_time = 1000000000 / initial_ips_cap;
    auto frame_start = std::chrono::system_clock::now();

    // Variables for benchmarking
    auto bm_input_start_time = std::chrono::system_clock::now();
    uint32_t bm_total_op_count = 0;
    uint64_t bm_total_op_time_ns = 0;
    uint32_t bm_late_ops = 0;

    // Main input loop
    while(this->engine->threads_run) {

        this->process_input();

        auto frame_end = std::chrono::system_clock::now();
        auto duration_std = frame_end - frame_start;
        uint64_t duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration_std).count();
        bm_total_op_time_ns += duration_ns;
        bm_total_op_count++;
        if(this->target_ips_time < 0) { continue; }
        auto wait_time = (this->target_ips_time - duration_ns);
        if(wait_time > 0) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(wait_time));
        } else {
            WARN("Input Lagging!");
            bm_late_ops++;
        }
        frame_start = frame_end + std::chrono::nanoseconds(wait_time);
    }

    // Calculate and print benchmarking values
    if(bm_total_op_count == 0) {
        WARN("No inputs processed! Skipping benchmarking");
    } else {
        float bm_total_op_time_ms = bm_total_op_time_ns / 1000000.0;
        uint64_t bm_total_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - bm_input_start_time).count();
        double bm_total_time_ms = bm_total_time_ns / 1000000.0;
        uint64_t bm_average_op_time_ns = bm_total_op_time_ns / bm_total_op_count;
        double bm_average_op_time_ms = bm_average_op_time_ns / 1000000.0;
        float bm_average_input_load = (((float) bm_total_op_time_ns) / (bm_total_time_ns) * 100);
        float bm_average_ips = bm_total_op_count / (bm_total_time_ms / 1000.0);\
        INFO("Total Input Loops: %u", bm_total_op_count);
        INFO("Time spent processing inputs: %uns (%.3fms)", bm_total_op_time_ns, bm_total_op_time_ms);
        INFO("Total time elapsed: %uns (%.3fms)", bm_total_time_ns, bm_total_time_ms);
        INFO("Average op time: %uns (%.3fms)", bm_average_op_time_ns, bm_average_op_time_ms);
        INFO("Average input load: %.2f%%", bm_average_input_load);
        INFO("Late Frames: %u", bm_late_ops);
        INFO("Average IPS: %.3f", bm_average_ips);
    }

    DEBUG("Input thread terminated");

}

void InputController::process_input() {
    SDL_Event input_event;
    SDL_PollEvent(&input_event);
    
    for(auto handler : this->handlers) {
        handler(input_event);
    }
}

// ====================
// == PUBLIC MEMBERS ==
// ====================

InputController::InputController(se::Engine* engine) {
    DEBUG("Initializing new input controller");
    this->engine = engine;

    // Start the input thread
    this->input_thread = std::thread(&InputController::input_thread_main, this);
}

InputController::~InputController() {

    if(this->input_thread.joinable()) {
        DEBUG("Waiting for input thread to exit");
        this->input_thread.join();
    }

}

void InputController::register_handler(InputHandler handler) {
    this->handlers.push_back(handler);
}