#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// 前向声明
class Menu;
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
    void showGameScreen();
    void showMenuScreen();
    void exitApplication();

private:
    Ui::MainWindow *ui;
    Menu *menuWidget;
    GameWidget *gameWidget;
};

#endif // MAINWINDOW_H