// filepath: d:\Qt\works\Qt-Brotato\mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menu.h"
#include "gamewidget.h"
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

    // 添加到 StackedWidget
    ui->stackedWidget->addWidget(menuWidget);
    ui->stackedWidget->addWidget(gameWidget);

    // 连接信号
    connect(menuWidget, &Menu::startGameRequested, this, &MainWindow::showGameScreen);
    connect(menuWidget, &Menu::exitGameRequested, this, &MainWindow::exitApplication);
    connect(gameWidget, &GameWidget::backToMenuRequested, this, &MainWindow::showMenuScreen);

    // 默认显示菜单
    showMenuScreen();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showGameScreen()
{
    ui->stackedWidget->setCurrentWidget(gameWidget);
    gameWidget->startGame();
}

void MainWindow::showMenuScreen()
{
    ui->stackedWidget->setCurrentWidget(menuWidget);
}

void MainWindow::exitApplication()
{
    QApplication::quit();
}