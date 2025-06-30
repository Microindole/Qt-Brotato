#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "player.h"


class Menu;
class Choose;
class GameWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showMenuScreen();
    void showChooseScreen();
    void exitApplication();
    void startGameFromSelection();

private:
    Ui::MainWindow *ui;
    Menu *menuWidget;
    GameWidget *gameWidget;
    Choose *chooseWidget;
};

#endif // MAINWINDOW_H
