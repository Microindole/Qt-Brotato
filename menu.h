#ifndef MENU_H
#define MENU_H

#include <QWidget>

namespace Ui {
class Menu;
}

class Menu : public QWidget
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    ~Menu();

signals:
    void startGameRequested(); // 发送开始游戏信号
    void exitGameRequested();  // 发送退出游戏信号

private slots:

private:
    Ui::Menu *ui;
};

#endif // MENU_H
