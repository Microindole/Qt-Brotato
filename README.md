# Qt-Brotato

![Qt](https://img.shields.io/badge/Qt-6.9-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.16+-green.svg)
![License](https://img.shields.io/badge/License-GPLv3-blue.svg)


🌟 一个基于 **Qt6.9** 框架开发的类 **Brotato** 生存游戏，使用 **CMake** 构建系统。玩家控制一个土豆角色在 2D 世界中生存，击败敌人，获得经验升级，并通过升级系统增强自己的能力。

---

## 🎮 游戏特色

- 🕹️ **经典生存玩法**: 在无尽的敌人波次中生存下去
- 🔧 **升级系统**: 玩家升级时可以选择不同的属性提升
- 👾 **多种敌人类型**: 不同属性和行为的敌人
- 🎵 **音效系统**: 完整的背景音乐和游戏音效
- ⏸️ **暂停与设置**: 支持游戏暂停和音量调节
- 🌊 **波次系统**: 敌人难度随波次递增

---

## 🛠 技术特性

- 🚀 **Qt6.9**: 使用最新的 Qt6.9 框架
- 🛠️ **CMake**: 现代化的构建系统
- 🧩 **模块化设计**: 游戏逻辑分离为多个专门文件
- 🎧 **多媒体支持**: 集成 Qt Multimedia 用于音频播放
- 🖥️ **面向对象**: 完全的 C++ 面向对象设计

---

## 📋 系统要求

| 要求              | 描述                     |
|-------------------|--------------------------|
| Qt               | 6.9.0 或更高版本         |
| CMake            | 3.16 或更高版本          |
| 编译器           | 支持 C++17 的编译器      |
| 操作系统         | Windows, macOS, Linux    |

---

## 🚀 构建与运行

### 使用 Qt Creator
1. 打开 **Qt Creator**
2. 选择 "Open Project"
3. 选择项目根目录下的 `CMakeLists.txt` 文件
4. 配置编译套件 (推荐使用 **Qt 6.9.0 MinGW 64-bit**)
5. 点击运行按钮 (Ctrl+R)

### 使用命令行
```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译
cmake --build .

# 运行 (Windows)
./brotato.exe
```

---

## 🎯 游戏玩法

### 基础操作
- **WASD**: 移动角色
- **ESC**: 暂停游戏
- **自动射击**: 角色会自动向最近的敌人射击

### 游戏机制
- ❤️ **生命值**: 角色有血量上限，受到伤害会减少生命值
- ⭐ **经验系统**: 击败敌人获得经验，升级时可选择属性提升
- 🌊 **波次系统**: 每个波次敌人数量和强度都会增加
- 🔧 **升级选择**: 升级时可从 4 个随机属性中选择一个提升

### 升级属性
- ❤️ **最大生命值**: +20 点生命值上限
- 💥 **攻击力**: +5 点攻击伤害
- 🏃 **移动速度**: +0.3 移动速度
- ♻️ **生命恢复**: +1.0/秒生命恢复速度

---

## 📁 项目结构

```
Qt-Brotato/
├── main.cpp                 # 程序入口
├── mainwindow.*             # 主窗口类
├── menu.*                   # 主菜单界面
├── gamewidget.*             # 游戏主界面
├── gamewidget_*.cpp         # 游戏逻辑模块化文件
│   ├── gamewidget_audio.cpp     # 音频管理
│   ├── gamewidget_collisions.cpp # 碰撞检测
│   ├── gamewidget_events.cpp    # 事件处理
│   ├── gamewidget_state.cpp     # 状态管理
│   ├── gamewidget_ui.cpp        # UI更新
│   └── gamewidget_update.cpp    # 游戏循环更新
├── player.*                 # 玩家角色类
├── enemy.*                  # 敌人类
├── bullet.*                 # 子弹类
├── pause.*                  # 暂停界面
├── settings.*               # 设置界面
├── upgradewidget.*          # 升级选择界面
├── resources.qrc            # Qt资源文件
├── images/                  # 图片资源
│   ├── brotato_*.png        # 角色和Logo
│   ├── common*.png          # 敌人图片
│   ├── map.png              # 地图背景
│   └── menu_background.png  # 菜单背景
├── sounds/                  # 音频资源
│   ├── background.wav       # 背景音乐
│   ├── shoot.wav            # 射击音效
│   ├── hit.wav              # 击中音效
│   └── push.wav             # 其他音效
└── build/                   # 构建输出目录
```

---

## 🎨 游戏素材

游戏包含以下素材文件:
- 🎭 **角色图片**: 多种土豆角色外观
- 👾 **敌人图片**: 不同类型的敌人外观
- 🗺️ **背景图片**: 游戏地图和界面背景
- 🎵 **音效文件**: 完整的游戏音效和背景音乐

---

## ⚙️ 核心类说明

### GameWidget
游戏的核心类，管理整个游戏循环和状态。包含以下主要模块:
- 🎧 **音频管理** (gamewidget_audio.cpp)
- 🛡️ **碰撞检测** (gamewidget_collisions.cpp) 
- 🔄 **事件处理** (gamewidget_events.cpp)
- 📊 **状态管理** (gamewidget_state.cpp)
- 🖥️ **UI更新** (gamewidget_ui.cpp)
- 🔄 **游戏更新** (gamewidget_update.cpp)

### Player
玩家角色类，包含:
- ❤️ **生命值**、💥 **攻击力**、🏃 **移动速度**等属性
- ⭐ **经验值和升级系统**
- 🎭 **角色动画和朝向**
- 🔧 **属性提升方法**

### Enemy
敌人类，支持:
- 👾 **多种敌人类型** (Type1, Type2)
- 🌊 **基于波次的属性缩放**
- 🛡️ **自动寻路和攻击**
- ⭐ **击败时给予玩家经验**

### Bullet
子弹类，实现:
- 🔄 **弹道计算和移动**
- 🛡️ **碰撞检测**
- 🧹 **自动清理机制**

---

## 🔧 开发说明

### 编译依赖
项目使用以下 Qt 模块:
- 🖥️ **Qt::Widgets** - GUI界面
- 🎧 **Qt::Multimedia** - 音频播放

### CMake 配置
- ✅ **支持 Qt5 和 Qt6 自动检测**
- ✅ **自动 MOC、UIC、RCC 处理**
- ✅ **跨平台构建支持**

---

## 📝 许可证

本项目采用 **GNU General Public License v3.0** 许可证。详情请参阅 [LICENSE](LICENSE) 文件。

---

## 🤝 贡献

欢迎提交问题报告和功能请求！如果您想为项目做出贡献，请:

1. Fork 本仓库
2. 创建您的功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交您的更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启一个 Pull Request

---

## 📞 联系方式

如有问题或建议，请通过 **GitHub Issues** 联系我们。

---

## 🎥 游戏演示
![游戏截图](images/screenshot.png)

[点击查看演示视频](https://www.youtube.com/watch?v=example)

---

🎮 **享受游戏乐趣! 在土豆兄弟的世界中生存下去!**
