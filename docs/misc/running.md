# Launching the Engine

This doucment contains information about launching the engine.  These options
apply to all applications which include the silhouette engine.

These variables are specified through environment variables because some of the
settings make fundamental changes to the engine, including changes which affect
the ability to load configuration files.  For example, the debug variable
`SE_DEBUG_APPDATA` changes where the configuration subsystem will look for
config files.

## Debug Launch Variables

The following variables can be used to change the behavior of the engine for
debugging purposes.

 * [`SE_DEBUG_USERDATA`](#`SE_DEBUG_USERDATA`)
 * [`SE_DEBUG_TEMPDATA`](#`SE_DEBUG_TEMPDATA`)
 * [`SE_DEBUG_CACHEDATA`](#`SE_DEBUG_CACHEDATA`)
 * [`SE_DEBUG_APPDATA`](#`SE_DEBUG_CACHEDATA`)

### `SE_DEBUG_USERDATA`

This variable overrides the user data directory.  The user data folder contains
data that is specific to a given user (in this case, the debug user).

### `SE_DEBUG_TEMPDATA`

The variable overrides the temporary data directory.  The temporary data folder
contains data that is not needed between launches.

### `SE_DEBUG_CACHEDATA`

This variable overrides the cache data directory.  The cache data folder
contains data that is reusable between launches, but not imperitive to engine
operation.

### `SE_DEBUG_APPDATA`

This variable overrides the application data direcotry.  The application data
folder contains data that is required for engine operation, and is generally
read-only.