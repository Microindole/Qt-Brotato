#include "gamewidget.h"
#include "resourcemanager.h"
#include <QMediaPlayer>

void GameWidget::setupAudio()
{
    // 游戏背景音乐设置
    backgroundMusic = new QMediaPlayer(this);
    backgroundAudioOutput = new QAudioOutput(this);
    backgroundMusic->setAudioOutput(backgroundAudioOutput);
    backgroundMusic->setSource(QUrl("qrc:/sounds/background.wav"));
    backgroundAudioOutput->setVolume(currentMusicVolume);
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
    ResourceManager::instance().setSfxVolume(currentSfxVolume);
}

void GameWidget::startBackgroundMusic()
{
    if (backgroundMusic->playbackState() != QMediaPlayer::PlayingState) {
        backgroundMusic->setLoops(QMediaPlayer::Infinite);
        backgroundMusic->play();
    }
}

void GameWidget::stopBackgroundMusic()
{
    backgroundMusic->stop();
}

void GameWidget::pauseBackgroundMusic()
{
    // 暂停音乐
    if (backgroundMusic->playbackState() == QMediaPlayer::PlayingState) {
        backgroundMusic->pause();
    }
}

void GameWidget::resumeBackgroundMusic()
{
    // 继续播放音乐
    if (backgroundMusic->playbackState() == QMediaPlayer::PausedState) {
        backgroundMusic->play();
    }
}
