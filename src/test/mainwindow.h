#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "se/fwd.hpp"

#include <QMainWindow>
#include <QListWidgetItem>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {

    Q_OBJECT

    public:
        explicit MainWindow(se::Engine* engine, se::Scene* target, QWidget *parent = nullptr);
        ~MainWindow();

    private slots:
        void on_entity_rot_x_valueChanged(double arg1);

        void on_entity_rot_y_valueChanged(double arg1);

        void on_entity_rot_z_valueChanged(double arg1);

        void on_entity_pos_x_valueChanged(double arg1);

        void on_entity_pos_y_valueChanged(double arg1);

        void on_entity_pos_z_valueChanged(double arg1);

        void on_entity_scale_x_valueChanged(double arg1);

        void on_entity_scale_y_valueChanged(double arg1);

        void on_entity_scale_z_valueChanged(double arg1);

        void on_entity_list_itemDoubleClicked(QListWidgetItem *item);

    private:
        Ui::MainWindow *ui;

        se::Engine* engine;

        se::Entity* target = nullptr;

        se::Scene* scene;

        /*!
         *  Entity no-update flag.
         * 
         *  Set when loading new entity details to prevent the change handlers
         *  from firing inappropriately.
         */
        bool no_update_entity = true;
};

#endif // MAINWINDOW_H
