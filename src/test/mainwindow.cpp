
#include "se/graphics/graphicsController.hpp"
#include "se/engine.hpp"
#include "se/entity.hpp"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "util/log.hpp"

#include "se/graphics/qtSilhouetteWidget.hpp"

MainWindow::MainWindow(se::Engine* engine, se::Entity* target, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    this->engine = engine;
    this->target = target;
    ui->setupUi(this);

    ui->silhouette->configure(engine);

    // Create the opengl widget
    //se::graphics::QTSilhouetteWidget widget(engine);
    //ui->gridLayout_3->replaceWidget(ui->gl_placeholder, &widget);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_rotation_sliderMoved(int position) {
    this->target->rz = (position / 1000.0);
    this->ui->silhouette->update();
    DEBUG("New slider value is %i", position);
}
