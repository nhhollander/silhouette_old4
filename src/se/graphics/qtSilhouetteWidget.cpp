/*!
 *  @file src/se/graphics/qtSilhouetteWidget.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifdef SE_ENABLE_QT_WIDGET_SUPPORT

#include "se/engine.hpp"
#include "se/graphics/graphicsController.hpp"
#include "se/graphics/qtSilhouetteWidget.hpp"

#include "util/config.hpp"
#include "util/log.hpp"

#include <QEvent>
#include <QKeyEvent>

/*!
 *  Ranomly generated passkey for permitting modifications to the special
 *  internal.gl.outputfbid variable.
*/
#define OUTPUT_FBID_PASSKEY 0xb2f131ae43c55c86

using namespace se::graphics;

// ====================
// == PUBLIC MEMBERS ==
// ====================

QTSilhouetteWidget::QTSilhouetteWidget(QWidget* qparent) :
    QOpenGLWidget(qparent) {
    this->setFocusPolicy(Qt::ClickFocus);
}

QTSilhouetteWidget::~QTSilhouetteWidget() {
}

void QTSilhouetteWidget::configure(se::Engine* engine) {
    this->engine = engine;
    // Lock output fbid variable
    engine->config->get("internal.gl.outputfbid")->lock(OUTPUT_FBID_PASSKEY);
    // Configure the surface
    int gl_major = engine->config->get_int("render.gl.major", 0);
    int gl_minor = engine->config->get_int("render.gl.minor", 0);
    if(gl_major == 0 && gl_minor == 0) {
        ERROR("OpenGL version not specified, program will now probably crash.");
        return;
    }
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(gl_major, gl_minor);
    format.setProfile(QSurfaceFormat::CoreProfile);
    this->setFormat(format);
}

// =======================
// == PROTECTED MEMBERS ==
// =======================

void QTSilhouetteWidget::initializeGL() {
    this->engine->graphics_controller->init_gl();
}

void QTSilhouetteWidget::paintGL() {
    this->engine->config->set(
        "internal.gl.outputfbid", (int) this->defaultFramebufferObject(),
        false, OUTPUT_FBID_PASSKEY);
    this->engine->graphics_controller->do_frame();

    // Check for pending tasks and initiate refresh
    if(this->engine->graphics_controller->pending_task_count() > 0) {
        this->update();
    }
}

void QTSilhouetteWidget::resizeGL(int width, int height) {
    this->engine->config->set("window.dimx", width);
    this->engine->config->set("window.dimy", height);
}

QSize QTSilhouetteWidget::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize QTSilhouetteWidget::sizeHint() const {
    return QSize(400, 400);
}

bool QTSilhouetteWidget::event(QEvent *event) {
    // Create the SDL event
    SDL_Event sdl_event;
    bool sdl_event_valid = true;
    bool redraw_necessary = true;

    // Translate the event type
    switch(event->type()) {
        //case QEvent::MouseButtonPress: sdl_event.type = SDL_MOUSEBUTTONDOWN; break;
        //case QEvent::MouseButtonRelease: sdl_event.type = SDL_MOUSEBUTTONUP; break;
        //case QEvent::MouseButtonDblClick: sdl_event.type = SDL_MOUSEBUTTONDOWN; break; // TODO: Correct?
        //case QEvent::MouseMove: sdl_event.type = SDL_MOUSEMOTION; break;
        case QEvent::KeyPress: sdl_event.type = SDL_KEYDOWN; break;
        case QEvent::KeyRelease: sdl_event.type = SDL_KEYUP; break;
        default: sdl_event_valid = false; redraw_necessary = false; break;
    }

    // Translate keys
    if(event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        QKeyEvent* key_event = static_cast<QKeyEvent*>(event);

        sdl_event.key.keysym.scancode = (SDL_Scancode) key_event->nativeScanCode();
        sdl_event.key.keysym.mod = key_event->modifiers();
        sdl_event.key.timestamp = key_event->timestamp();
        // TODO: window ID

        if(event->type() == QEvent::KeyPress) {
            sdl_event.key.state = SDL_PRESSED;
        } else {
            sdl_event.key.state = SDL_RELEASED;
        }

        int keycode = key_event->key();

        #define KEY(a,b) case a: sdl_event.key.keysym.sym = b; break;
        switch(keycode) {
            KEY(Qt::Key_Escape, SDLK_ESCAPE)
            KEY(Qt::Key_Tab, SDLK_TAB)
            //case Qt::Key_Backtab: no equal?
            KEY(Qt::Key_Backspace, SDLK_BACKSPACE)
            KEY(Qt::Key_Return, SDLK_RETURN)
            KEY(Qt::Key_Enter, SDLK_KP_ENTER)
            KEY(Qt::Key_Insert, SDLK_INSERT)
            KEY(Qt::Key_Delete, SDLK_DELETE)
            KEY(Qt::Key_Pause, SDLK_PAUSE)
            KEY(Qt::Key_Print, SDLK_PRINTSCREEN)
            KEY(Qt::Key_SysReq, SDLK_SYSREQ)
            KEY(Qt::Key_Clear, SDLK_CLEAR)
            KEY(Qt::Key_Home, SDLK_HOME)
            KEY(Qt::Key_End, SDLK_END)
            KEY(Qt::Key_Left, SDLK_LEFT)
            KEY(Qt::Key_Up, SDLK_UP)
            KEY(Qt::Key_Right, SDLK_RIGHT)
            KEY(Qt::Key_Down, SDLK_DOWN)
            KEY(Qt::Key_PageUp, SDLK_PAGEUP)
            KEY(Qt::Key_PageDown, SDLK_PAGEDOWN)
            KEY(Qt::Key_Shift, SDLK_LSHIFT)
            KEY(Qt::Key_Control, SDLK_LCTRL)
            // KEY(Qt::Key_Meta) no equal?
            KEY(Qt::Key_Alt, SDLK_LALT)
            //KEY(Qt::Key_AltGr) no equal?
            KEY(Qt::Key_CapsLock, SDLK_CAPSLOCK)
            KEY(Qt::Key_NumLock, SDLK_NUMLOCKCLEAR) // correct?
            KEY(Qt::Key_ScrollLock, SDLK_SCROLLLOCK)
            KEY(Qt::Key_F1, SDLK_F1)
            KEY(Qt::Key_F2, SDLK_F2)
            KEY(Qt::Key_F3, SDLK_F3)
            KEY(Qt::Key_F4, SDLK_F4)
            KEY(Qt::Key_F5, SDLK_F5)
            KEY(Qt::Key_F6, SDLK_F6)
            KEY(Qt::Key_F7, SDLK_F7)
            KEY(Qt::Key_F8, SDLK_F8)
            KEY(Qt::Key_F9, SDLK_F9)
            KEY(Qt::Key_F10, SDLK_F10)
            KEY(Qt::Key_F11, SDLK_F11)
            KEY(Qt::Key_F12, SDLK_F12)
            KEY(Qt::Key_F13, SDLK_F13)
            KEY(Qt::Key_F14, SDLK_F14)
            KEY(Qt::Key_F15, SDLK_F15)
            KEY(Qt::Key_F16, SDLK_F16)
            KEY(Qt::Key_F17, SDLK_F17)
            KEY(Qt::Key_F18, SDLK_F18)
            KEY(Qt::Key_F19, SDLK_F19)
            KEY(Qt::Key_F20, SDLK_F20)
            KEY(Qt::Key_F21, SDLK_F21)
            KEY(Qt::Key_F22, SDLK_F22)
            KEY(Qt::Key_F23, SDLK_F23)
            KEY(Qt::Key_F24, SDLK_F24)
            // SDL does not have key codes defined for F25-F35
            KEY(Qt::Key_Menu, SDLK_MENU)
            KEY(Qt::Key_Help, SDLK_HELP)
            // Keys, some of these are the same but otheres aren't
            KEY(Qt::Key_Space, SDLK_SPACE);
            KEY(Qt::Key_Exclam, SDLK_EXCLAIM);
            KEY(Qt::Key_QuoteDbl, SDLK_QUOTEDBL);
            KEY(Qt::Key_NumberSign, SDLK_HASH);
            KEY(Qt::Key_Dollar, SDLK_DOLLAR);
            KEY(Qt::Key_Percent, SDLK_PERCENT);
            KEY(Qt::Key_Ampersand, SDLK_AMPERSAND);
            KEY(Qt::Key_Apostrophe, SDLK_QUOTE);
            KEY(Qt::Key_ParenLeft, SDLK_LEFTPAREN);
            KEY(Qt::Key_ParenRight, SDLK_RIGHTPAREN);
            KEY(Qt::Key_Asterisk, SDLK_ASTERISK);
            KEY(Qt::Key_Plus, SDLK_PLUS);
            KEY(Qt::Key_Comma, SDLK_COMMA);
            KEY(Qt::Key_Minus, SDLK_MINUS);
            KEY(Qt::Key_Period, SDLK_PERIOD);
            KEY(Qt::Key_Slash, SDLK_SLASH);
            KEY(Qt::Key_0, SDLK_0);
            KEY(Qt::Key_1, SDLK_1);
            KEY(Qt::Key_2, SDLK_2);
            KEY(Qt::Key_3, SDLK_3);
            KEY(Qt::Key_4, SDLK_4);
            KEY(Qt::Key_5, SDLK_5);
            KEY(Qt::Key_6, SDLK_6);
            KEY(Qt::Key_7, SDLK_7);
            KEY(Qt::Key_8, SDLK_8);
            KEY(Qt::Key_9, SDLK_9);
            KEY(Qt::Key_Colon, SDLK_COLON);
            KEY(Qt::Key_Semicolon, SDLK_SEMICOLON);
            KEY(Qt::Key_Less, SDLK_LESS);
            KEY(Qt::Key_Greater, SDLK_GREATER);
            KEY(Qt::Key_Question, SDLK_QUESTION);
            KEY(Qt::Key_At, SDLK_AT);
            KEY(Qt::Key_A, SDLK_a);
            KEY(Qt::Key_B, SDLK_b);
            KEY(Qt::Key_C, SDLK_c);
            KEY(Qt::Key_D, SDLK_d);
            KEY(Qt::Key_E, SDLK_e);
            KEY(Qt::Key_F, SDLK_f);
            KEY(Qt::Key_G, SDLK_g);
            KEY(Qt::Key_H, SDLK_h);
            KEY(Qt::Key_I, SDLK_i);
            KEY(Qt::Key_J, SDLK_j);
            KEY(Qt::Key_L, SDLK_l);
            KEY(Qt::Key_M, SDLK_m);
            KEY(Qt::Key_N, SDLK_n);
            KEY(Qt::Key_O, SDLK_o);
            KEY(Qt::Key_P, SDLK_p);
            KEY(Qt::Key_Q, SDLK_q);
            KEY(Qt::Key_R, SDLK_r);
            KEY(Qt::Key_S, SDLK_s);
            KEY(Qt::Key_T, SDLK_t);
            KEY(Qt::Key_U, SDLK_u);
            KEY(Qt::Key_V, SDLK_v);
            KEY(Qt::Key_W, SDLK_w);
            KEY(Qt::Key_X, SDLK_x);
            KEY(Qt::Key_Y, SDLK_y);
            KEY(Qt::Key_Z, SDLK_z);
            default:
                sdl_event_valid = false;
                redraw_necessary = false; 
                WARN("Invalid key code [%i].  May not be implemented.", keycode);
                break;
            #undef KEY
        }
    }
    // Translate mouse

    // Send the event
    if(sdl_event_valid) {
        int result = SDL_PushEvent(&sdl_event);
    }
    if(redraw_necessary) {
        this->update();
    }
    return QWidget::event(event);
}

#endif