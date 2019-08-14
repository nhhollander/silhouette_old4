/*!
 *  @file src/se/logic/logicController.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/logic/logicController.hpp"

#include "se/engine.hpp"

#include "util/log.hpp"
#include "util/config.hpp"

#include <chrono>

using namespace se::logic;

// =====================
// == PRIVATE MEMBERS ==
// =====================

void LogicController::logic_thread_main() {
    util::log::set_thread_name("LOGIC");
    INFO("Hello from the logic thread!");

    // Set up the TPS cap
    int initial_tps_cap = this->engine->config->get_int("logic.tps");
    this->target_tick_time = 1000000000 / initial_tps_cap;
    DEBUG("Target Tick Time: %u", this->target_tick_time);
    auto tick_start = std::chrono::system_clock::now();
    // Variable for benchmarking
    auto bm_logic_start_time = std::chrono::system_clock::now();
    uint32_t bm_total_tick_count = 0;
    uint64_t bm_total_tick_time_ns = 0;
    uint32_t bm_late_ticks = 0;
    uint64_t bm_tickables_tocked = 0;

    // Main logic loop
    while(this->engine->threads_run) {

        // Update the scaled clock
        uint32_t cdelta = (this->target_tick_time * *this->time_scale) / 1000000;
        this->scaled_clock += cdelta;

        for(auto tickable : this->tickables) {
            tickable->tick(this->scaled_clock, cdelta);
        }

        auto tick_end = std::chrono::system_clock::now();
        auto duration_std = tick_end - tick_start;
        uint64_t duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration_std).count();
        bm_total_tick_time_ns += duration_ns;
        bm_total_tick_count++;
        bm_tickables_tocked += this->tickables.size();
        auto wait_time = (this->target_tick_time - duration_ns);
        if(wait_time > 0) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(wait_time));
        } else {
            float ms = -(wait_time / 1000000.0);
            WARN("Tick took %.3fms too long!", ms);
            bm_late_ticks++;
        }
        tick_start = tick_end + std::chrono::nanoseconds(wait_time);

    }

    // Calculate and print benchmarking values
    if(bm_total_tick_count == 0) {
        WARN("No ticks tocked!  Skipping benchmarking");
    } else if(bm_tickables_tocked == 0) {
        WARN("No tickables tocked!  Skipping behcnmarking");
    }else {
        float bm_total_tick_time_ms = bm_total_tick_time_ns / 1000000.0;
        uint64_t bm_total_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - bm_logic_start_time).count();
        double bm_total_time_ms = bm_total_time_ns / 1000000.0;
        uint64_t bm_average_tick_time_ns = bm_total_tick_time_ns / bm_total_tick_count;
        double bm_average_tick_time_ms = bm_average_tick_time_ns / 1000000.0;
        float bm_average_tick_load = (((float) bm_total_tick_time_ns) / (bm_total_time_ns) * 100);
        float bm_average_tps = bm_total_tick_count / (bm_total_time_ms / 1000.0);
        uint64_t bm_average_time_per_tickable_ns = bm_total_time_ns / bm_tickables_tocked;
        float bm_average_time_per_tickable_ms = bm_average_time_per_tickable_ns / 1000000.0;
        INFO("Total ticks tocked: %u", bm_total_tick_count);
        INFO("Time spent ticking: %uns (%.3fms)", bm_total_tick_time_ns, bm_total_tick_time_ms);
        INFO("Time time elapsed: %uns (%.3fms)", bm_total_time_ns, bm_total_time_ms);
        INFO("Average tick time: %uns (%.3fms)", bm_average_tick_time_ns, bm_average_tick_time_ms);
        INFO("Average logic load: %.2f%%", bm_average_tick_load);
        INFO("Average time per tickable: %uns (%.3fms)", bm_average_time_per_tickable_ns, bm_average_time_per_tickable_ms);
        INFO("Late Ticks: %u", bm_late_ticks);
        INFO("Average TPS: %.3f", bm_average_tps);
    }

    DEBUG("Logic thread terminated");
}

// ====================
// == PUBLIC MEMBERS ==
// ====================

LogicController::LogicController(se::Engine* engine) {
    DEBUG("Initializing new logic controller");
    this->engine = engine;

    this->time_scale = (volatile double*) engine->config->get_doublep("logic.scale");

    this->logic_thread = std::thread(&LogicController::logic_thread_main, this);
}

LogicController::~LogicController() {

    if(this->logic_thread.joinable()) {
        DEBUG("Waiting for logic thread to exit");
        this->logic_thread.join();
    }

}

void LogicController::register_tickable(Tickable* tickable) {
    for(auto i : this->tickables) {
        if(i == tickable) {
            WARN("Attemped to register duplicate tickable!");
            return;
        }
    }
    this->tickables.push_back(tickable);
}

void LogicController::deregister_tickable(Tickable* tickable) {
    for(size_t i = 0; i < this->tickables.size(); i++) {
        if(this->tickables[i] == tickable) {
            this->tickables.erase(this->tickables.begin() + i);
            return;
        }
    }
    WARN("Attemped to deregister nonexistant tickable!");
}
