#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QPixmap>

class Player : public QGraphicsEllipseItem
{
public:
    Player();
    
    // 基础属性获取器
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    float getSpeed() const { return speed; }
    
    // 新增属性获取器
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    int getExpToNextLevel() const { return expToNextLevel; }
    float getHealthRegen() const { return healthRegen; }
    int getAttackPower() const { return attackPower; }
    int getArmor() const { return armor; }
    int getAttackRange() const { return attackRange; } // <-- 新增：获取攻击距离

    // 基础功能
    void takeDamage(int damage);
    void heal(int amount);
    
    // 新增功能
    void gainExperience(int exp);
    void levelUp();
    void regenerateHealth(); // 生命再生逻辑
    void setFacingDirection(bool right); // <-- 新增：设置朝向的方法
    void setMoving(bool moving);
    bool isMoving() const { return moving; }
    void increaseAttackRange(float multiplier); // 提升攻击距离的方法

    // 动画相关
    void advance(int phase) override;
    QRectF boundingRect() const override;
    
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    // 基础属性
    int health;
    int maxHealth;
    float speed;
    
    // 新增属性
    int level;              // 等级
    int experience;         // 当前经验
    int expToNextLevel;     // 升级所需经验
    float healthRegen;      // 生命再生（每秒回复量）
    int attackPower;        // 攻击力
    int armor;              // 护甲（减伤百分比）
    int attackRange = 3000;  // 默认攻击距离为 100 像素
    
    // 动画相关
    QPixmap bodyPixmap;     // 身体图片
    QPixmap footPixmap;     // 脚部图片
    qreal animationCounter; // 动画计数器
    bool facingRight; // <-- 新增：记录当前朝向 (true=右, false=左)
    bool moving = false;

    // 辅助函数
    int calculateExpForLevel(int level); // 计算升级所需经验
};

#endif // PLAYER_H
