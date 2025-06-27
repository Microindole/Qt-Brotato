#include "choose.h"
#include "ui_choose.h"

Choose::Choose(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Choose)
{
    ui->setupUi(this);

    // 将每个角色按钮的点击信号连接到一个lambda表达式
    // 当按钮被点击时，它会发出 characterSelected 信号并附带正确的角色类型
    connect(ui->allrounderButton, &QPushButton::clicked, this, [this](){ emit characterSelected(Player::AllRounder); });
    connect(ui->fighterButton, &QPushButton::clicked, this, [this](){ emit characterSelected(Player::Fighter); });
    connect(ui->doctorButton, &QPushButton::clicked, this, [this](){ emit characterSelected(Player::Doctor); });
    connect(ui->bullButton, &QPushButton::clicked, this, [this](){ emit characterSelected(Player::Bull); });

    // 连接返回按钮的信号
    connect(ui->backButton, &QPushButton::clicked, this, &Choose::backToMenuRequested);
}

Choose::~Choose()
{
    delete ui;
}
