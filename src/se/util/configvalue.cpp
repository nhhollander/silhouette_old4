/*!
 *  @file src/se/util/configvalue.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/util/config.hpp"

#include <string.h>

#include "se/util/log.hpp"

// ==============================
// = CONSTRUCTOR AND DESTRUCTOR =
// ==============================

se::util::ConfigurationValue::ConfigurationValue(Configuration* parent, const char* ref_name) {
    this->parent = parent;
    // Save the reference name
    this->ref_name = (const char*) new char[strlen(ref_name) + 1];
    strcpy((char*) this->ref_name, ref_name);
}

se::util::ConfigurationValue::~ConfigurationValue() {
    delete[] this->cstring_;
    delete[] this->ref_name;
}

// ==================
// = PUBLIC MEMBERS =
// ==================

bool se::util::ConfigurationValue::set(const std::string src, uint64_t pk) {
    // Forward as a c string
    return this->set(src.c_str(), pk);
}

bool se::util::ConfigurationValue::set(const char* src, uint64_t pk) {
    // Check if this value is locked
    if(this->write_lock == CV_WRITE_LOCK_LOCKED) {
        if(this->passkey != 0) {
            if(this->passkey != pk) {
                WARN("Invalid passkey when writing to locked configuratrion value [%s]", this->ref_name);
                return false;
            }
        } else {
            WARN("Attempted to write to locked configuration value [%s]", this->ref_name);
            return false;
        }
    }
    // Allocate a new string
    delete[] this->cstring_;
    this->cstring_ = new char[strlen(src) + 1];
    strcpy((char*) this->cstring_, src);
    // Interpret as an integer
    this->int_ = atoi(src);
    // Interpret as a float
    this->float_ = (float) atof(src);
    // Interpret as a double
    this->double_ = atof(src);
    // Interpret as boolean
    if(strlen(src) == 1) {
        // Check the first letters
        switch(src[0]) {
            case 't':
            case 'T':
            case 'y':
            case 'Y':
                this->bool_ = true;
                this->valid_bool = true;
                break;
            case 'f':
            case 'F':
            case 'n':
            case 'N':
                this->bool_ = false;
                this->valid_bool = true;
                break;
            default:
                this->valid_bool = false;
        }
    } else {
        // Make the string lowercase
        int in_len = strlen(src);
        char lower[in_len + 1];
        lower[in_len] = '\0';
        for(int i = 0; i < in_len; i++) {
            lower[i] = tolower(src[i]);
        }
        // Compare strings
        if(strcmp(lower,"yes") == 0) {
            this->bool_ = true;
            this->valid_bool = true;
        } else if(strcmp(lower,"true") == 0) {
            this->bool_ = true;
            this->valid_bool = true;
        } else if(strcmp(lower,"no") == 0) {
            this->bool_ = false;
            this->valid_bool = true;
        } else if(strcmp(lower,"false") == 0) {
            this->bool_ = false;
            this->valid_bool = true;
        } else {
            this->valid_bool = false;
        }
    }
    // Invoke the change handlers
    this->invoke_change_handlers();
    return true;
}

bool se::util::ConfigurationValue::set(int src, uint64_t pk) {
    // Check if this value is locked
    if(this->write_lock == CV_WRITE_LOCK_LOCKED) {
        if(this->passkey != 0) {
            if(this->passkey != pk) {
                WARN("Invalid passkey when writing to locked configuratrion value [%s]", this->ref_name);
                return false;
            }
        } else {
            WARN("Attempted to write to locked configuration value [%s]", this->ref_name);
            return false;
        }
    }
    // Interpretation of integers is pretty darn easy
    delete[] this->cstring_;
    this->cstring_ = new char[32];
    snprintf((char*) this->cstring_, 32, "%i", src);
    // Interpret as integer
    this->int_ = src;
    // Interpret as float
    this->float_ = (float) src;
    // Interpret as double
    this->double_ = (double) src;
    // Interpret as boolean
    this->bool_ = src != 0;
    this->valid_bool = true;
    // Invoke the change handlers
    this->invoke_change_handlers();
    
    if(this->write_lock == CV_WRITE_LOCK_NEXTWRITE) {
        this->write_lock = CV_WRITE_LOCK_LOCKED;
    }
    return true;
}
bool se::util::ConfigurationValue::set(float src, uint64_t pk) {
    return this->set((double) src);
}
bool se::util::ConfigurationValue::set(double src, uint64_t pk) {
    // Check if this value is locked
    if(this->write_lock == CV_WRITE_LOCK_LOCKED) {
        if(this->passkey != 0) {
            if(this->passkey != pk) {
                WARN("Invalid passkey when writing to locked configuratrion value [%s]", this->ref_name);
                return false;
            }
        } else {
            WARN("Attempted to write to locked configuration value [%s]", this->ref_name);
            return false;
        }
    }
    // Interpretation of integers is pretty darn easy
    delete[] this->cstring_;
    this->cstring_ = new char[32];
    snprintf((char*) this->cstring_, 32, "%f", src);
    // Interpret as integer
    this->int_ = (int) src;
    // Interpret as a float
    this->float_ = (float) src;
    // Interpret as double
    this->double_ = src;
    // Interpret as boolean
    this->bool_ = src != 0;
    this->valid_bool = true;
    // Invoke the change handlers
    this->invoke_change_handlers();

    if(this->write_lock == CV_WRITE_LOCK_NEXTWRITE) {
        this->write_lock = CV_WRITE_LOCK_LOCKED;
    }
    return true;
}

bool se::util::ConfigurationValue::set(bool src, uint64_t pk) {
    // Check if this value is locked
    if(this->write_lock == CV_WRITE_LOCK_LOCKED) {
        if(this->passkey != 0) {
            if(this->passkey != pk) {
                WARN("Invalid passkey when writing to locked configuratrion value [%s]", this->ref_name);
                return false;
            }
        } else {
            WARN("Attempted to write to locked configuration value [%s]", this->ref_name);
            return false;
        }
    }
    delete[] this->cstring_;
    this->cstring_ = new char[6];
    if(src) {
        this->cstring_[0] = 't';
        this->cstring_[1] = 'r';
        this->cstring_[2] = 'u';
        this->cstring_[3] = 'e';
        this->cstring_[4] = '\0';
    } else {
        this->cstring_[0] = 'f';
        this->cstring_[1] = 'a';
        this->cstring_[2] = 'l';
        this->cstring_[3] = 's';
        this->cstring_[4] = 'e';
        this->cstring_[5] = '\0';
    }
    // Interpret as integer
    this->int_ = (int) src;
    // Interpret as float
    this->float_ = (float) src;
    // Interpret as double
    this->double_ = (double) src;
    // Interpret as boolean
    this->bool_ = src;
    this->valid_bool = true;
    // Invoke the change handlers
    this->invoke_change_handlers();
    
    if(this->write_lock == CV_WRITE_LOCK_NEXTWRITE) {
        this->write_lock = CV_WRITE_LOCK_LOCKED;
    }
    return true;
}

void se::util::ConfigurationValue::lock(uint64_t pk) {
    this->write_lock = CV_WRITE_LOCK_LOCKED;
    this->passkey = pk;
    DEBUG("Configuration value [%s] locked (passkey: %s)", this->ref_name,
        (pk == 0) ? "FALSE" : "TRUE");
}

int se::util::ConfigurationValue::lock_status() {
    return this->write_lock;
}

void se::util::ConfigurationValue::add_change_handler(ConfigChangeHandler handler) {
    this->change_handlers.push_back(handler);
}

// ===================
// = PRIVATE MEMBERS =
// ===================

void se::util::ConfigurationValue::invoke_change_handlers() {
    // Invoke those handlers
    for(auto handler : this->change_handlers) {
        handler(this, this->parent);
    }
}