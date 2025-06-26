#include "gamewidget.h"
#include "bullet.h"
#include "enemy.h"
#include "coin.h"
#include "resourcemanager.h"

void GameWidget::checkCollisions()
{
    if (!player) return;

    // 子弹与敌人碰撞
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        bool bulletHit = false;
        for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
            if ((*bulletIt)->collidesWithItem(*enemyIt)) {
                (*enemyIt)->takeDamage(player->getAttackPower());
                ResourceManager::instance().playSound("hit");
                
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
                
                bulletHit = true;
                break;
            } else { 
                ++enemyIt; 
            }
        }
        if (bulletHit) {
            gameScene->removeItem(*bulletIt);
            delete *bulletIt;
            bulletIt = bullets.erase(bulletIt);
        } else { 
            ++bulletIt; 
        }
    }

    // 敌人与玩家碰撞
    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
        if ((*enemyIt)->collidesWithItem(player)) {
            player->takeDamage((*enemyIt)->getDamage());
            ResourceManager::instance().playSound("hit");
            if (player->getHealth() <= 0) {
                gameOver();
                return;
            }
            gameScene->removeItem(*enemyIt);
            delete *enemyIt;
            enemyIt = enemies.erase(enemyIt);
        } else { 
            ++enemyIt; 
        }
    }

    // 玩家与金币碰撞
    for (auto coinIt = coins.begin(); coinIt != coins.end();) {
        if (player->collidesWithItem(*coinIt)) {
            // 玩家拾取金币
            player->addCoins((*coinIt)->getValue());
            ResourceManager::instance().playSound("coin"); // 播放音效

            // 从场景和列表中移除金币
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
