#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "se/graphics/qtSilhouetteWidget.hpp"

MainWindow::MainWindow(se::Engine* engine, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    this->engine = engine;
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
