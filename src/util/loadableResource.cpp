/*!
 *  @file src/util/loadableReource.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "util/loadableResource.hpp"

#include "util/log.hpp"

using namespace util;

const char* util::loadable_resource_state_name(LoadableResourceState state) {
    switch(state) {
        case LoadableResourceState::NOT_LOADED:  return "NOT_LOADED";
        case LoadableResourceState::LOADING:     return "LOADING";
        case LoadableResourceState::LOADED:      return "LOADED";
        case LoadableResourceState::ERROR:       return "ERROR";
        case LoadableResourceState::CHILD_ERROR: return "CHILD_ERROR";
        default: return "<Invalid LoadableResourceState>";
    }
}

void LoadableResource::increment_resource_user_counter() {
    this->resource_user_counter_mutex.lock();
    this->resource_user_counter++;
    if(this->resource_user_counter == 1) {
        this->load_();
    }
    this->resource_user_counter_mutex.unlock();
}

void LoadableResource::decrement_resource_user_counter() {
    this->resource_user_counter_mutex.lock();
    if(this->resource_user_counter == 0) {
        WARN("Attempted to decrement user counter below zero!");
    } else {
        this->resource_user_counter--;
        if(this->resource_user_counter == 0) {
            this->unload_();
        }
    }
    this->resource_user_counter_mutex.unlock();
}

LoadableResourceState LoadableResource::get_resource_state() {
    return this->resource_state;
}

void LoadableResource::reload() {
    this->unload_();
    this->load_();
}