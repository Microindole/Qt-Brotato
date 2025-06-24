#include "gamewidget.h"
#include "bullet.h"
#include "enemy.h"

void GameWidget::checkCollisions()
{
    if (!player) return;

    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        bool bulletHit = false;
        for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
            if ((*bulletIt)->collidesWithItem(*enemyIt)) {
                (*enemyIt)->takeDamage(10);
                playHitSound();
                
                if ((*enemyIt)->isDead()) {
                    gameScene->removeItem(*enemyIt);
                    delete *enemyIt;
                    enemyIt = enemies.erase(enemyIt);
                    score += 5;
                    enemiesKilled++;
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

    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
        if ((*enemyIt)->collidesWithItem(player)) {
            player->takeDamage((*enemyIt)->getDamage());
            playHitSound();
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