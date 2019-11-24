/*!
 *  @file include/util/debugstrings.hpp
 * 
 *  Copyright 2018 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "util/debugstrings.hpp"

#include <GL/glew.h>
#include <SDL2/SDL_events.h>
#include <png.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define CASE(c) case(c): return #c;

const char* util::string::gl_shader_type(unsigned int shader) {
    switch(shader) {
        CASE(GL_VERTEX_SHADER)
        CASE(GL_FRAGMENT_SHADER)
        CASE(GL_GEOMETRY_SHADER)
        default: return "<invalid shader type>";
    }
}

const char* util::string::gl_error_name(unsigned int code) {
    switch(code) {
        CASE(GL_NO_ERROR)
        CASE(GL_INVALID_ENUM)
        CASE(GL_INVALID_VALUE)
        CASE(GL_INVALID_OPERATION)
        CASE(GL_INVALID_FRAMEBUFFER_OPERATION)
        CASE(GL_OUT_OF_MEMORY)
        CASE(GL_STACK_UNDERFLOW)
        CASE(GL_STACK_OVERFLOW)
        default: return "<invalid error code>";
    }
}

const char* util::string::gl_error_desc(unsigned int code) {
    switch(code) {
        case(GL_NO_ERROR): return "No error has been recorded. The value of this symbolic constant is guaranteed to be 0.";
        case(GL_INVALID_ENUM): return "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.";
        case(GL_INVALID_VALUE): return "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.";
        case(GL_INVALID_OPERATION): return "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.";
        case(GL_INVALID_FRAMEBUFFER_OPERATION): return "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.";
        case(GL_OUT_OF_MEMORY): return "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
        case(GL_STACK_UNDERFLOW): return "An attempt has been made to perform an operation that would cause an internal stack to underflow.";
        case(GL_STACK_OVERFLOW): return "An attempt has been made to perform an operation that would cause an internal stack to overflow. ";
        default: return "<invalid error code>";
    }
}

const char* util::string::gl_type_name(GLenum input) {
    switch(input) {
        CASE(GL_FLOAT)
        CASE(GL_FLOAT_VEC2)
        CASE(GL_FLOAT_VEC3)
        CASE(GL_FLOAT_VEC4)
        CASE(GL_FLOAT_MAT2)
        CASE(GL_FLOAT_MAT3)
        CASE(GL_FLOAT_MAT4)
        CASE(GL_FLOAT_MAT2x3)
        CASE(GL_FLOAT_MAT2x4)
        CASE(GL_FLOAT_MAT3x2)
        CASE(GL_FLOAT_MAT3x4)
        CASE(GL_FLOAT_MAT4x2)
        CASE(GL_FLOAT_MAT4x3)
        CASE(GL_INT)
        CASE(GL_INT_VEC2)
        CASE(GL_INT_VEC3)
        CASE(GL_INT_VEC4)
        CASE(GL_UNSIGNED_INT)
        CASE(GL_UNSIGNED_INT_VEC2)
        CASE(GL_UNSIGNED_INT_VEC3)
        CASE(GL_UNSIGNED_INT_VEC4)
        CASE(GL_DOUBLE)
        CASE(GL_DOUBLE_VEC2)
        CASE(GL_DOUBLE_VEC3)
        CASE(GL_DOUBLE_VEC4)
        CASE(GL_DOUBLE_MAT2)
        CASE(GL_DOUBLE_MAT3)
        CASE(GL_DOUBLE_MAT4)
        CASE(GL_DOUBLE_MAT2x3)
        CASE(GL_DOUBLE_MAT2x4)
        CASE(GL_DOUBLE_MAT3x2)
        CASE(GL_DOUBLE_MAT3x4)
        CASE(GL_DOUBLE_MAT4x2)
        CASE(GL_DOUBLE_MAT4x3)
        default: return "<invalid GLenum>";
    }
}

const char* util::string::gl_tex_type_name(unsigned int type) {
    switch(type) {
        CASE(GL_TEXTURE_2D)
        CASE(GL_PROXY_TEXTURE_2D)
        CASE(GL_TEXTURE_1D_ARRAY)
        CASE(GL_PROXY_TEXTURE_1D_ARRAY)
        CASE(GL_TEXTURE_RECTANGLE)
        CASE(GL_PROXY_TEXTURE_RECTANGLE)
        CASE(GL_TEXTURE_CUBE_MAP_POSITIVE_X)
        CASE(GL_TEXTURE_CUBE_MAP_NEGATIVE_X)
        CASE(GL_TEXTURE_CUBE_MAP_POSITIVE_Y)
        CASE(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y)
        CASE(GL_TEXTURE_CUBE_MAP_POSITIVE_Z)
        CASE(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
        CASE(GL_PROXY_TEXTURE_CUBE_MAP)
        default: return "<invalid texture type>";
    }
}

const char* util::string::sdl_event_type_name(unsigned int type) {
    switch(type) {
        CASE(SDL_FIRSTEVENT)
        CASE(SDL_QUIT)
        CASE(SDL_APP_TERMINATING)
        CASE(SDL_APP_LOWMEMORY)
        CASE(SDL_APP_WILLENTERBACKGROUND)
        CASE(SDL_APP_DIDENTERBACKGROUND)
        CASE(SDL_APP_WILLENTERFOREGROUND)
        CASE(SDL_APP_DIDENTERFOREGROUND)
        CASE(SDL_WINDOWEVENT)
        CASE(SDL_SYSWMEVENT)
        CASE(SDL_KEYDOWN)
        CASE(SDL_KEYUP)
        CASE(SDL_TEXTEDITING)
        CASE(SDL_TEXTINPUT)
        CASE(SDL_KEYMAPCHANGED)
        CASE(SDL_MOUSEMOTION)
        CASE(SDL_MOUSEBUTTONDOWN)
        CASE(SDL_MOUSEBUTTONUP)
        CASE(SDL_MOUSEWHEEL)
        CASE(SDL_JOYAXISMOTION)
        CASE(SDL_JOYBALLMOTION)
        CASE(SDL_JOYHATMOTION)
        CASE(SDL_JOYBUTTONDOWN)
        CASE(SDL_JOYBUTTONUP)
        CASE(SDL_JOYDEVICEADDED)
        CASE(SDL_JOYDEVICEREMOVED)
        CASE(SDL_CONTROLLERAXISMOTION)
        CASE(SDL_CONTROLLERBUTTONDOWN)
        CASE(SDL_CONTROLLERBUTTONUP)
        CASE(SDL_CONTROLLERDEVICEADDED)
        CASE(SDL_CONTROLLERDEVICEREMOVED)
        CASE(SDL_CONTROLLERDEVICEREMAPPED)
        CASE(SDL_FINGERDOWN)
        CASE(SDL_FINGERUP)
        CASE(SDL_FINGERMOTION)
        CASE(SDL_DOLLARGESTURE)
        CASE(SDL_DOLLARRECORD)
        CASE(SDL_MULTIGESTURE)
        CASE(SDL_CLIPBOARDUPDATE)
        CASE(SDL_DROPFILE)
        CASE(SDL_DROPTEXT)
        CASE(SDL_DROPBEGIN)
        CASE(SDL_DROPCOMPLETE)
        CASE(SDL_AUDIODEVICEADDED)
        CASE(SDL_AUDIODEVICEREMOVED)
        CASE(SDL_RENDER_TARGETS_RESET)
        CASE(SDL_RENDER_DEVICE_RESET)
        CASE(SDL_USEREVENT)
        CASE(SDL_LASTEVENT)
        default: return "<invalid SDL_Event>";
    }
}

const char* util::string::sdl_event_type_description(unsigned int type) {
    switch(type) {
        case(SDL_FIRSTEVENT): return "do not remove (unused)";
        case(SDL_QUIT): return "use-requested quit";
        case(SDL_APP_TERMINATING): return "OS is terminating the application";
        case(SDL_APP_LOWMEMORY): return "OS is low on memory; free some";
        case(SDL_APP_WILLENTERBACKGROUND): return "application is entering background";
        case(SDL_APP_DIDENTERBACKGROUND): return "application entered background";
        case(SDL_APP_WILLENTERFOREGROUND): return "application is entering foreground";
        case(SDL_APP_DIDENTERFOREGROUND): return "application entered foreground";
        case(SDL_WINDOWEVENT): return "window state change";
        case(SDL_SYSWMEVENT): return "system specific event";
        case(SDL_KEYDOWN): return "key pressed";
        case(SDL_KEYUP): return "key released";
        case(SDL_TEXTEDITING): return "keyboard text editing (composition)";
        case(SDL_TEXTINPUT): return "keyboard text input";
        case(SDL_KEYMAPCHANGED): return "keymap changed due to a system event such as an input language or keyboard layout change";
        case(SDL_MOUSEMOTION): return "mouse moved";
        case(SDL_MOUSEBUTTONDOWN): return "mouse button pressed";
        case(SDL_MOUSEBUTTONUP): return "mouse button released";
        case(SDL_MOUSEWHEEL): return "mouse wheel motion";
        case(SDL_JOYAXISMOTION): return "joystick axis motion";
        case(SDL_JOYBALLMOTION): return "joystick trackball motion";
        case(SDL_JOYHATMOTION): return "joystick hat position change";
        case(SDL_JOYBUTTONDOWN): return "joystick button pressed";
        case(SDL_JOYBUTTONUP): return "joystick button released";
        case(SDL_JOYDEVICEADDED): return "joystick connected";
        case(SDL_JOYDEVICEREMOVED): return "joystick disconnected";
        case(SDL_CONTROLLERAXISMOTION): return "controller axis motion";
        case(SDL_CONTROLLERBUTTONDOWN): return "controller button pressed";
        case(SDL_CONTROLLERBUTTONUP): return "controller button released";
        case(SDL_CONTROLLERDEVICEADDED): return "controller connected";
        case(SDL_CONTROLLERDEVICEREMOVED): return "controller disconnected";
        case(SDL_CONTROLLERDEVICEREMAPPED): return "controller mapping updated";
        case(SDL_FINGERDOWN): return "user has touched input device";
        case(SDL_FINGERUP): return "user has stopped touching input device";
        case(SDL_FINGERMOTION): return "user is dragging finger on input device";
        case(SDL_DOLLARGESTURE): return "<no description>";
        case(SDL_DOLLARRECORD): return "<no description>";
        case(SDL_MULTIGESTURE): return "<no description>";
        case(SDL_CLIPBOARDUPDATE): return "the clipboard changed";
        case(SDL_DROPFILE): return "the system requests a file open";
        case(SDL_DROPTEXT): return "text/plain drag-and-drop event";
        case(SDL_DROPBEGIN): return "a new set of drops is beginning";
        case(SDL_DROPCOMPLETE): return "current set of drops is now complete";
        case(SDL_AUDIODEVICEADDED): return "a new audio device is available";
        case(SDL_AUDIODEVICEREMOVED): return "an audio device has been removed";
        case(SDL_RENDER_TARGETS_RESET): return "the render targets have been reset and their contents need to be updated";
        case(SDL_RENDER_DEVICE_RESET): return "the device has been reset and all textures need to be recreated";
        case(SDL_USEREVENT): return "a user-specified event";
        case(SDL_LASTEVENT): return "only for bounding internal arrays";
        default: return "<invalid SDL_Event>";
    }
}

const char* util::string::libpng_color_type_name(unsigned int type) {
    switch(type) {
        CASE(PNG_COLOR_TYPE_GRAY)
        CASE(PNG_COLOR_TYPE_PALETTE)
        CASE(PNG_COLOR_TYPE_RGB)
        CASE(PNG_COLOR_TYPE_RGB_ALPHA)
        CASE(PNG_COLOR_TYPE_GRAY_ALPHA)
        //CASE(PNG_COLOR_TYPE_RGBA)  // Duplicate
        //CASE(PNG_COLOR_TYPE_GA)    // Duplicate
        default: return "<invalid color type>";
    }
}

const char* util::string::gl_framebuffer_status_name(unsigned int status) {
    switch(status) {
        CASE(GL_FRAMEBUFFER_COMPLETE)
        CASE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
        CASE(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
        CASE(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
        CASE(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
        CASE(GL_FRAMEBUFFER_UNSUPPORTED)
        CASE(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
        CASE(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
        case(0): return "<general grror>";
        default: return "<invalid status>";
    }
}

const char* util::string::gl_message_source(unsigned int source) {
    switch(source) {
        CASE(GL_DEBUG_SOURCE_API)
        CASE(GL_DEBUG_SOURCE_WINDOW_SYSTEM)
        CASE(GL_DEBUG_SOURCE_SHADER_COMPILER)
        CASE(GL_DEBUG_SOURCE_THIRD_PARTY)
        CASE(GL_DEBUG_SOURCE_APPLICATION)
        CASE(GL_DEBUG_SOURCE_OTHER)
        default: return "<invalid message source>";
    }
}

const char* util::string::gl_message_type(unsigned int type) {
    switch(type) {
        CASE(GL_DEBUG_TYPE_ERROR)
        CASE(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
        CASE(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
        CASE(GL_DEBUG_TYPE_PORTABILITY)
        CASE(GL_DEBUG_TYPE_PERFORMANCE)
        CASE(GL_DEBUG_TYPE_MARKER)
        CASE(GL_DEBUG_TYPE_PUSH_GROUP)
        CASE(GL_DEBUG_TYPE_POP_GROUP)
        CASE(GL_DEBUG_TYPE_OTHER)
        default: return "<invalid message type>";
    }
}

const char* util::string::gl_message_severity(unsigned int severity) {
    switch(severity) {
        CASE(GL_DEBUG_SEVERITY_HIGH)
        CASE(GL_DEBUG_SEVERITY_MEDIUM)
        CASE(GL_DEBUG_SEVERITY_LOW)
        CASE(GL_DEBUG_SEVERITY_NOTIFICATION)
        default: return "<invalid message severity>";
    }
}

const char* util::string::ft2_error_name(unsigned int code) {
    switch(code) {
        // Not an error
        CASE(FT_Err_Ok)
        // Generic Errors
        CASE(FT_Err_Cannot_Open_Resource)
        CASE(FT_Err_Unknown_File_Format)
        CASE(FT_Err_Invalid_File_Format)
        CASE(FT_Err_Invalid_Version)
        CASE(FT_Err_Lower_Module_Version)
        CASE(FT_Err_Invalid_Argument)
        CASE(FT_Err_Unimplemented_Feature)
        CASE(FT_Err_Invalid_Table)
        CASE(FT_Err_Invalid_Offset)
        CASE(FT_Err_Array_Too_Large)
        CASE(FT_Err_Missing_Module)
        CASE(FT_Err_Missing_Property)
        // Glyph/character errors
        CASE(FT_Err_Invalid_Glyph_Index)
        CASE(FT_Err_Invalid_Character_Code)
        CASE(FT_Err_Invalid_Glyph_Format)
        CASE(FT_Err_Cannot_Render_Glyph)
        CASE(FT_Err_Invalid_Outline)
        CASE(FT_Err_Invalid_Composite)
        CASE(FT_Err_Too_Many_Hints)
        CASE(FT_Err_Invalid_Pixel_Size)
        // Handle Errors
        CASE(FT_Err_Invalid_Handle)
        CASE(FT_Err_Invalid_Library_Handle)
        CASE(FT_Err_Invalid_Driver_Handle)
        CASE(FT_Err_Invalid_Face_Handle)
        CASE(FT_Err_Invalid_Size_Handle)
        CASE(FT_Err_Invalid_Slot_Handle)
        CASE(FT_Err_Invalid_CharMap_Handle)
        CASE(FT_Err_Invalid_Stream_Handle)
        // Driver Errros
        CASE(FT_Err_Too_Many_Drivers)
        CASE(FT_Err_Too_Many_Extensions)
        // Mamory Errors
        CASE(FT_Err_Out_Of_Memory)
        CASE(FT_Err_Unlisted_Object)
        // Stream Errors
        CASE(FT_Err_Cannot_Open_Stream)
        CASE(FT_Err_Invalid_Stream_Seek)
        CASE(FT_Err_Invalid_Stream_Skip)
        CASE(FT_Err_Invalid_Stream_Read)
        CASE(FT_Err_Invalid_Stream_Operation)
        CASE(FT_Err_Invalid_Frame_Operation)
        CASE(FT_Err_Nested_Frame_Access)
        CASE(FT_Err_Invalid_Frame_Read)
        // Raster Errors
        CASE(FT_Err_Raster_Uninitialized)
        CASE(FT_Err_Raster_Corrupted)
        CASE(FT_Err_Raster_Overflow)
        CASE(FT_Err_Raster_Negative_Height)
        // Cache Errors
        CASE(FT_Err_Too_Many_Caches)
        // TrueType and SFNT errors
        CASE(FT_Err_Invalid_Opcode)
        CASE(FT_Err_Too_Few_Arguments)
        CASE(FT_Err_Stack_Overflow)
        CASE(FT_Err_Code_Overflow)
        CASE(FT_Err_Bad_Argument)
        CASE(FT_Err_Divide_By_Zero)
        CASE(FT_Err_Invalid_Reference)
        CASE(FT_Err_Debug_OpCode)
        CASE(FT_Err_ENDF_In_Exec_Stream)
        CASE(FT_Err_Nested_DEFS)
        CASE(FT_Err_Invalid_CodeRange)
        CASE(FT_Err_Execution_Too_Long)
        CASE(FT_Err_Too_Many_Function_Defs)
        CASE(FT_Err_Too_Many_Instruction_Defs)
        CASE(FT_Err_Table_Missing)
        CASE(FT_Err_Horiz_Header_Missing)
        CASE(FT_Err_Locations_Missing)
        CASE(FT_Err_Name_Table_Missing)
        CASE(FT_Err_CMap_Table_Missing)
        CASE(FT_Err_Hmtx_Table_Missing)
        CASE(FT_Err_Post_Table_Missing)
        CASE(FT_Err_Invalid_Horiz_Metrics)
        CASE(FT_Err_Invalid_CharMap_Format)
        CASE(FT_Err_Invalid_PPem)
        CASE(FT_Err_Invalid_Vert_Metrics)
        CASE(FT_Err_Could_Not_Find_Context)
        CASE(FT_Err_Invalid_Post_Table_Format)
        CASE(FT_Err_Invalid_Post_Table)
        CASE(FT_Err_DEF_In_Glyf_Bytecode)
        CASE(FT_Err_Missing_Bitmap)
        // CFF, CID, and Type 1 errors (what the heck does that mean?)
        CASE(FT_Err_Syntax_Error)
        CASE(FT_Err_Stack_Underflow)
        CASE(FT_Err_Ignore)
        CASE(FT_Err_No_Unicode_Glyph_Name)
        CASE(FT_Err_Glyph_Too_Big)
        // BDF errors
        CASE(FT_Err_Missing_Startfont_Field)
        CASE(FT_Err_Missing_Font_Field)
        CASE(FT_Err_Missing_Size_Field)
        CASE(FT_Err_Missing_Fontboundingbox_Field)
        CASE(FT_Err_Missing_Chars_Field)
        CASE(FT_Err_Missing_Startchar_Field)
        CASE(FT_Err_Missing_Encoding_Field)
        CASE(FT_Err_Missing_Bbx_Field)
        CASE(FT_Err_Bbx_Too_Big)
        CASE(FT_Err_Corrupted_Font_Header)
        CASE(FT_Err_Corrupted_Font_Glyphs)
        default: return "<invalid error>";
    }
}