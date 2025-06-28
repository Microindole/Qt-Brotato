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
    bool isUnique = false; // 标记是否为一次性购买
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

    void showShop(Player* player);
    void clearPurchaseHistory();
    void prepareForNewVisit();

signals:
    void continueToNextWave();
    void backToMenu();
    void itemPurchased(const QString& itemId);

private slots:
    void handleContinueClicked();
    void handleBackToMenuClicked();
    void handleItemPurchaseClicked();
    void handleRefreshClicked();

private:
    void setupItems();          // 初始化所有可售物品
    void refreshItemList();     // 刷新商品列表（例如，每次进店商品都不同）
    void updateCoinDisplay();   // 更新金币显示
    void updateButtonsState();

    Ui::Shop *ui;
    QList<ShopItem> m_allItems;
    Player* m_player;
    QSet<QString> m_purchasedUniqueItems;
    bool m_isNewVisit;
};

#endif // SHOP_H
