/*!
 *  @file include/util/config.hpp
 * 
 *  This file defines the Configuration object and it's helper functions.
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _UTIL_CONFIG_H_
#define _UTIL_CONFIG_H_

#include <unordered_map>
#include <vector>
#include <functional>
#include <string>

#include "util/fwd.hpp"

#define CV_WRITE_LOCK_NONE 0
#define CV_WRITE_LOCK_NEXTWRITE 1
#define CV_WRITE_LOCK_LOCKED 2

/*!
 *  Create a local change handler.
 * 
 *  Because of the way that c++ is, it's basically impossible to pass a pointer
 *  to a member function anywhere.  The easiest way to get around this is to
 *  use the black magic known as `std::bind` to create a callable object with
 *  the object reference built in.  This macro takes the static name of a member
 *  function and automatically converts it to a static callable with the
 *  appropriate arguments needed to be treated as a `util::ConfigChangeHandler`.
 * 
 *  For example, to create a static configuration change handler that wraps the
 *  method `bar` in class `Foo` in namespace `baz`, you would do the following:
 * 
 *  ```c++
 *  util::ConfigChangeHandler handler = CREATE_LOCAL_CHANGE_HANDLER(baz::Foo::bar);
 *  ```
 */
#define CREATE_LOCAL_CHANGE_HANDLER(func) \
    (util::ConfigChangeHandler) \
    std::bind( \
        &func, this, std::placeholders::_1, std::placeholders::_2);

namespace util {

    /// Configuration change handler
    typedef std::function<void(util::ConfigurationValue*,util::Configuration*)> ConfigChangeHandler;

    /*!
     *  Configuration Value Container.
     * 
     *  Computers nowadays have plenty of ram, so keeping a copy of every value
     *  in a multitude of formats is a small price to pay.
     * 
     *  Configuration value containers are created once, and live in the same
     *  location for the entire lifespan of their parent Configuration object.
     *  Because of this, it is safe to perform a single lookup on a
     *  configuration and store the pointer for fast lookups at a later time.
     */
    class ConfigurationValue {

        private:

            /// Parent Configuration
            util::Configuration* parent;

            /// Change handlers
            std::vector<ConfigChangeHandler> change_handlers;

            /// Invoke change handlers
            void invoke_change_handlers();

            /*!
             *  Write Lock.
             * 
             *  The write lock prevents the value of this configuration value
             *  from being changed accidentally.
             * 
             *  **Warning: *This is not a secure lock*, it is intended to
             *  prevent accidental changes to values which are logically
             *  constant.**  For example, the silhouette engine value
             *  "window.dimx" is used to create frame buffers and texture, but
             *  no mechanism is or likely will be implemented to automatically
             *  re-initialize when the value is changed.  Because of this, there
             *  is no legitimate situation where a change to this value would
             *  lead to anything other than unexpected behavior.
             * 
             *  **Warning: *This is not a secure lock*, it is intended to
             *  prevent accidental changes to logically constant values.**
             * 
             *  For example, the silhouette engine value `window.dimx` is used
             *  to create the window and initialize the graphics pipeline.  At
             *  this time, there is no support for display re-sizing, and such
             *  changes to this value will be ignored, or worse, cause undefined
             *  behavior in other parts of the application.  To prevent such a
             *  situation, this value is write-locked on engine initialization.
             * 
             *  When attempting to write to a locked variable, a warning will
             *  be printed to the console, the value will remain unchanged, and
             *  `false` will be returned.
             * 
             *  The following values may be used:
             *  | Value                   | Description                        |
             *  |-------------------------|------------------------------------|
             *  |`CV_WRITE_LOCK_NONE`     | All writes allowed.                |
             *  |`CV_WRITE_LOCK_NEXTWRITE`| Unlocked, will lock on next `set()`|
             *  |`CV_WRITE_LOCK_LOCKED`   | All writes will fail.              |
             */
            uint8_t write_lock = 0;

        public:

            /// Reference Name
            const char* ref_name = nullptr;

            /// Value as a string
            volatile std::string string_;
            /// Value as a c string
            volatile char* cstring_ = nullptr;
            /// Value as an integer
            volatile int int_;
            /// Value as a double
            volatile double double_;
            /// Value as a boolean
            volatile bool bool_;

            /// Contains a valid boolean
            volatile bool valid_bool = false;

            /*!
             *  Update the configuration value from a string.
             * 
             *  If this value is locked, a warning will be generated and this
             *  method will return `false`.
             * 
             *  @param src  Input string
             * 
             *  @return `true` on success, `false` on failure.
             */
            bool set(const std::string src);
            /*!
             *  Update the configuration value from a c string.
             * 
             *  If this value is locked, a warning will be generated and this
             *  method will return `false`.
             * 
             *  @param src  Input c string
             * 
             *  @return `true` on success, `false` on failure.
             */
            bool set(const char* src);
            /*!
             *  Update the configuration value from an integer.
             * 
             *  If this value is locked, a warning will be generated and this
             *  method will return `false`.
             * 
             *  @param src  Input integer
             * 
             *  @return `true` on success, `false` on failure.
             */
            bool set(int src);
            /*!
             *  Update the configuration value from a double.
             * 
             *  If this value is locked, a warning will be generated and this
             *  method will return `false`.
             * 
             *  @param src  Input double
             * 
             *  @return `true` on success, `false` on failure.
             */
            bool set(double src);
            /*!
             *  Update the configuration value from a boolean.
             * 
             *  If this value is locked, a warning will be generated and this
             *  method will return `false`.
             * 
             *  @param src  Input bool
             * 
             *  @return `true` on success, `false` on failure.
             */
            bool set(bool src);

            /// Lock this configuration value
            void lock();
            /// Get the lock status
            int lock_status();

            /*!
             *  Add a change handler.
             */
            void add_change_handler(ConfigChangeHandler handler);

            /// Configuration value constructor
            ConfigurationValue(util::Configuration* parent, const char* ref_name = "<unnamed>");
            /// Configuration value destructor
            ~ConfigurationValue();

    };
    
    /*!
     *  Configuration Class.
     * 
     *  Configurations are dictionaries of keys and values.  Unlike a
     *  traditional hash map, ConfigurationValue objects provide a wide range
     *  of access methods.
     * 
     *  @include util/config.cpp
     */
    class Configuration {

        private:

            /*!
             *  Configuration Name.
             * 
             *  Each configuration should have a name associated with it to
             *  assist in the debugging of improperly formatted configuration
             *  entries.
             */
            const char* name;

        public:

            /*!
             *  Configuration Map.
             * 
             *  Entries are stored in pairs of uint32_t hashes generated with
             *  jenkins one-at-a-time hasing function, and the super-duper
             *  fancy-pantsy configuration container defined above.
             */
            std::unordered_map<uint32_t, ConfigurationValue*> config_values;

            /*!
             *  Create a new configuration.
             * 
             *  @param name Name Optional name to assign to the configuration
             *                   file for use in debugging and error messages.
             */
            Configuration(const char* name = "<unnamed>");

            /*!
             *  Destroy the configuration object.
             * 
             *  Releases all memory allocated for the configuration object and
             *  its contained values.
             */
            ~Configuration();

            /*!
             *  Parse configuration data.
             * 
             *  This method parses and loads the given raw configuration data.
             *  Improperly formatted entries will generate a warning, and be
             *  ignored.
             * 
             *  @param cfgdata  Configuration data to load
             * 
             *  @return The number of entries loaded.
             */
            int parse(const char* cfgdata);

            /*!
             *  Load a configuration from a file.
             * 
             *  This method reads data from the specified file, and passes it to
             *  the `parse()` method for loading.
             * 
             *  @param fname    Name of the file to parse
             * 
             *  @return The number of entities loaded, or a negative number if
             *          an error prevented the file from being read.
             */
            int load(const char* fname);

            /*!
             *  Get a configuration value.
             * 
             *  This method attempts to locate and return a pointer to the
             *  ConfigurationValue structure for the requested key.
             * 
             *  In the event that no value can be located for the given key,
             *  this method will return a null pointer.
             * 
             *  @param key  Key to be hashed and used for the lookup
             *  @param quiet    Suppress warnings.
             * 
             *  @return Pointer to the configuration value, or nullptr if it can
             *          not be located.
             */
            ConfigurationValue* get(const char* key, bool quiet = false);

            /*!
             *  Get a c string pointer.
             * 
             *  This method returns a pointer to the requested configuration
             *  value as a string.
             * 
             *  In the event that no value can be located for the given key,
             *  the optional `default_` parameter will be returned.
             * 
             *  @param key  Lookup key
             * 
             *  @return Pointer to the value, or `default_`
             */
            const volatile char** get_cstringp(const char* key, const char** default__ = nullptr);

            /*!
             *  Get an integer pointer.
             * 
             *  This method returns a pointer to the requested configuration
             *  value as an integer.
             * 
             *  In the event that no value can be located for the given key, the
             *  optional `default_` parameter will be returned.
             * 
             *  @param key  Lookup key
             * 
             *  @return Pointer to the value, or `default_`
             */
            const volatile int* get_intp(const char* key, int* default_ = nullptr);

            /*!
             *  Get a double pointer.
             * 
             *  This method returns a pointer to the requested configuration
             *  value as a double.
             * 
             *  In the even that no value can be located for the given key, the
             *  optional `default_` parameter will be returned.
             * 
             *  @param key  Lookup key
             * 
             *  @return Pointer to the value, or `default_`
             */
            const volatile double* get_doublep(const char* key, double* default_ = nullptr);

            /*!
             *  Get a boolean pointer.
             * 
             *  This method returns a pointer to the requested configuration
             *  value as a boolean.
             * 
             *  In the event that no value can be located for the given key, the
             *  optional `default_` parameter will be returned.
             * 
             *  In the event that a boolean is requested, but it is marked as
             *  invalid in the configuration value, a warning will be generated
             *  and the default value will be returned.
             * 
             *  @param key  Lookup key
             * 
             *  @return Pointer to the value, or `default_`
             */
            const volatile bool* get_boolp(const char* key, bool* default_ = nullptr);

            /*!
             *  Get a string (`std::string default_`).
             * 
             *  This method returns a `std::string` containing the requested
             *  configuration value.
             * 
             *  In the event that no value can be located for the given key, the
             *  optional `default_` parameter will be returned.
             * 
             *  @param key  Lookup key
             * 
             *  @return Value as a `std::string`, or `default_`
             */
            const std::string get_string(const char* key, std::string default_ = std::string(""));

            /*!
             *  Get a string (`char* default_`).
             * 
             *  This method returns a `std::string` containing the requested
             *  configuration value.
             * 
             *  In the event that no value can be located for the given key, the
             *  optional `default_` parameter will be returned.
             * 
             *  @param key  Lookup key
             * 
             *  @return Value as a `std::string`, or `default_`
             */
            const std::string get_string(const char* key, const char* default_ = "");

            /*!
             *  Get a c string.
             * 
             *  This method returns the requested configuration value as a
             *  string.
             * 
             *  In the event that no value can be located for the given key, the
             *  optional `default_` parameter will be returned.
             * 
             *  @param key  Lookup key
             * 
             *  @return The requested value, or `default_`
             */
            const char* get_cstring(const char* key, const char* default_ = nullptr);

            /*!
             *  Get an integer.
             * 
             *  This method returns the requested configuration value as an
             *  integer.
             * 
             *  In the event that no value can be located for the given key, the
             *  optional `default_` parameter will be returned.
             * 
             *  @param key  Lookup key
             * 
             *  @return The requested value, or `default_`
             */
            int get_int(const char* key, int default_ = 0);

            /*!
             *  Get a double.
             * 
             *  This method returns the requested configuration value as a
             *  double.
             * 
             *  In the event that no value can be located for the given key, the
             *  optional `default_` parameter will be returned.
             * 
             *  @param key  Lookup key
             * 
             *  @return The requested value, or `default_`
             */
            double get_double(const char* key, double default_ = 0.0);

            /*!
             *  Get a boolean.
             * 
             *  This method returns the requested configuration value as a
             *  boolean.
             * 
             *  In the event that no value can be located for the given key, the
             *  optional `default_` parameter will be returned.
             * 
             *  In the event that a boolean is requested, but it is marked as
             *  invalid in the configuration value, a warning will be generated
             *  and the default value will be returned.
             * 
             *  @param key  Lookup key
             * 
             *  @return The requested value, or `default_`
             */
            bool get_bool(const char* key, bool default_ = false);

            /*!
             *  Set a string.
             * 
             *  This method sets the specified configuration value to the given
             *  string.
             * 
             *  In the event that no value object can be located for the given
             *  key, a warning will be generated unless the create parameter is
             *  set to true.
             * 
             *  @param key      Lookup key
             *  @param value    Value to set
             * 
             *  @return `true` on insertion, `false` otherwise.
             */
            bool set(const char* key, const char* value, bool create = false);

            /*!
             *  Set an integer.
             * 
             *  This method sets the specified configuration value to the given
             *  integer.
             * 
             *  In the event that no value object can be located for the given
             *  key, a warning will be generated unless the create parameter is
             *  set to true.
             * 
             *  @param key      Lookup key
             *  @param value    Value to set
             * 
             *  @return `true` on insertion, `false` otherwise.
             */
            bool set(const char* key, int value, bool create = false);

            /*!
             *  Set a double.
             * 
             *  This method sets the specified configuration value to the given
             *  double.
             * 
             *  In the event that no value object can be located for the given
             *  key, a warning will be generated unless the create parameter is
             *  set to true.
             * 
             *  @param key      Lookup key
             *  @param value    Value to set
             * 
             *  @return `true` on insertion, `false` otherwise.
             */
            bool set(const char* key, double value, bool create = false);

            /*!
             *  Set a boolean.
             * 
             *  This method sets the specified configuration value to the given
             *  boolean.
             * 
             *  In the event that no value object can be located for the given
             *  key, a warning will be generated unless the create parameter is
             *  set to true.
             * 
             *  @param key      Lookup key
             *  @param value    Value to set
             * 
             *  @return `true` on insertion, `false` otherwise.
             */
            bool set(const char* key, bool value, bool create = false);

    };

}

#endif