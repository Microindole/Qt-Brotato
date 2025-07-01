# 模块详解: 全局单例与管理系统

本文档详细解析 `Brotato-Qt` 项目中两个至关重要的后台管理模块：`ResourceManager` 和 `GameData`。这两个类都采用了**单例（Singleton）设计模式**，为整个应用程序提供集中的资源管理和全局状态共享服务。

## 1. 单例设计模式的应用

### 为什么使用单例？

在游戏开发中，某些资源和数据需要在程序的任何地方都能被方便地访问，同时又要确保它们在内存中只存在一份。

- **资源（图片、音频）**: 加载资源是耗时操作。如果每个需要图片的类都自己去加载一遍，会造成巨大的性能浪费和内存冗余。
- **全局游戏状态**: 玩家的选择（角色、地图）、DLC的激活状态等信息，需要在不同的界面（如`Menu`, `Choose`, `GameWidget`）之间共享。

单例模式完美地解决了这些问题。它保证一个类只有一个实例，并提供一个全局访问点来获取这个实例。

### C++中的实现方式

本项目采用了经典的 **“Meyers' Singleton”** 实现方式，这被认为是C++11及以后版本中最简单、最线程安全的实现方法：

```
// resourcemanager.h
class ResourceManager : public QObject
{
public:
    static ResourceManager& instance() {
        static ResourceManager instance; // 关键：静态局部变量
        return instance;
    }
private:
    // 将构造、析构、拷贝函数设为私有，防止外部创建或复制实例
    ResourceManager();
    ~ResourceManager();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
};
```

当 `instance()` 函数第一次被调用时，静态局部变量 `instance` 会被创建并初始化。由于C++标准保证了静态局部变量的初始化是线程安全的，因此这种方式既简洁又可靠。

在代码的任何地方，都可以通过 `ResourceManager::instance().someFunction()` 来调用它的方法。

## 2. `ResourceManager` (资源管理器)

### 核心职责

- **集中加载**: 负责加载游戏所需的所有静态资源，如图片（`QPixmap`）和音效（`QSoundEffect`）。
- **资源缓存**: 将加载过的资源缓存在内存中（使用`QMap`），避免重复的磁盘I/O操作，极大地提升了性能。
- **统一接口**: 提供全局统一的接口 `getPixmap()` 和 `playSound()`，供其他模块调用。

### 关键实现分析

- **预加载 (`preloadAssets`)**: 在`ResourceManager`的构造函数中调用`preloadAssets()`，这意味着当程序第一次请求`ResourceManager`实例时，所有核心资源就会被一次性加载到内存中。这可能会稍微增加程序的启动时间，但能有效避免游戏过程中的卡顿，对于追求流畅体验的游戏来说是标准做法。
- **图片缓存 (`m_pixmapCache`)**: 一个`QMap<QString, QPixmap>`，以资源路径为键，`QPixmap`对象为值。`getPixmap()`函数会先检查缓存中是否存在请求的图片，如果存在则直接返回，否则从磁盘（或Qt资源系统）加载，存入缓存后再返回。
- **音效缓存 (`m_soundEffectCache`)**: 一个`QMap<QString, QSoundEffect*>`，以音效名称为键，`QSoundEffect`指针为值。与图片不同，`QSoundEffect`对象在预加载时就被创建，`playSound()`只是触发其`play()`方法。

## 3. `GameData` (全局游戏数据)

### 核心职责

- **状态共享**: 作为一个全局的数据容器，存储那些需要在不同界面和模块之间共享的状态。
- **解耦模块**: 避免了在不同界面间通过复杂的构造函数参数或信号槽来传递状态。例如，`Choose`界面只需要将玩家的选择存入`GameData`，而`GameWidget`则直接从`GameData`中读取这些选择，两者之间没有直接的依赖关系。

### 存储的核心数据

- **DLC状态**:
  - `dlc1_purchased`: 标记DLC是否已购买。
  - `dlc1_active`: 标记DLC在当前游戏会话中是否被玩家勾选激活。
- **玩家选择**:
  - `selectedMap`: 存储玩家最终选择的`MapInfo`结构体。
  - `selectedCharacter`: 存储玩家最终选择的`Player::CharacterType`枚举。

### 使用流程示例

1. **写入数据 (在 `Choose` 界面)**:

   ```
   // choose.cpp -> onConfirmClicked()
   // 当玩家点击“确认选择”时
   GameData::instance().selectedMap = m_allMaps.value(m_selectedMapKey);
   GameData::instance().selectedCharacter = m_selectedCharacter;
   emit selectionConfirmed();
   ```

2. **读取数据 (在 `MainWindow` 中)**:

   ```
   // mainwindow.cpp -> startGameFromSelection()
   // MainWindow接收到selectionConfirmed信号后
   Player::CharacterType chosenChar = GameData::instance().selectedCharacter;
   MapInfo chosenMap = GameData::instance().selectedMap;
   gameWidget->prepareGame(chosenChar, chosenMap); // 将数据传递给GameWidget
   ```

## 4. 应用的Qt知识点

- **单例设计模式**: C++核心设计模式在Qt项目中的应用。
- **`QObject`**: 两个单例都继承自`QObject`，这使得它们能够利用Qt的元对象系统（尽管在这个具体实现中未直接使用信号槽）。
- **Qt容器类**: `QMap`被用来实现高效的键值对缓存。
- **`QPixmap`与`QSoundEffect`**: Qt多媒体和图形模块中的核心类，用于处理图片和音频。
- **模块解耦**: 通过全局数据中心的设计思想，降低了不同UI模块之间的耦合度。