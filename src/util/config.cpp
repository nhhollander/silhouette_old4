/*!
 *  @file src/util/config.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "util/config.hpp"

#include "util/log.hpp"
#include "util/hash.hpp"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <regex>
#include <mutex>

/// Value insertion mutex
std::mutex insertion_lock;

// ==============================
// = CONSTRUCTOR AND DESTRUCTOR =
// ==============================

util::Configuration::Configuration(const char* name) {

    this->name = name;

}

util::Configuration::~Configuration() {

    // Destroy all of the keys
    for(auto iterator = this->config_values.begin(); iterator != this->config_values.end(); iterator++) {
        delete iterator->second;
    }

}

// ===================
// = PRIVATE METHODS =
// ===================

// ==================
// = PUBLIC METHODS =
// ==================

int util::Configuration::parse(const char* config_data) {

    int entry_count = 0;

    std::regex expression("(.*?) ?= ?(.*)");
    std::smatch matches;

    std::istringstream input_stream(config_data);
    int line_number = 0;
    // Iterate through every line in the configuration data
    for(std::string line; std::getline(input_stream, line); line_number++) {
        if(line.length() == 0) {
            // Blank line
            continue;
        }
        if(line[0] == '#') {
            // This line is a comment
            continue;
        }
        // Match the line
        if(std::regex_search(line, matches, expression)) {
            // Get the key and the value.
            std::string key_str = matches[1].str();
            std::string val_str = matches[2].str();
            const char* key = key_str.c_str();
            const char* val = val_str.c_str();
            // Check if this is a locked entry
            bool lock = false;
            if(key_str[0] == '!') {
                lock = true;
                key = &key[1];
            }
            this->set(key, val, true);
            if(lock) {
                this->get(key)->lock();
            }
            entry_count++;
        } else {
            // Invalid line
            WARN("[%s] Invalid value pair at line [%i]: [%s]", this->name, line_number, line);
        }
    }

    DEBUG("Loaded [%i] configuration entries", entry_count);
    return entry_count;

}

int util::Configuration::load(const char* fname) {

    // Open the file
    FILE* cfile = fopen(fname, "r");
    if(cfile == nullptr) {
        ERROR("Failed to open config file [%s] [%i: %s]",
            fname, errno, strerror(errno));
        return -1;
    }
    // Determine the length of the file
    if(fseek(cfile, 0, SEEK_END) != 0) {
        ERROR("Failed to locate end of config file (seek) [%s] [%i: %s]",
            fname, errno, strerror(errno));
        return -2;
    }
    // Get the length
    size_t filesize = ftello(cfile);
    if(filesize < 0) {
        ERROR("Failed to locate end of config file (tell) [%s] [%i: %s]",
            fname, errno, strerror(errno));
        return -3;
    }
    // Allocate memory for the file data
    char* fdata = new char[filesize];
    // Return the the beginning of the file
    if(fseek(cfile, 0, SEEK_SET) != 0) {
        ERROR("Failed to return to beginning of config file [%s] [%i: %s]",
            fname, errno, strerror(errno));
        return -4;
    }
    // Read the data
    size_t read_count = fread(fdata, 1, filesize, cfile);
    if(read_count < filesize) {
        WARN("Read fewer bytes than expected from config file [%s] (expected %i got %i)",
            fname, filesize, read_count);
    }
    // Close the file
    if(fclose(cfile) != 0) {
        WARN("Failed to close config file after reading [%s] [%i: %s]",
            fname, errno, strerror(errno));
    }

    DEBUG("Loaded [%u] bytes of config data", filesize);

    // Pass the data to the parser
    int line_count = this->parse(fdata);

    delete[] fdata;

    return line_count;

}

bool util::Configuration::create_if_non_existant(const char* key) {
    insertion_lock.lock();

    bool result = false;
    uint32_t key_hash = util::hash::jenkins(key, strlen(key));
    auto lookup = this->config_values.find(key_hash);
    if(lookup == this->config_values.end()) {
        // Create the value
        DEBUG("Creating empty configuration value for key [%s]", key);
        util::ConfigurationValue* ncv = new util::ConfigurationValue(this, key);
        this->config_values.insert(std::pair(key_hash, ncv));
        result = true;
    }

    insertion_lock.unlock();
    
    return result;
}

util::ConfigurationValue* util::Configuration::get(const char* key, bool quiet) {
    uint32_t key_hash = util::hash::jenkins(key, strlen(key));
    auto lookup = this->config_values.find(key_hash);
    if(lookup == this->config_values.end()) {
        // Not found
        if(!quiet) {
            WARN("[%s] Requested key [%s] not found!", this->name, key);
        }
        return nullptr;
    }
    return lookup->second;
}

const volatile char** util::Configuration::get_cstringp(const char* key, const char** default_) {
    // Get the value
    const util::ConfigurationValue* cv = this->get(key);
    if(cv == nullptr) {
        return (const volatile char**) default_;
    }
    // Return the string
    return (const volatile char**) &cv->cstring_;
}

const volatile int* util::Configuration::get_intp(const char* key, int* default_) {
    // Get the value
    const util::ConfigurationValue* cv = this->get(key);
    if(cv == nullptr) {
        return (const volatile int*) default_;
    }
    // Return the integer
    return (const volatile int*) &cv->int_;
}

const volatile float* util::Configuration::get_floatp(const char* key, float* default_) {
    // Get the value
    const util::ConfigurationValue* cv = this->get(key);
    if(cv == nullptr) {
        return (const volatile float*) default_;
    }
    // Return the double
    return (const volatile float*) &cv->float_;}

const volatile double* util::Configuration::get_doublep(const char* key, double* default_) {
    // Get the value
    const util::ConfigurationValue* cv = this->get(key);
    if(cv == nullptr) {
        return (const volatile double*) default_;
    }
    // Return the double
    return (const volatile double*) &cv->double_;
}

const volatile bool* util::Configuration::get_boolp(const char* key, bool* default_) {
    // Get the value
    const util::ConfigurationValue* cv = this->get(key);
    if(cv == nullptr) {
        return (const volatile bool*) default_;
    }
    // Check for invalid boolean
    if(!cv->valid_bool) {
        WARN("[%s] Requesting invalid boolean* [%s]",
            this->name, key);
        return default_;
    }
    // Return the boolean
    return (const volatile bool*) &cv->bool_;
}

const std::string util::Configuration::get_string(const char* key, std::string default_) {
    // Forward to standard string getter
    return this->get_string(key, default_.c_str());
}

const std::string util::Configuration::get_string(const char* key, const char* default_) {
    // Get the value
    const util::ConfigurationValue* cv = this->get(key);
    if(cv == nullptr) {
        return std::string(default_);
    }
    // Return as a string
    return std::string((char*) cv->cstring_);
}

const char* util::Configuration::get_cstring(const char* key, const char* default_) {
    // Get the value
    const util::ConfigurationValue* cv = this->get(key);
    if(cv == nullptr) {
        return default_;
    }
    // Return the string
    return (const char*) cv->cstring_;
}

int util::Configuration::get_int(const char* key, int default_) {
    // Get the value
    const util::ConfigurationValue* cv = this->get(key);
    if(cv == nullptr) {
        return default_;
    }
    // Return the integer
    return (const volatile int) cv->int_;
}

float util::Configuration::get_float(const char* key, float default_) {
    // Get the value
    const util::ConfigurationValue* cv = this->get(key);
    if(cv == nullptr) {
        return default_;
    }
    // Return the double
    return (float) cv->float_;
}

double util::Configuration::get_double(const char* key, double default_) {
    // Get the value
    const util::ConfigurationValue* cv = this->get(key);
    if(cv == nullptr) {
        return default_;
    }
    // Return the double
    return (double) cv->double_;
}

bool util::Configuration::get_bool(const char* key, bool default_) {
    // Get the value
    const util::ConfigurationValue* cv = this->get(key);
    if(cv == nullptr) {
        return (bool) default_;
    }
    if(!cv->valid_bool) {
        WARN("[%s] Requesting invalid boolean [%s]",
            this->name, key);
        return (bool) default_;
    }
    // Return the boolean
    return (bool) cv->bool_;
}

bool util::Configuration::set(const char* key, const char* value, bool create, uint64_t pk) {
    if(create) {
        insertion_lock.lock();
    }

    // Locate the value
    util::ConfigurationValue* cv = (util::ConfigurationValue*) this->get(key, create);
    // Check if exists
    if(cv == nullptr) {
        // Check if create
        if(create) {
            util::ConfigurationValue* ncv = new util::ConfigurationValue(this, key);
            ncv->set(value);
            uint32_t key_hash = util::hash::jenkins(key, strlen(key));
            this->config_values.insert(std::pair(key_hash, ncv));
        } else {
            WARN("Attempted to set non-existant configuration value [%s] to [%s]",
                key, value);
            return false;
        }
    } else {
        // Set the value
        cv->set(value, pk);
    }

    if(create) {
        insertion_lock.unlock();
    }
    // Success
    return true;
}

bool util::Configuration::set(const char* key, int value, bool create, uint64_t pk) {
    if(create) {
        insertion_lock.lock();
    }

    // Locate the value
    util::ConfigurationValue* cv = (util::ConfigurationValue*) this->get(key, create);
    // Check if exists
    if(cv == nullptr) {
        // Check if create
        if(create) {
            util::ConfigurationValue* ncv = new util::ConfigurationValue(this, key);
            ncv->set(value);
            uint32_t key_hash = util::hash::jenkins(key, strlen(key));
            this->config_values.insert(std::pair(key_hash, ncv));
        } else {
            WARN("Attempted to set non-existant configuration value [%s] to [%i]",
                key, value);
            return false;
        }
    } else {
        // Set the value
        cv->set(value, pk);
    }

    if(create) {
        insertion_lock.unlock();
    }
    // Success
    return true;
}

bool util::Configuration::set(const char* key, double value, bool create, uint64_t pk) {
    if(create) {
        insertion_lock.lock();
    }

    // Locate the value
    util::ConfigurationValue* cv = (util::ConfigurationValue*) this->get(key, create);
    // Check if exists
    if(cv == nullptr) {
        // Check if create
        if(create) {
            util::ConfigurationValue* ncv = new util::ConfigurationValue(this, key);
            ncv->set(value);
            uint32_t key_hash = util::hash::jenkins(key, strlen(key));
            this->config_values.insert(std::pair(key_hash, ncv));
        } else {
            WARN("Attempted to set non-existant configuration value [%s] to [%f]",
                key, value);
            return false;
        }
    } else {
        // Set the value
        cv->set(value, pk);
    }

    if(create) {
        insertion_lock.unlock();
    }
    // Success
    return true;
}

bool util::Configuration::set(const char* key, bool value, bool create, uint64_t pk) {
    if(create) {
        insertion_lock.lock();
    }

    // Locate the value
    util::ConfigurationValue* cv = (util::ConfigurationValue*) this->get(key, create);
    // Check if exists
    if(cv == nullptr) {
        // Check if create
        if(create) {
            util::ConfigurationValue* ncv = new util::ConfigurationValue(this, key);
            ncv->set(value);
            uint32_t key_hash = util::hash::jenkins(key, strlen(key));
            this->config_values.insert(std::pair(key_hash, ncv));
        } else {
            WARN("Attempted to set non-existant configuration value [%s] to [%s]",
                key, value ? "true" : "false");
            return false;
        }
    } else {
        // Set the value
        cv->set(value, pk);
    }

    if(create) {
        insertion_lock.unlock();
    }
    // Success
    return true;
}