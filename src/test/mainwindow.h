#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "se/fwd.hpp"

#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {

    Q_OBJECT

    public:
        explicit MainWindow(se::Engine* engine, se::Entity* target, QWidget *parent = nullptr);
        ~MainWindow();

    private slots:
        void on_rotation_sliderMoved(int position);

        void on_msaa_level_valueChanged(int arg1);

private:
        Ui::MainWindow *ui;

        se::Engine* engine;

        se::Entity* target;
};

#endif // MAINWINDOW_H
