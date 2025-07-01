# Brotato-Qt: 一个功能完备的Qt综合实训项目

<p align="center"> <img src="https://img.shields.io/badge/Qt-6.x-blue.svg?style=for-the-badge&logo=qt" alt="Qt 6"> <img src="https://img.shields.io/badge/C++-17-blue.svg?style=for-the-badge&logo=c%2B%2B" alt="C++17"> <img src="https://img.shields.io/badge/CMake-3.16+-green.svg?style=for-the-badge&logo=cmake" alt="CMake"> <img src="https://img.shields.io/badge/Python-3.x-blue.svg?style=for-the-badge&logo=python" alt="Python"> <img src="https://img.shields.io/badge/Flask-2.x-lightgrey.svg?style=for-the-badge&logo=flask" alt="Flask"> </p>

<p align="center"> <em>本项目是一个基于 Qt/C++ 构建，并集成了Python/Flask支付后端的类幸存者游戏，是学习和实践Qt应用开发的绝佳案例。</em> </p>


## 🌟 项目简介

**Brotato-Qt** 不仅仅是一个简单的2D生存游戏，它是一个功能丰富的综合性实训项目，旨在全面展示如何运用现代C++和Qt框架构建一个完整的、可扩展的桌面应用。玩家将扮演一个“土豆”英雄，在不断涌来的敌人波次中生存下来，通过战斗获取金币和经验，升级角色，购买道具，并最终挑战强大的Boss。

作为一份**Qt实训项目**，本项目的核心价值在于其**模块化的架构**、**清晰的代码实现**以及对**Qt核心知识点的全面运用**。特别是集成的**DLC购买流程模拟**，通过与**Python Flask后端**和**支付宝沙箱**的交互，真实地模拟了从客户端发起支付、到服务器处理、再到最终解锁游戏内容的全过程，完整地覆盖了桌面应用开发的多个方面。

## ✨ 核心功能

### 游戏玩法

- **🕹️ 经典肉鸽生存**: 在有限的场地内，面对一波又一波越来越强的敌人，考验你的走位与策略。
- **🎭 四种可选角色**:
  - **全能者**: 属性均衡，自带微弱的生命恢复。
  - **斗士**: 伤害更高，但更脆弱。
  - **医生**: 攻击力较低，但拥有强大的生命恢复能力。
  - **公牛**: 血量极高，攻击距离较短，是坚实的肉盾。
- **⚔️ 多样化的敌人与Boss**:
  - 四种行为各异的普通敌人（近战、快速、冲锋、远程）。
  - 强大的周期性Boss，拥有普通和**狂暴（二阶段）**两种攻击模式。
- **🗺️ 可选地图与DLC**:
  - 包含一个标准地图和三个带有不同**Debuff**的DLC地图（熔岩、沙漠、洞穴）。
  - 完整的DLC购买流程，提供沉浸式的功能解锁体验。
- **🔺 随机升级系统**: 每次升级时，从随机出现的属性中三选一，打造你独一无二的角色Build。
- **🛒 波次间商店**: 在每波战斗结束后，使用获得的金币在商店中购买各种强力道具，永久提升你的能力。



## 📁 模块化架构详解

为了提升代码的可读性、可维护性和可扩展性，本项目采用了高度模块化的设计。每个模块权责分明，并通过Qt的信号与槽机制进行低耦合的通信。

**点击下方链接查看每个模块的详细设计文档：**

- [**📄 1. 总体架构与页面管理 (MainWindow)**](./docs/MAIN_ARCHITECTURE.md)
- [**📄 2. 核心游戏逻辑 (GameWidget)**](./docs/GAME_LOGIC.md)
- [**📄 3. 游戏实体设计 (Player, Enemy, Boss)**](./docs/GAME_ENTITIES.md)
- [**📄 4. UI系统与用户交互 (Menus, Shops, etc.)**](./docs/UI_SYSTEM.md)
- [**📄 5. 全局单例与管理系统 (ResourceManager, GameData)**](./docs/MANAGEMENT_SYSTEMS.md)
- [**📄 6. DLC支付系统 (C++/Python 交互)**](./docs/DLC_PAYMENT.md)
- [**📄 7. Natapp 内网穿透详细教程**](./docs/NATAPP_TUTORIAL.md)



## 💡 Qt核心知识点应用详解

本项目旨在全面运用Qt框架的各项核心技术，是检验和巩固Qt学习成果的理想平台。以下是项目中重点运用的Qt知识点：

#### 1. UI设计与布局

- **UI文件 (`.ui`)**: 项目广泛使用Qt Designer进行界面设计，将UI布局（如`menu.ui`, `choose.ui`, `gamewidget.ui`）与业务逻辑分离。这些`.ui`文件通过`uic`（UI编译器）在构建时自动生成C++头文件（如`ui_menu.h`），在代码中通过`ui->setupUi(this);`进行加载和实例化。
- **布局管理器**:
  - `QHBoxLayout` 和 `QVBoxLayout` 用于组织横向和垂直的控件。
  - `QGridLayout` 用于构建更复杂的网格布局，如主菜单和角色选择界面。
  - `QStackedWidget` 作为核心的页面切换容器，在`MainWindow`中管理着主菜单、选择界面和游戏界面的切换，是单窗口多页面应用的典型实现。
- **样式表 (QSS)**: 项目通过QSS（Qt Style Sheets）对`QPushButton`, `QLabel`, `QProgressBar`等控件进行深度美化，实现了统一的深色游戏风格。QSS的运用类似于Web开发中的CSS，是Qt界面美化的重要手段。

#### 2. 图形与视图框架

- **`QGraphicsScene` 与 `QGraphicsView`**: 这是游戏世界构建的基石。`gameScene`作为无限大的逻辑场景，容纳所有的游戏对象。`ui->gameView`作为“摄像机”，负责将场景中的一部分渲染到屏幕上，并自动处理缩放和滚动。
- **自定义图形项 (`QGraphicsItem`)**: 游戏中的所有动态对象（`Player`, `Enemy`, `Boss`, `Bullet`, `Coin`）都继承自`QGraphicsItem`的子类（如`QGraphicsEllipseItem`, `QGraphicsPixmapItem`），从而能够被`QGraphicsScene`管理。
- **自定义绘制 (`paint` event)**: 通过重写`paint()`方法，实现了对游戏对象的完全自定义绘制。例如，玩家的呼吸动画、脚部摆动、动态血条的显示等，都是在`paint()`中通过`QPainter`完成的。
- **碰撞检测**: 利用`QGraphicsItem::collidesWithItem()`方法，高效地实现了子弹与敌人、玩家与敌人、玩家与金币之间的碰撞检测。
- **动画 (`advance` event)**: `QGraphicsScene::advance()`由一个`QTimer`周期性调用，它会触发场景中所有图形项的`advance()`方法。本项目利用此机制来更新每个游戏对象的位置和状态，从而驱动整个游戏的动画。

#### 3. 信号与槽机制

- **基础应用**: 项目中随处可见信号与槽的应用，例如，将`QPushButton::clicked()`信号连接到自定义的槽函数上，以响应用户的点击操作。
- **自定义信号**: 为了解耦不同模块，项目中定义了大量自定义信号。例如，`Choose`界面完成选择后会`emit selectionConfirmed();`信号，`MainWindow`接收到该信号后调用槽函数切换到游戏界面。这充分体现了Qt“高内聚、低耦合”的设计思想。
- **Lambda表达式**: 对于简单的连接，项目使用了C++11的Lambda表达式，使代码更紧凑。例如：`connect(ui->allrounderButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::AllRounder); });`

#### 4. Qt核心模块运用

- **事件系统 (`QEvent`)**: 通过重写`keyPressEvent()`和`keyReleaseEvent()`来处理键盘输入，控制玩家移动。重写`showEvent()`在窗口显示时执行初始化逻辑。重写`resizeEvent()`来动态调整UI布局，保证窗口缩放时的显示效果。
- **定时器 (`QTimer`, `QElapsedTimer`)**: `QTimer`是整个游戏的“心跳”，驱动着游戏主循环（`updateGame`）、敌人生成、自动射击等周期性任务。`QElapsedTimer`则用于精确计算并显示游戏的实时帧率（FPS）。
- **资源系统 (`.qrc`)**: 所有的图片、音效、配置文件（如`maps.xml`）都被添加到了Qt资源文件`resources.qrc`中。这使得所有资源都被编译进最终的可执行文件，极大地简化了程序的分发和部署，避免了因缺少资源文件而导致的运行时错误。
- **文件与数据处理**:
  - `QFile`用于从资源系统中读取`:configs/maps.xml`。
  - `QXmlStreamReader`以高效、低内存占用的流式方式解析XML配置文件，加载地图数据。
- **网络模块 (`Qt Network`)**: 在`PaymentManager`类中，使用`QNetworkAccessManager`, `QNetworkRequest`, `QNetworkReply`来与Python后端进行HTTP通信，实现了POST数据（创建订单）和GET数据（轮询支付状态）的功能。
- **多媒体模块 (`Qt Multimedia`)**: `QMediaPlayer`用于播放循环的背景音乐（BGM），而`QSoundEffect`则用于播放需要低延迟、高频率触发的游戏音效（如射击、击中）。

#### 5. 元对象系统

- **`Q_OBJECT`宏**: 所有自定义了信号或槽的类都包含了此宏，它是元对象系统正常工作的前提。
- **`qobject_cast`**: 在`Menu`类的构造函数中，使用`qobject_cast`对`this->layout()`进行安全的类型转换，是类型安全的向下转型的标准做法。
- **`Q_DECLARE_METATYPE`**: 在`gamedata.h`中，使用`Q_DECLARE_METATYPE(MapInfo)`将自定义的`MapInfo`结构体注册到Qt的元类型系统中，这使得`MapInfo`对象可以被存放在`QVariant`中，并在信号与槽之间作为参数传递。

## 🛠️ 技术栈

| 分类         | 技术/工具                           | 备注                             |
| ------------ | ----------------------------------- | -------------------------------- |
| **客户端**   | `Qt 6.x`, `C++17`                   | 核心框架与编程语言               |
| **构建系统** | `CMake 3.16+`                       | 跨平台项目构建                   |
| **Qt模块**   | `Widgets`, `Multimedia`, `Network`  | 分别用于UI、音视频播放、网络通信 |
| **服务端**   | `Python 3.x`, `Flask`, `Flask-CORS` | 提供支付API接口                  |
| **支付接口** | `alipay-sdk-python`                 | 与支付宝沙箱环境交互             |
| **其他库**   | `qrcode`, `pillow`                  | 在服务端用于生成支付二维码       |

## 🚀 如何运行

要完整体验本项目（包括DLC购买），您需要分别运行 **C++游戏客户端** 和 **Python支付服务器**。

### 1. 运行支付服务器 (Python)

**前置要求**:

- Python 3.6+
- pip 包管理器
- 一个可以接收公网请求的工具（如 [natapp](https://natapp.cn/) 或 [ngrok](https://ngrok.com/)），用于接收支付宝的异步回调。

**步骤**:

1. **安装Python依赖**:

   ```
   # 进入项目根目录
   cd /path/to/your/project
   # 安装所有必要的库
   pip install flask flask-cors alipay-sdk-python qrcode pillow
   ```

2. **配置支付宝沙箱密钥**:

   - 前往 [支付宝开放平台](https://open.alipay.com/develop/sandbox/app) 登录并进入沙箱环境。
   - 将下载的应用私钥 (`app_private_key.pem`) 和支付宝公钥 (`alipay_public_key.pem`) 文件放入项目根目录下的 `keys` 文件夹中。

3. **配置公网穿透**:

   - 使用 `natapp` 或 `ngrok` 等工具，将你本地的 `5000` 端口暴露到公网，获得一个公网地址。
   - 打开 `app.py` 文件，修改 `PUBLIC_DOMAIN` 变量为你获取到的公网地址。

4. **启动服务器**:

   ```
   python app.py
   ```

### 2. 构建并运行游戏客户端 (C++/Qt)

**前置要求**:

- Qt 6.2 或更高版本
- 支持 C++17 的编译器 (如 MSVC, MinGW, GCC, Clang)
- CMake 3.16 或更高版本

**使用 Qt Creator (推荐)**:

1. 打开 Qt Creator，选择 `文件` -> `打开文件或项目`，选择项目根目录下的 `CMakeLists.txt`。
2. 在弹出的“配置项目”窗口中，选择一个合适的构建套件 (Kit)。
3. 点击“配置项目”，等待CMake配置完成。
4. 点击左下角的“运行”按钮 (或按 `Ctrl+R`) 即可编译并启动游戏。

> **⚠️ 注意**: 请确保在Qt Creator中选择的构建套件与您安装的Qt版本和编译器匹配，这是成功编译的关键。

## 📁 项目架构详解

```
Brotato-Qt/
├── 📂 build/                  # CMake构建目录
├── 📂 images/                 # 图片资源 (角色, 敌人, 地图, UI等)
├── 📂 keys/                   # 存放支付宝密钥 (需自行配置)
├── 📂 sounds/                 # 音频资源 (.wav)
├── 📜 app.py                  # Python Flask支付服务器
├── 📜 bullet.h/cpp             # 子弹类 (QGraphicsItem)
├── 📜 boss.h/cpp               # Boss类 (继承自Enemy)
├── 📜 choose.h/cpp/ui          # 角色与地图选择界面 (QWidget)
├── 📜 coin.h/cpp               # 金币掉落物类 (QGraphicsPixmapItem)
├── 📜 dynamicbackground.h/cpp  # 动态视差背景 (QWidget, 自定义paintEvent)
├── 📜 enemy.h/cpp              # 敌人基类 (QGraphicsEllipseItem)
├── 📜 gamedata.h               # 全局数据单例 (QObject, Q_DECLARE_METATYPE)
├── 📜 gamewidget.h/cpp/ui      # 游戏核心逻辑与视图 (QWidget, QGraphicsView)
├── 📜 gamewidget_*.cpp         # GameWidget的模块化拆分文件
├── 📜 main.cpp                 # 程序入口 (QApplication)
├── 📜 mainwindow.h/cpp/ui      # 主窗口 (QMainWindow, QStackedWidget)
├── 📜 maps.xml                 # 地图配置文件 (由QXmlStreamReader解析)
├── 📜 menu.h/cpp/ui            # 主菜单界面 (QWidget)
├── 📜 pause.h/cpp/ui           # 暂停界面 (QWidget)
├── 📜 payment_manager.h/cpp    # C++支付管理器 (Qt Network)
├── 📜 player.h/cpp             # 玩家角色类 (QGraphicsEllipseItem)
├── 📜 resourcemanager.h/cpp    # 资源管理器单例 (QObject, QPixmap, QSoundEffect)
├── 📜 settings.h/cpp/ui        # 设置界面 (QWidget)
├── 📜 shop.h/cpp/ui            # 商店界面 (QWidget)
├── 📜 upgradewidget.h/cpp/ui   # 升级选择界面 (QWidget)
├── 📜 CMakeLists.txt           # CMake构建脚本
└── 📜 resources.qrc            # Qt资源集合文件
```

### 核心设计模式

- **单例模式 (Singleton)**: `ResourceManager` 和 `GameData` 使用单例模式，确保全局只有一个实例，方便在任何地方访问资源和共享游戏状态，有效避免了复杂的依赖注入和信号传递。
- **状态机模式 (State Machine)**: `Boss` 和冲锋怪 `Charger` 的行为逻辑中隐式地使用了状态机（如 `Idle`, `Chasing`, `Attacking`, `Dashing`），使得复杂的AI行为管理变得清晰。
- **组件化思想**: 将庞大的 `GameWidget` 类按功能（音频、碰撞、事件等）拆分到多个 `.cpp` 文件中，是大型项目开发中保持代码整洁的有效方法。

## 📝 开发与扩展注意点

- **添加新地图**:
  1. 在 `images/` 目录下添加新的地图背景图片。
  2. 在 `resources.qrc` 中注册该图片。
  3. 在 `maps.xml` 中添加一个新的 `<map>` 条目。
  4. 在 `choose.ui` 中添加一个新的 `QPushButton`，并将其 `objectName` 命名为 `mapButton_你的key`。
  5. 在 `choose.cpp` 的构造函数中为新按钮连接 `onMapButtonClicked` 槽函数。
- **数值平衡**: 游戏的核心数值（玩家属性、敌人成长、商店价格等）都以硬编码的形式存在于各自的 `.cpp` 文件中（如 `player.cpp`, `enemy.cpp`, `shop.cpp`）。为了更方便的配置，未来可以考虑将这些数值也移到外部XML或JSON文件中。
- **资源路径**: 所有资源路径都以 `:/` 开头，这意味着它们被编译到了可执行文件中。这是Qt资源系统的标准用法，能保证程序在任何地方都能找到资源。

## 🤝 贡献

我们非常欢迎各种形式的贡献！如果你有任何建议或发现了Bug，请随时提交 **Issue**。

🎮 **感谢您的关注，祝您在土豆的世界里玩得开心！**