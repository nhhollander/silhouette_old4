/*!
 *  @file include/util/debugstrings.hpp
 * 
 *  Static functions for returning string representations of constants, as well
 *  as their descriptions.
 * 
 *  Copyright 2018 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _UTIL_DEBUGSTRINGS_H_
#define _UTIL_DEBUGSTRINGS_H_

namespace util::string {

    /*!
     *  Return the type of a GL_xxx_SHADER.
     * 
     *  @param shader   Shader constant
     *  
     *  @return Name of the shader, or "<invalid shader type>"
     */
    const char* gl_shader_type(unsigned int shader);

    /*!
     *  Return the name of an OpenGL error returned by `glGetError()`;
     * 
     *  @param code    Error code
     * 
     *  @return Name of the error, or "<invalid error code>"
     */
    const char* gl_error_name(unsigned int code);

    /*!
     *  Return the description of an OpenGL error returned by `glGetError()`
     * 
     *  @param code    Error code
     * 
     *  @return Description of the error, or "<invalid error code>"
     */
    const char* gl_error_desc(unsigned int code);

    /*!
     *  Return the name of an OpenGL variable type.
     * 
     *  @param input    Type
     * 
     *  @return Name of the type, or "<invalid gl type>"
     */
    const char* gl_type_name(unsigned int input);

    /*!
     *  Return the name of an SDL event type.
     * 
     *  @param type     Event type
     * 
     *  @return Name of the event, or "<invalid SDL_Event>"
     */
    const char* sdl_event_type_name(unsigned int type);

    /*!
     *  Return the description of an SDL event type.
     * 
     *  @param type     Event type
     * 
     *  @return Description of the event, or "<invalid SDL_Event>"
     */
    const char* sdl_event_type_description(unsigned int type);

    /*!
     *  Return the name of a libPNG color type.
     * 
     *  @param type     Color type
     * 
     *  @return Name of the color type, or "<invalid color type>"
     */
    const char* libpng_color_type_name(unsigned int type);

    /*!
     *  Return the name of an OpenGL framebuffer status.
     * 
     *  In the special case where there is an opengl error, a status value of 0
     *  may be returned.  This method treats that special case by returning
     *  "<general error>".
     *
     *  @param status   Status Value.
     * 
     *  @return Name of the status, or "<invalid status>".
     */
    const char* gl_framebuffer_status_name(unsigned int status);

    /*!
     *  Return the name of an OpenGL debug message source.
     * 
     *  @param source   OpenGL source.
     * 
     *  @return Name of the source, or "<invalid message source>".
     */
    const char* gl_message_source(unsigned int source);

    /*!
     *  Return the name of an OpenGL debug message type.
     * 
     *  @param type     OpenGL debug message type.
     * 
     *  @return Name of the type, or "<invalid message type>".
     */
    const char* gl_message_type(unsigned int type);

    /*!
     *  Return the name of an OpenGL debug message severity.
     * 
     *  @param severity OpenGL debug message severity.
     * 
     *  @return Name of the severity, or "<invalid message severity>".
     */
    const char* gl_message_severity(unsigned int severity);

}

#endif