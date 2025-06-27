#ifndef CHOOSE_H
#define CHOOSE_H

#include <QWidget>
#include "player.h" // 包含Player头文件以使用角色类型枚举

namespace Ui {
class Choose;
}

class Choose : public QWidget
{
    Q_OBJECT

public:
    explicit Choose(QWidget *parent = nullptr);
    ~Choose();

signals:
    // 当一个角色被选择时，发出此信号，并携带角色类型
    void characterSelected(Player::CharacterType type);
    // 当点击返回按钮时，发出此信号
    void backToMenuRequested();

private:
    Ui::Choose *ui;
};

#endif // CHOOSE_H
