/*!
 *  @file include/se/graphics/qtSilhouetteWidget.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifdef SE_ENABLE_QT_WIDGET_SUPPORT

#ifndef _SE_GRAPHICS_QTSILHOUETTEWIDGET_H_
#define _SE_GRAPHICS_QTSILHOUETTEWIDGET_H_

#include "se/fwd.hpp"

#include <QOpenGLWidget>

namespace se::graphics {

    class QTSilhouetteWidget : public QOpenGLWidget {

        Q_OBJECT

        private:

            se::Engine* engine;

        public:

            QTSilhouetteWidget(QWidget* qparent = nullptr);

            ~QTSilhouetteWidget();

            void configure(se::Engine* engine);

        protected:

            void initializeGL();
            void paintGL();
            void resizeGL(int width, int height);

            QSize minimumSizeHint() const;
            QSize sizeHint() const;

    };

}

#endif
#endif