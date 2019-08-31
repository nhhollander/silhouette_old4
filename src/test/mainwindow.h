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
        explicit MainWindow(se::Engine* engine, QWidget *parent = nullptr);
        ~MainWindow();

    private:
        Ui::MainWindow *ui;

        se::Engine* engine;
};

#endif // MAINWINDOW_H
