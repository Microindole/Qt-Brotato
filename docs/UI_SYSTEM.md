# 模块详解: UI系统与用户交互

本文档详细解析 `Brotato-Qt` 项目中负责用户交互的各个UI界面模块，包括主菜单、选择界面、商店、暂停和升级界面等。

## 1. 核心设计思想

项目中的UI系统遵循了Qt标准的**模型-视图-控制器(MVC)\**思想的简化实践，即\**UI与逻辑分离**：

- **UI层 (View)**: 由`.ui`文件和对应的`QWidget`子类构成，负责界面的显示和接收用户的原始输入（如点击）。
- **逻辑层 (Controller/Model)**: 由`GameWidget`、`MainWindow`和单例类(`GameData`)等构成，负责处理业务逻辑、维护数据状态，并响应UI层发出的信号。

这种分离使得UI设计和后端逻辑可以独立修改，极大地提高了代码的可维护性。

## 2. 各UI模块详解

### 2.1. `Menu` (主菜单)

- **文件**: `menu.h`, `menu.cpp`, `menu.ui`
- **职责**:
  - 作为应用的入口界面。
  - 提供“开始游戏”、“设置”、“退出”等主要导航功能。
  - 集成并显示DLC购买/激活控件。
- **关键实现**:
  - **动态背景**: `Menu`的构造函数中创建了一个`DynamicBackgroundWidget`实例，并将其设置为底层，实现了带有视差滚动效果的动态背景。
  - **DLC控件管理**: `updateDlcUi()`函数根据`GameData`单例中的`dlc1_purchased`状态，动态地显示“购买DLC”按钮或“激活DLC”复选框，实现了UI与数据状态的同步。
  - **支付交互**: 点击购买按钮后，调用`PaymentManager`单例发起支付流程，并通过信号槽机制处理支付成功/失败的后续UI更新。

### 2.2. `Choose` (角色与地图选择)

- **文件**: `choose.h`, `choose.cpp`, `choose.ui`
- **职责**:
  - 让玩家选择要使用的角色和地图。
  - 根据DLC激活状态，动态显示可用的地图。
  - 实时显示所选地图的预览图和描述信息。
- **关键实现**:
  - **静态UI，动态逻辑**: 界面上的所有角色和地图按钮都是在`.ui`文件中静态创建的。C++代码在运行时（`showEvent`中）根据`GameData::instance().dlc1_active`来决定哪些按钮应该被`setVisible(true/false)`。这种方法比纯代码动态生成UI更稳定、更易于维护。
  - **数据驱动的UI更新**: `updateMapDetails()`函数根据被点击按钮的`mapKey`，从预加载的`m_allMaps`中查找对应的`MapInfo`，并用其数据更新预览图(`mapPreviewLabel`)和描述(`mapDescriptionLabel`)。
  - **默认选择**: 在`setupInitialState()`函数中，通过`ui->allrounderButton->click()`和`ui->mapButton_default->click()`以编程方式模拟点击，确保玩家进入界面时总有一个默认的有效选项，防止了因未选择而直接开始游戏导致的崩溃。

### 2.3. `Shop` (商店)

- **文件**: `shop.h`, `shop.cpp`, `shop.ui`
- **职责**:
  - 在每波战斗结束后，为玩家提供一个购买道具的界面。
  - 根据玩家拥有的金币数量，动态更新购买按钮的可点击状态。
  - 支持刷新商品列表。
- **关键实现**:
  - **商品池与随机刷新**: `setupItems()`函数在内部定义了一个包含所有可能商品的`QList<ShopItem>`。`refreshItemList()`函数会在每次进入商店时，对这个列表进行随机排序（`std::shuffle`），然后取出前4个商品动态创建UI并显示。
  - **动态UI生成**: 与`Choose`界面不同，商店的商品项是完全由代码在`refreshItemList()`中动态创建的（包括`QWidget`卡片、`QLabel`、`QPushButton`），并添加到`QGridLayout`中。
  - **状态同步**: `updateButtonsState()`和`updateCoinDisplay()`函数确保了UI（如金币数量、按钮可用性）与玩家的实时数据(`m_player->getCoins()`)保持同步。

### 2.4. `Pause` (暂停菜单)

- **文件**: `pause.h`, `pause.cpp`, `pause.ui`
- **职责**:
  - 在游戏暂停时显示，提供“继续”、“重开”、“设置”、“返回主菜单”等选项。
  - 显示玩家当前的各项统计数据。
- **关键实现**:
  - **上下文相关的UI**: `Pause`类提供了`showForPause()`和`showForWaveComplete()`两个不同的函数。`GameWidget`会根据当前是“手动暂停”还是“波次完成”，调用不同的函数来显示不同的标题和按钮组合，实现了UI的复用。
  - **数据展示**: `updateStats()`函数接收一个`Player`对象的指针，并用其数据更新UI上的各个`QLabel`，展示玩家的实时属性。

### 2.5. `UpgradeWidget` (升级选择)

- **文件**: `upgradewidget.h`, `upgradewidget.cpp`, `upgradewidget.ui`
- **职责**:
  - 在玩家升级时，提供一个全屏的、模态的升级选项界面。
  - 随机展示几个升级选项供玩家选择。
- **关键实现**:
  - **随机选项**: `GameWidget`在调用`showUpgradeMenu()`时，会先创建一个包含所有可能升级项的列表，使用`std::shuffle`打乱顺序，然后将前几个选项传递给`upgradeWidget->setUpgradeOptions()`。
  - **信号传递**: 当玩家点击某个升级按钮后，`UpgradeWidget`会发出`upgradeSelected(UpgradeType)`信号，并将对应的升级类型作为参数传递出去。`GameWidget`接收到这个信号后，为玩家应用实际的属性加成。

## 3. 应用的Qt知识点

- **UI与逻辑分离**: 这是整个UI系统设计的核心原则。
- **`QWidget`子类化**: 每个UI界面都是一个`QWidget`的子类，实现了高度的封装和复用。
- **布局管理器**: `QVBoxLayout`, `QHBoxLayout`, `QGridLayout`的熟练使用。
- **动态UI控制**: 通过`setVisible()`, `setEnabled()`, `setText()`等函数，在运行时根据数据状态动态改变UI的外观和行为。
- **信号与槽**: 在UI控件和业务逻辑之间、以及不同UI模块之间建立通信。
- **资源管理**: 通过`ResourceManager`加载所有UI所需的图标和图片。