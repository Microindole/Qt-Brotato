#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menu.h"
#include "gamewidget.h"
#include "choose.h"
#include <QApplication>
#include <QMessageBox>
#include <QRandomGenerator>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建页面
    menuWidget = new Menu(this);
    gameWidget = new GameWidget(this);
    chooseWidget = new Choose(this);

    ui->stackedWidget->addWidget(menuWidget);
    ui->stackedWidget->addWidget(chooseWidget);
    ui->stackedWidget->addWidget(gameWidget);

    // 连接信号
    // 从菜单到角色选择
    connect(menuWidget, &Menu::startGameRequested, this, &MainWindow::showChooseScreen);
    // 从角色地图选择到游戏
    connect(chooseWidget, &Choose::selectionConfirmed, this, &MainWindow::startGameFromSelection);
    // 从选择返回菜单
    connect(chooseWidget, &Choose::backToMenuRequested, this, &MainWindow::showMenuScreen);
    // 从游戏返回菜单
    connect(gameWidget, &GameWidget::backToMenuRequested, this, &MainWindow::showMenuScreen);
    // 退出应用
    connect(menuWidget, &Menu::exitGameRequested, this, &MainWindow::exitApplication);


    showMenuScreen();
    showFullScreen();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showMenuScreen()
{
    ui->stackedWidget->setCurrentWidget(menuWidget);
}

void MainWindow::showChooseScreen()
{
    ui->stackedWidget->setCurrentWidget(chooseWidget);
}

void MainWindow::exitApplication()
{
    QApplication::quit();
}

void MainWindow::startGameFromSelection()
{
    Player::CharacterType chosenChar = GameData::instance().selectedCharacter;
    MapInfo chosenMap = GameData::instance().selectedMap;
    gameWidget->prepareGame(chosenChar, chosenMap);
    ui->stackedWidget->setCurrentWidget(gameWidget);
    gameWidget->startGame();
}
