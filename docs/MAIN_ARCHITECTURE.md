# 模块详解: 总体架构与页面管理

本文档详细解析 `Brotato-Qt` 项目的顶层架构，重点围绕 `MainWindow` 类及其在页面管理中的核心作用。

## 1. 核心职责

`MainWindow` 作为程序的主窗口，是整个应用的**顶层容器**和**交通枢纽**。它的主要职责包括：

- **初始化所有UI页面**：在构造函数中创建主菜单(`Menu`)、选择界面(`Choose`)和游戏界面(`GameWidget`)的实例。
- **管理页面切换**: 利用 `QStackedWidget` 作为页面容器，实现不同界面之间的无缝切换。
- **连接核心信号与槽**: 作为“粘合剂”，负责连接不同页面发出的信号到对应的页面切换槽函数上，构建起整个应用的交互流程。

## 2. 文件组成

- `mainwindow.h`: `MainWindow` 类的头文件，定义了类结构、成员变量和槽函数声明。
- `mainwindow.cpp`: `MainWindow` 类的实现文件，包含了构造、析构和页面切换的逻辑。
- `mainwindow.ui`: 使用Qt Designer设计的UI文件，其核心是一个`QStackedWidget`，占据了整个窗口的中央区域。

## 3. 关键实现分析

### 3.1. 页面容器: `QStackedWidget`

`QStackedWidget` 是实现单窗口多页面应用的标准Qt控件。

- **页面添加**: 在 `MainWindow` 的构造函数中，所有主要的UI界面（`menuWidget`, `chooseWidget`, `gameWidget`）都被创建并使用 `ui->stackedWidget->addWidget()` 添加到 `QStackedWidget` 中。
- **页面切换**: 通过调用 `ui->stackedWidget->setCurrentWidget()` 并传入目标页面的指针，可以方便地显示指定的页面。这是所有页面跳转功能的核心。

### 3.2. 交互流程: 信号与槽的连接

`MainWindow` 的构造函数是整个应用交互逻辑的“布线中心”。它通过 `connect` 函数，将不同页面发出的信号连接到 `MainWindow` 自身的槽函数上，从而驱动页面切换。

```
// mainwindow.cpp
// 示例：连接“开始游戏”按钮的流程
connect(menuWidget, &Menu::startGameRequested, this, &MainWindow::showChooseScreen);

// 示例：连接“确认选择”按钮的流程
connect(chooseWidget, &Choose::selectionConfirmed, this, &MainWindow::startGameFromSelection);

// 示例：连接所有“返回主菜单”的流程
connect(chooseWidget, &Choose::backToMenuRequested, this, &MainWindow::showMenuScreen);
connect(gameWidget, &GameWidget::backToMenuRequested, this, &MainWindow::showMenuScreen);
```

这种设计的优点在于：

- **高内聚**: 每个页面（如`Menu`）只负责发出表示“意图”的信号（如`startGameRequested`），它不关心接下来会发生什么。
- **低耦合**: `MainWindow` 作为上层模块，决定了接收到这些信号后应该执行什么动作（切换到哪个页面）。如果未来流程需要改变（例如在菜单和选择之间增加一个过场动画页面），只需要修改 `MainWindow` 中的 `connect` 逻辑，而无需改动 `Menu` 或 `Choose` 类的代码。

### 3.3. 页面切换槽函数

`MainWindow` 中定义了一系列简单的槽函数，专门用于执行页面切换：

```
// mainwindow.cpp
void MainWindow::showMenuScreen()
{
    ui->stackedWidget->setCurrentWidget(menuWidget);
}

void MainWindow::showChooseScreen()
{
    ui->stackedWidget->setCurrentWidget(chooseWidget);
}

void MainWindow::startGameFromSelection()
{
    // 准备并启动游戏
    gameWidget->prepareGame(...); 
    ui->stackedWidget->setCurrentWidget(gameWidget);
    gameWidget->startGame();
}
```

## 4. 应用的Qt知识点

- **`QMainWindow`**: 作为标准的应用主窗口，提供了菜单栏、工具栏、状态栏等标准组件的框架（尽管本项目未全部使用）。
- **`QStackedWidget`**: 实现单窗口多页面切换的核心。
- **信号与槽机制**: 构建模块间通信的桥梁，实现逻辑解耦。
- **UI文件加载**: 通过 `ui->setupUi(this)` 将`.ui`文件定义的界面与C++代码关联起来。
- **对象所有权**: 所有在 `MainWindow` 中创建的`QWidget`子类（如`menuWidget`）都将`this`（即`MainWindow`实例）作为父对象。这利用了Qt的对象树机制，当`MainWindow`被销毁时，其所有子对象也会被自动销毁，有效防止了内存泄漏。