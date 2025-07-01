# 模块详解: 游戏实体设计 (Player, Enemy, Boss)

本文档详细解析 `Brotato-Qt` 项目中核心游戏实体（`Player`, `Enemy`, `Boss`）的设计与实现。这些类是构成游戏世界动态元素的基础。

## 1. 核心设计思想

- **继承与多态**: `Boss` 类继承自 `Enemy` 类，`Enemy` 又继承自 `QObject` 和 `QGraphicsEllipseItem`。这种设计利用了C++的继承和多态特性，使得 `Boss` 可以重用 `Enemy` 的基础属性和功能（如生命值、受伤），同时又能通过重写虚函数（如 `moveTowards`, `paint`）来实现自己独特的行为和外观。
- **数据驱动**: 游戏对象的初始属性（如生命值、攻击力）大部分是在构造函数中根据传入的参数（如角色类型、波数）来设置的，体现了一定的数据驱动思想。
- **组件化行为**: 敌人的特殊行为（如冲锋、远程攻击）是通过在内部组合`QTimer`来实现的，这是一种简单的组件化思想，将特定行为逻辑封装在定时器的槽函数中。

## 2. Player (玩家类)

`Player` 类代表了玩家控制的角色，是游戏中交互最复杂的实体。

### 文件组成

- `player.h`: 定义了 `Player` 类的接口、属性、信号以及角色类型的枚举 `CharacterType`。
- `player.cpp`: 实现了 `Player` 类的所有功能。

### 关键实现分析

- **双重继承**: `class Player : public QObject, public QGraphicsEllipseItem`
  - 继承自 `QObject` 是为了能够使用Qt的信号与槽机制，例如，当玩家升级时可以 `emit levelUpOccurred();`。
  - 继承自 `QGraphicsEllipseItem` 使其成为一个可以被添加到`QGraphicsScene`中的图形项，并提供了基础的形状（椭圆）用于碰撞检测。
- **属性系统**: 包含了生命、速度、等级、经验、攻击力、护甲等所有核心战斗属性。提供了丰富的`increase...`和`multiply...`方法用于升级和道具加成。
- **自定义绘制 (`paint`)**: `paint()` 方法是玩家视觉表现的核心。
  - 它根据 `facingRight` 标志位，使用 `painter->scale(-1, 1)` 来实现角色的水平翻转。
  - 根据 `moving` 标志位，选择不同的动画参数（如身体浮动幅度和缩放频率），实现了静止“呼吸”和移动“奔跑”两种不同的动画效果。
  - 分层绘制：先绘制脚，再绘制身体，保证了正确的遮挡关系。
  - 动态血条：在生命值非满时，在角色头顶动态计算并绘制血条。
- **动画 (`advance`)**: `advance()` 方法由游戏主循环驱动，仅用于更新内部的 `animationCounter` 动画计数器，并请求重绘，从而实现持续的动画效果，即使玩家静止不动。
- **生命恢复/腐蚀 (`regenerateHealth`)**: 该函数设计得非常巧妙，通过一个浮点数累加器 `healthRegenAccumulator`，可以精确地处理小数形式的生命恢复或腐蚀效果（如熔岩地图的-0.5/秒）。

## 3. Enemy (敌人基类)

`Enemy` 是所有非Boss敌人的基类，定义了敌人的通用行为和属性。

### 文件组成

- `enemy.h`: 定义了 `Enemy` 类的接口，以及敌人类型的枚举 `EnemyType`。
- `enemy.cpp`: 实现了通用敌人的逻辑。

### 关键实现分析

- **构造与生成**: 在构造函数中，根据传入的 `waveNumber`（波数）和随机生成的 `EnemyType`，为不同类型的敌人设置不同的初始属性（血量、速度、伤害）和成长系数。这使得游戏难度可以随波次动态提升。
- **虚函数**: `moveTowards`, `getDamage`, `takeDamage` 等函数被声明为 `virtual`，这使得派生类 `Boss` 可以重写这些函数，实现自己的移动和伤害逻辑，这是多态的核心应用。
- **状态机AI**:
  - **冲锋怪 (`Charger`)**: 使用一个`QTimer` (`chargeTimer`) 和一个枚举 `ChargeState` (`Idle`, `Preparing`, `Charging`, `Cooldown`) 实现了一个完整的冲锋攻击状态机。通过定时器切换状态，实现了“准备-冲刺-冷却”的连贯行为。
  - **远程怪 (`Ranged`)**: 移动逻辑特殊，会试图与玩家保持一个“首选距离” `preferredDistance`，太远则靠近，太近则后退。攻击行为由另一个`QTimer` (`rangedAttackTimer`) 驱动，周期性地发射 `fireBullet` 信号。

## 4. Boss (Boss类)

`Boss` 继承自 `Enemy`，是一个更复杂、拥有阶段性行为的特殊敌人。

### 文件组成

- `boss.h`: 定义了 `Boss` 类的接口，和它独有的状态枚举与信号。
- `boss.cpp`: 实现了 `Boss` 的特殊逻辑。

### 关键实现分析

- **重写基类函数**: `Boss` 重写了 `moveTowards` 和 `takeDamage`。
  - `moveTowards`: 实现了比普通敌人更复杂的移动AI。
  - `takeDamage`: 在调用基类受伤逻辑的基础上，增加了血量判断，当血量低于50%时，触发 `enterEnragedState()` 进入二阶段。
- **二阶段状态机**:
  - **普通阶段**: 周期性地执行三连发攻击。
  - **狂暴阶段 (`m_isEnraged`)**: 当血量低于一半时触发，Boss移速增加，攻击模式变为“向玩家冲刺+释放弹幕”，攻击性和威胁度都大幅提升。
- **自定义信号**: 定义了 `fireRadialShot(int bulletCount)` 信号，用于通知 `GameWidget` 生成弹幕攻击，实现了Boss特殊技能与游戏逻辑的解耦。

## 5. 应用的Qt知识点

- **面向对象编程**: 继承、多态、封装在这些实体类中得到了充分的体现。
- **`QGraphicsItem`体系**: `QGraphicsEllipseItem`, `QGraphicsPixmapItem` 的继承与使用。
- **自定义绘制**: `QPainter` 的高级应用，包括坐标变换（`scale`, `translate`, `rotate`）、透明度设置（`setOpacity`）、复杂图形绘制等。
- **`QTimer`**: 用于实现AI行为的定时触发和状态切换。
- **信号与槽**: 用于敌人（远程怪、Boss）向 `GameWidget` 发出“请求开火”的信号。