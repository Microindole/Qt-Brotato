#ifndef SHOP_H
#define SHOP_H

#include <QWidget>
#include "player.h"

// 定义商店出售的物品
struct ShopItem {
    QString id;          // 唯一ID
    QString name;        // 名称
    QString description; // 描述
    QString iconPath;    // 图标资源路径
    int price;           // 价格
};

namespace Ui {
class Shop;
}

class Shop : public QWidget
{
    Q_OBJECT

public:
    explicit Shop(QWidget *parent = nullptr);
    ~Shop();

    void showShop(Player* player); // 显示商店时，传入玩家指针以获取金币数量

signals:
    void continueToNextWave(); // "继续"按钮被点击
    void backToMenu();         // "返回主菜单"按钮被点击
    void itemPurchased(const QString& itemId); // 物品被购买时发出信号

private slots:
    void handleContinueClicked();
    void handleBackToMenuClicked();
    void handleItemPurchaseClicked();

private:
    void setupItems();          // 初始化所有可售物品
    void refreshItemList();     // 刷新商品列表（例如，每次进店商品都不同）
    void updateCoinDisplay();   // 更新金币显示

    Ui::Shop *ui;
    QList<ShopItem> m_allItems; // 存储所有可能的商品
    Player* m_player;           // 指向玩家的指针
};

#endif // SHOP_H
