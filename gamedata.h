#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QString>
#include <QObject>
#include "player.h" // 包含Player头文件以使用角色枚举

// 定义地图信息结构体
struct MapInfo {
    QString key;
    QString name;
    QString description;
    QString backgroundPath;
    // 增加更多Buff类型
    enum BuffType { None, HpRegenDebuff, SpeedDebuff, AttackRangeDebuff } buffType = None;
    float buffValue = 0;
};
// 使用 Q_DECLARE_METATYPE 让这个结构体可以在信号/槽中传递
Q_DECLARE_METATYPE(MapInfo)


// 单例类，用于在整个游戏范围内共享数据
class GameData : public QObject
{
    Q_OBJECT
public:
    static GameData& instance() {
        static GameData data;
        return data;
    }

    // DLC 状态
    bool dlc1_purchased = false;
    bool dlc1_active = false;

    // 存储玩家的最终选择
    MapInfo selectedMap;
    Player::CharacterType selectedCharacter = Player::AllRounder; // 默认选择一个

private:
    GameData(QObject* parent = nullptr) : QObject(parent) {} // 构造函数可以是公有的，因为是单例
    GameData(const GameData&) = delete;
    GameData& operator=(const GameData&) = delete;
};

#endif // GAMEDATA_H
