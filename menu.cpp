#include "menu.h"
#include "ui_menu.h"
#include <QMessageBox>

Menu::Menu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);

    // 连接信号
    connect(ui->startGameButton, &QPushButton::clicked, this, &Menu::startGameRequested);
    connect(ui->exitButton, &QPushButton::clicked, this, &Menu::exitGameRequested);
    connect(ui->helpButton, &QPushButton::clicked, this, &Menu::showHelp);
}

Menu::~Menu()
{
    delete ui;
}

void Menu::showHelp()
{
    QMessageBox::information(this, "游戏帮助",
        "<h2>控制说明</h2>"
        "<p><b>移动:</b> 使用 <b>W, A, S, D</b> 或<b>方向键</b>来移动你的土豆。</p>"
        "<p><b>攻击:</b> 角色会自动瞄准并攻击范围内最近的敌人。</p>"
        "<p><b>暂停:</b> 按下 <b>ESC</b> 键可以暂停或继续游戏。</p>"
        "<h3>目标</h3>"
        "<p>在越来越强大的敌人波次中生存下来，尽可能获得更高的分数！</p>");
}
