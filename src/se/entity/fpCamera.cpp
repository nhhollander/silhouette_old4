/*!
 *  @file src/se/entity/fpCamera.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/entity/fpCamera.hpp"

#include "se/engine.hpp"
#include "se/input/inputController.hpp"

#include "se/util/log.hpp"
#include "se/util/debugstrings.hpp"

#include <SDL2/SDL.h>
#include <math.h>

using namespace se::entity;

// =====================
// == PRIVATE MEMBERS ==
// =====================

void FPCamera::sdl_event_handler(SDL_Event event) {
    // Check if begin or end mouse lock
    if(!this->camera_locked && event.type == SDL_MOUSEBUTTONDOWN) {
        this->lock_mouse();
    } else if(event.type == SDL_KEYDOWN) {
        if(event.key.keysym.sym == SDLK_ESCAPE) {
            if(this->camera_locked) {
                this->release_mouse();
            } else {
                // Mouse is unlocked and escape was pressed, time to terminate
                SDL_Event quit_event;
                quit_event.type = SDL_QUIT;
                SDL_PushEvent(&quit_event);
            }
        }
    }

    // Only process movement if the camera is locked
    if(!this->camera_locked) { return; }
    if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        if(event.key.keysym.sym == SDLK_w) {
            this->key_w = (event.type == SDL_KEYDOWN);
        } else if(event.key.keysym.sym == SDLK_s) {
            this->key_s = (event.type == SDL_KEYDOWN);
        } else if(event.key.keysym.sym == SDLK_a) {
            this->key_a = (event.type == SDL_KEYDOWN);
        } else if(event.key.keysym.sym == SDLK_d) {
            this->key_d = (event.type == SDL_KEYDOWN);
        }
    } else if(event.type == SDL_MOUSEMOTION) {
        int x = event.motion.xrel;
        int y = event.motion.yrel;
        this->rz -= (x / 600.0);
        this->rx -= (y / 600.0);
        if(this->rx > 1.5707) {
            this->rx = 1.5707;
        } else if(this->rx < -1.5707) {
            this->rx = -1.5707;
        }
        if(this->rz > 6.2831) {
            this->rz -= 6.2831;
        } else if(this->rz < 0) {
            this->rz += 6.2831;
        }
    }
}

// ====================
// == PUBLIC MEMBERS ==
// ====================

FPCamera::FPCamera(se::Engine* engine) : Camera(engine) {
    std::function handler = [this](SDL_Event event){this->sdl_event_handler(event);};
    engine->input_controller->register_handler(handler);
    engine->logic_controller->register_tickable(this);
}

FPCamera::~FPCamera() {}

void FPCamera::lock_mouse() {
    DEBUG("Locking mouse pointer");
    SDL_SetRelativeMouseMode(SDL_TRUE);
    this->camera_locked = true;
}

void FPCamera::release_mouse() {
    DEBUG("Releasing mouse pointer");
    SDL_SetRelativeMouseMode(SDL_FALSE);
    this->camera_locked = false;
}

void FPCamera::tick(uint64_t clock, uint32_t cdelta) {
    /* THIS IS A REALLY HACKY SOLUTION THAT SHOULD NOT BE DONE THIS WAY!  IT
    COMPLETELY IGNORES ENGINE TIME SCALING, WHICH IS LIKE SUPER ULTRA MEGA BAD. */
    float move_angle = this->rz;
    //if((this->key_a && !this->key_d && this->key_w) || (!this->key_a && this->key_d && this->key_s)) {
    //    move_angle += 3.14159 / 4;
    //} else if((!this->key_a && this->key_d && this->key_w) || (this->key_a && !this->key_d && this->key_s)) {
    //    move_angle -= 3.14159 / 4;
    //}

    if(this->key_w) {
        if(this->key_a) {
            move_angle += 3.14159 / 4;
        } else if(this->key_d) {
            move_angle -= 3.14159 / 4;
        }
    } else if(this->key_s) {
        if(this->key_a) {
            move_angle += (3.14159 / 4) * 3;
        } else if(this->key_d) {
            move_angle -= (3.14159 / 4) * 3;
        } else {
            move_angle += 3.14159;
        }
    } else if(this->key_a && !this->key_d) {
        move_angle += 3.14159 / 2;
    } else if(this->key_d && !this->key_a) {
        move_angle -= 3.14159 / 2;
    } else {
        return;
    }

    float dx = sin(move_angle) * .05;
    float dy = cos(move_angle) * .05;
    this->x -= dx;
    this->y += dy;
}