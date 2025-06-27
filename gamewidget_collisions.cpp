#include "gamewidget.h"
#include "bullet.h"
#include "enemy.h"
#include "coin.h"
#include "resourcemanager.h"

void GameWidget::checkCollisions()
{
    if (!player) return;

    // 子弹碰撞检测
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); ) {
        Bullet* bullet = *bulletIt;
        bool bulletRemoved = false;

        if (bullet->getOwner() == Bullet::PlayerBullet) {
            // 玩家的子弹 -> 检测与敌人的碰撞
            for (auto enemyIt = enemies.begin(); enemyIt != enemies.end(); ++enemyIt) {
                Enemy* enemy = *enemyIt;
                if (bullet->collidesWithItem(enemy)) {
                    enemy->takeDamage(player->getAttackPower()); // 伤害来自玩家
                    ResourceManager::instance().playSound("hit");

                    // 子弹击中后应被移除
                    gameScene->removeItem(bullet);
                    delete bullet;
                    bulletIt = bullets.erase(bulletIt);
                    bulletRemoved = true;
                    break; // 一颗子弹只击中一个敌人
                }
            }
        } else { // bullet->getOwner() == Bullet::EnemyBullet
            // 敌人的子弹 -> 检测与玩家的碰撞
            if (bullet->collidesWithItem(player)) {
                player->takeDamage(bullet->getDamage()); // 伤害来自子弹自身
                ResourceManager::instance().playSound("hit");

                // 子弹击中后应被移除
                gameScene->removeItem(bullet);
                delete bullet;
                bulletIt = bullets.erase(bulletIt);
                bulletRemoved = true;
            }
        }

        if (!bulletRemoved) {
            ++bulletIt; // 如果子弹没有被移除，则迭代器前进
        }
    }

    // 死亡敌人处理
    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end(); ) {
        if ((*enemyIt)->isDead()) {
            player->gainExperience((*enemyIt)->getExperienceValue());
            score += 5;
            enemiesKilled++;
            spawnCoin((*enemyIt)->pos());
            gameScene->removeItem(*enemyIt);
            delete *enemyIt;
            enemyIt = enemies.erase(enemyIt);
        } else {
            ++enemyIt;
        }
    }

    // 敌人与玩家近战碰撞
    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end(); ) {
        if ((*enemyIt)->collidesWithItem(player)) {
            player->takeDamage((*enemyIt)->getDamage());
            ResourceManager::instance().playSound("hit");

            // 敌人撞到玩家后死亡
            gameScene->removeItem(*enemyIt);
            delete *enemyIt;
            enemyIt = enemies.erase(enemyIt);
        } else {
            ++enemyIt;
        }
    }

    // 检查玩家是否死亡
    if (player->getHealth() <= 0) {
        gameOver();
        return;
    }

    // 玩家与金币碰撞
    for (auto coinIt = coins.begin(); coinIt != coins.end();) {
        if (player->collidesWithItem(*coinIt)) {
            player->addCoins((*coinIt)->getValue());
            ResourceManager::instance().playSound("coin");
            gameScene->removeItem(*coinIt);
            delete *coinIt;
            coinIt = coins.erase(coinIt);
        } else {
            ++coinIt;
        }
    }
}

void GameWidget::cleanupObjects()
{
    QRectF sceneBounds = gameScene->sceneRect().adjusted(-100, -100, 100, 100);
    for (auto it = bullets.begin(); it != bullets.end();) {
        if (!sceneBounds.contains((*it)->pos())) {
            gameScene->removeItem(*it);
            delete *it;
            it = bullets.erase(it);
        } else { ++it; }
    }
}
