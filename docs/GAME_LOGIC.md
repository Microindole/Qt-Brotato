# 模块详解: 核心游戏逻辑 (GameWidget)

本文档详细解析 `Brotato-Qt` 项目的**游戏核心模块** `GameWidget`。`GameWidget` 是整个游戏玩法的中枢，负责管理游戏循环、场景、对象、状态以及所有相关的交互逻辑。

## 1. 核心职责

`GameWidget` 的职责非常广泛，是项目中最大、最复杂的模块。其核心职责包括：

- **游戏世界构建**: 创建并管理 `QGraphicsScene`，作为所有游戏对象的容器。
- **游戏主循环**: 通过 `QTimer` 驱动 `updateGame()` 函数，实现游戏的周期性更新（约60FPS）。
- **对象管理**: 负责玩家(`Player`)、敌人(`Enemy`, `Boss`)、子弹(`Bullet`)、金币(`Coin`)等游戏对象的创建、销毁和生命周期管理。
- **状态管理**: 控制游戏的核心状态，如 `gameRunning`, `gamePaused`, `m_isWaveTransition` 等。
- **游戏流程控制**: 管理从游戏开始、波次进行、波次结束、进入商店/升级，到下一波开始的完整流程。
- **物理与碰撞**: 在主循环中调用碰撞检测函数，处理所有对象间的交互。
- **UI更新**: 负责更新游戏界面上的所有状态显示，如血条、经验条、分数、波次等。

## 2. 文件组成

为了提高代码的可维护性，`GameWidget` 的庞大逻辑被拆分到了多个 `.cpp` 文件中，这是一种优秀的工程实践。

- `gamewidget.h`: `GameWidget` 类的头文件，定义了所有成员变量、函数声明和信号槽。
- `gamewidget.cpp`: 包含构造函数、析构函数以及游戏准备和启动的核心逻辑。
- `gamewidget_update.cpp`: 包含游戏主循环 `updateGame()` 和相关的移动、生成、射击等逻辑。
- `gamewidget_collisions.cpp`: 包含 `checkCollisions()` 函数，专门处理所有碰撞检测。
- `gamewidget_state.cpp`: 包含管理游戏状态的函数，如暂停、继续、游戏结束、波次转换等。
- `gamewidget_ui.cpp`: 包含所有与UI更新相关的函数，如 `updateUI()`, `showPauseMenu()`, `showShopScreen()` 等。
- `gamewidget_events.cpp`: 包含所有事件处理函数，如 `keyPressEvent`, `keyReleaseEvent`, `resizeEvent`。
- `gamewidget_audio.cpp`: 包含所有与音频播放相关的函数，如背景音乐和音效的控制。
- `gamewidget.ui`: 使用Qt Designer设计的游戏界面，核心是一个`QGraphicsView`用于显示游戏场景，以及顶部的状态栏。

## 3. 关键实现分析

### 3.1. 图形视图框架 (Graphics View Framework)

游戏画面的实现完全依赖于Qt的图形视图框架。

- **`QGraphicsScene` (`gameScene`)**: 作为逻辑场景，它的坐标系就是游戏世界的坐标系。通过`gameScene->setBackgroundBrush(...)`设置背景，通过`gameScene->setSceneRect(...)`定义游戏世界的边界。
- **`QGraphicsView` (`ui->gameView`)**: 作为视图，负责将`gameScene`渲染到屏幕上。`ui->gameView->fitInView(gameScene->sceneRect(), Qt::KeepAspectRatio)`这行代码至关重要，它能确保无论窗口如何缩放，游戏场景总是能保持正确的宽高比并完整地显示在视图中。
- **`QGraphicsItem`**: 所有游戏对象（Player, Enemy等）都继承自`QGraphicsItem`，通过`gameScene->addItem()`被添加到场景中进行统一管理和渲染。

### 3.2. 游戏主循环

游戏的主循环由一个`QTimer` (`gameTimer`)驱动，它以大约60FPS的频率（`gameTimer->start(16)`)周期性地调用`updateGame()`槽函数。

`updateGame()`函数是游戏世界前进的动力，它在每一帧都会执行以下操作：

1. **处理玩家输入**: 调用`movePlayer()`根据当前按下的按键移动玩家。
2. **更新对象位置**: 遍历所有敌人和子弹，调用它们的`move`或`moveTowards`方法。
3. **碰撞检测**: 调用`checkCollisions()`处理所有可能的碰撞事件。
4. **清理越界对象**: 调用`cleanupObjects()`移除飞出屏幕的子弹。
5. **更新UI**: 调用`updateUI()`刷新屏幕上的各种状态显示。
6. **驱动场景动画**: 调用`gameScene->advance()`，这会触发场景中所有`QGraphicsItem`的`advance()`方法，用于实现对象的内部动画（如呼吸、浮动等）。

### 3.3. 状态机

`GameWidget`内部通过一系列布尔值标志位实现了一个简单的状态机，来控制游戏的不同阶段：

- `gameRunning`: 游戏是否正在进行。
- `gamePaused`: 游戏是否处于玩家手动暂停状态。
- `m_isWaveTransition`: 是否处于波次之间的过渡阶段（商店/升级界面）。

这些状态标志在 `updateGame()` 和其他事件处理函数中被用来判断是否应该执行相应的逻辑，例如：`if (!gameRunning || gamePaused || !player) return;`。

### 3.4. 模块化拆分

将一个庞大的类拆分到多个实现文件中是一种被称为**“部分类(Partial Class)”**（尽管C++没有原生支持）的实践。在本项目中，所有`gamewidget_*.cpp`文件都`#include "gamewidget.h"`，并各自实现头文件中声明的一部分函数。

**好处**:

- **提高可读性**: 每个文件只关注一部分逻辑（如UI、碰撞），更容易阅读和理解。
- **便于协作**: 在团队开发中，不同开发者可以同时修改不同的文件而不会产生冲突。
- **降低认知负担**: 当需要修改某部分逻辑时，可以直接定位到对应的文件，而无需在数千行的单个文件中滚动查找。

## 4. 应用的Qt知识点

- **图形视图框架**: `QGraphicsScene`, `QGraphicsView`, `QGraphicsItem` 的综合运用。
- **定时器**: `QTimer` 用于驱动游戏循环，`QElapsedTimer` 用于FPS计算。
- **事件处理**: `keyPressEvent`, `keyReleaseEvent`, `resizeEvent`。
- **音频**: `QMediaPlayer` (BGM) 和 `QSoundEffect` (SFX)。
- **容器类**: `QList` 用于管理游戏对象列表，`QSet` 用于高效地存储当前按下的按键。
- **内存管理**: `qDeleteAll()` 用于安全、方便地删除容器中的所有指针对象，防止内存泄漏。