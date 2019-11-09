/*!
 *  @file src/test/mainwindow.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifdef SE_ENABLE_QT_WIDGET_SUPPORT

#include "se/graphics/graphicsController.hpp"
#include "se/engine.hpp"
#include "se/entity.hpp"
#include "se/scene.hpp"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "util/config.hpp"
#include "util/log.hpp"

#include "se/graphics/qtSilhouetteWidget.hpp"

#include <QDirModel>

MainWindow::MainWindow(se::Engine* engine, se::Scene* scene, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    this->engine = engine;
    this->scene = scene;
    ui->setupUi(this);

    ui->silhouette->configure(engine);

    // Load entities
    for(auto i : *scene->get_entities()) {
        const char* name = i.second->get_name();
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QString(name));
        this->ui->entity_list->addItem(item);
    }

    // Load configuration values
    for(auto i : engine->config->config_values) {
        const char* name = i.second->ref_name;
        const char* value = (const char*) i.second->cstring_;
        DEBUG("Config: %s: %s", name, value);
    }

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_entity_rot_x_valueChanged(double arg1) {
    if(this->no_update_entity) { return; }
    this->target->rx = (float) arg1;
    this->ui->silhouette->update();
}

void MainWindow::on_entity_rot_y_valueChanged(double arg1) {
    if(this->no_update_entity) { return; }
    this->target->ry = (float) arg1;
    this->ui->silhouette->update();
}

void MainWindow::on_entity_rot_z_valueChanged(double arg1) {
    if(this->no_update_entity) { return; }
    this->target->rz = (float) arg1;
    this->ui->silhouette->update();
}

void MainWindow::on_entity_pos_x_valueChanged(double arg1) {
    if(this->no_update_entity) { return; }
    this->target->x = arg1;
    this->ui->silhouette->update();
}

void MainWindow::on_entity_pos_y_valueChanged(double arg1) {
    if(this->no_update_entity) { return; }
    this->target->y = arg1;
    this->ui->silhouette->update();
}

void MainWindow::on_entity_pos_z_valueChanged(double arg1) {
    if(this->no_update_entity) { return; }
    this->target->z = arg1;
    this->ui->silhouette->update();
}

void MainWindow::on_entity_scale_x_valueChanged(double arg1) {
    if(this->no_update_entity) { return; }
    this->target->sx = arg1;
    this->ui->silhouette->update();
}

void MainWindow::on_entity_scale_y_valueChanged(double arg1) {
    if(this->no_update_entity) { return; }
    this->target->sy = arg1;
    this->ui->silhouette->update();
}

void MainWindow::on_entity_scale_z_valueChanged(double arg1) {
    if(this->no_update_entity) { return; }
    this->target->sz = arg1;
    this->ui->silhouette->update();
}

void MainWindow::on_entity_list_itemDoubleClicked(QListWidgetItem *item)
{

    this->no_update_entity = true;

    // Disable spinners
    this->ui->entity_rot_x->setEnabled(false);
    this->ui->entity_rot_y->setEnabled(false);
    this->ui->entity_rot_z->setEnabled(false);
    this->ui->entity_pos_x->setEnabled(false);
    this->ui->entity_pos_y->setEnabled(false);
    this->ui->entity_pos_z->setEnabled(false);
    this->ui->entity_scale_x->setEnabled(false);
    this->ui->entity_scale_y->setEnabled(false);
    this->ui->entity_scale_z->setEnabled(false);

    std::string name = item->text().toStdString();
    DEBUG("Double click on [%s]", name.c_str());

    // Find the entity
    se::Entity* e = this->scene->get_entity(name.c_str());
    if(e == nullptr) {
        ERROR("Invalid entity [%s]!");
        return;
    }

    this->target = e;

    this->ui->entity_name->setText(QString(e->get_name()));
    this->ui->entity_type->setText(QString(e->get_type()));

    this->ui->entity_rot_x->setValue(this->target->rx);
    this->ui->entity_rot_y->setValue(this->target->ry);
    this->ui->entity_rot_z->setValue(this->target->rz);
    this->ui->entity_pos_x->setValue(this->target->x);
    this->ui->entity_pos_y->setValue(this->target->y);
    this->ui->entity_pos_z->setValue(this->target->z);
    this->ui->entity_scale_x->setValue(this->target->sx);
    this->ui->entity_scale_y->setValue(this->target->sy);
    this->ui->entity_scale_z->setValue(this->target->sz);

    this->ui->entity_rot_x->setEnabled(true);
    this->ui->entity_rot_y->setEnabled(true);
    this->ui->entity_rot_z->setEnabled(true);
    this->ui->entity_pos_x->setEnabled(true);
    this->ui->entity_pos_y->setEnabled(true);
    this->ui->entity_pos_z->setEnabled(true);
    this->ui->entity_scale_x->setEnabled(true);
    this->ui->entity_scale_y->setEnabled(true);
    this->ui->entity_scale_z->setEnabled(true);

    this->no_update_entity = false;
}

#endif