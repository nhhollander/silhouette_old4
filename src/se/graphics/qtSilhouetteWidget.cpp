#include "se/engine.hpp"
#include "se/graphics/graphicsController.hpp"
#include "se/graphics/qtSilhouetteWidget.hpp"

#include "util/config.hpp"
#include "util/log.hpp"

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

// =====================
// == PRIVATE MEMBERS ==
// =====================