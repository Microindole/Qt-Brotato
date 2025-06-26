#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QPixmap>
#include <QObject>

class Player : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

public:
    // 角色类型的枚举
    enum CharacterType {
        AllRounder, // 全能
        Fighter,    // 斗士
        Doctor,     // 医生
        Bull        // 公牛
    };

    explicit Player(CharacterType type = AllRounder);
    
    // 属性获取器
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    float getSpeed() const { return speed; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    int getExpToNextLevel() const { return expToNextLevel; }
    float getHealthRegen() const { return healthRegen; }
    int getAttackPower() const { return attackPower; }
    int getArmor() const { return armor; }
    int getAttackRange() const { return attackRange; }
    bool showHealthBar = true;
    int getCoins() const;
    void addCoins(int amount);

    // 功能
    void takeDamage(int damage);
    void heal(int amount);
    void gainExperience(int exp);
    void regenerateHealth(); 
    void setFacingDirection(bool right); 
    void setMoving(bool moving);
    bool isMoving() const { return moving; }
    
    // 属性提升方法
    void increaseMaxHealth(int amount);
    void increaseAttackPower(int amount);
    void increaseSpeed(float amount);
    void increaseHealthRegen(float amount);
    void increaseAttackRange(float multiplier);

    // 动画相关
    void advance(int phase) override;
    QRectF boundingRect() const override;

signals:
    void levelUpOccurred();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    void levelUp();
    int calculateExpForLevel(int level);
    void initializeStats(CharacterType type);

    // 属性
    int health;
    int maxHealth;
    float speed;
    int level;
    int experience;
    int expToNextLevel;
    float healthRegen;
    float healthRegenAccumulator;
    int attackPower;
    int armor;
    int attackRange = 3000;
    int coins;
    
    // 动画
    QPixmap bodyPixmap;
    QPixmap footPixmap;
    qreal animationCounter;
    bool facingRight;
    bool moving = false;
};

#endif // PLAYER_H
