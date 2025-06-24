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
}

Menu::~Menu()
{
    delete ui;
}
