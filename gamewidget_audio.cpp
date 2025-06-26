#include "gamewidget.h"

void GameWidget::setupAudio()
{
    // 游戏背景音乐设置
    backgroundMusic = new QMediaPlayer(this);
    backgroundAudioOutput = new QAudioOutput(this);
    backgroundMusic->setAudioOutput(backgroundAudioOutput);
    backgroundMusic->setSource(QUrl("qrc:/sounds/background.wav"));
    backgroundAudioOutput->setVolume(currentMusicVolume);

    // 射击音效设置
    shootSound = new QMediaPlayer(this);
    shootAudioOutput = new QAudioOutput(this);
    shootSound->setAudioOutput(shootAudioOutput);
    shootSound->setSource(QUrl("qrc:/sounds/shoot.wav"));
    shootAudioOutput->setVolume(currentSfxVolume);

    // 攻击音效设置
    hitSound = new QMediaPlayer(this);
    hitAudioOutput = new QAudioOutput(this);
    hitSound->setAudioOutput(hitAudioOutput);
    hitSound->setSource(QUrl("qrc:/sounds/hit.wav"));
    hitAudioOutput->setVolume(currentSfxVolume);
    
    // 游戏结束音效设置
    gameOverSound = new QMediaPlayer(this);
    gameOverAudioOutput = new QAudioOutput(this);
    gameOverSound->setAudioOutput(gameOverAudioOutput);
    gameOverSound->setSource(QUrl("qrc:/sounds/push.wav"));
    gameOverAudioOutput->setVolume(currentSfxVolume);

    // 金币音效设置
    coinPickupSound = new QMediaPlayer(this);
    coinPickupAudioOutput = new QAudioOutput(this);
    coinPickupSound->setAudioOutput(coinPickupAudioOutput);
    coinPickupSound->setSource(QUrl("qrc:/sounds/push.wav"));
    coinPickupAudioOutput->setVolume(currentSfxVolume);
}

void GameWidget::setMusicVolume(float volume)
{
    currentMusicVolume = volume;
    if (backgroundAudioOutput) {
        backgroundAudioOutput->setVolume(currentMusicVolume);
    }
}

void GameWidget::setSfxVolume(float volume)
{
    currentSfxVolume = volume;
    if (shootAudioOutput) {
        shootAudioOutput->setVolume(currentSfxVolume);
    }
    if (hitAudioOutput) {
        hitAudioOutput->setVolume(currentSfxVolume);
    }
    if (gameOverAudioOutput) {
        gameOverAudioOutput->setVolume(currentSfxVolume);
    }
    if (coinPickupAudioOutput) {
        coinPickupAudioOutput->setVolume(currentSfxVolume);
    }
}

void GameWidget::playShootSound()
{
    if (shootSound->playbackState() == QMediaPlayer::PlayingState) {
        shootSound->setPosition(0);
    } else {
        shootSound->play();
    }
}

void GameWidget::playHitSound()
{
    if (hitSound->playbackState() == QMediaPlayer::PlayingState) {
        hitSound->setPosition(0);
    } else {
        hitSound->play();
    }
}

void GameWidget::playGameOverSound()
{
    gameOverSound->play();
}

void GameWidget::playCoinPickupSound()
{
    // 不重置播放状态，允许快速连续拾取时音效叠加
    if (coinPickupSound->playbackState() == QMediaPlayer::PlayingState) {
        coinPickupSound->setPosition(0);
    }
    coinPickupSound->play();
}

void GameWidget::startBackgroundMusic()
{
    backgroundMusic->setLoops(QMediaPlayer::Infinite);
    backgroundMusic->play();
}

void GameWidget::stopBackgroundMusic()
{
    backgroundMusic->stop();
}
