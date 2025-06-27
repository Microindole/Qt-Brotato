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
    void showGameScreen(Player::CharacterType character);
    void showMenuScreen();
    void showChooseScreen();
    void exitApplication();

private:
    Ui::MainWindow *ui;
    Menu *menuWidget;
    GameWidget *gameWidget;
    Choose *chooseWidget;
};

#endif // MAINWINDOW_H
