#include "resourcemanager.h"
#include <QDebug>
#include <QUrl>
#include <QSize>

// 返回单例实例的引用
ResourceManager& ResourceManager::instance()
{
    static ResourceManager instance;
    return instance;
}

// 私有构造函数，在第一次调用instance()时执行
ResourceManager::ResourceManager(QObject *parent)
    : QObject(parent)
{
    // 在构造时执行所有资源的预加载
    preloadAssets();
}

// 析构函数，负责清理所有音效对象
ResourceManager::~ResourceManager()
{
    qDeleteAll(m_soundEffectCache);
    m_soundEffectCache.clear();
}

// 预加载函数，执行所有耗时操作和资源初始化
void ResourceManager::preloadAssets()
{
    qInfo() << "ResourceManager: Preloading assets...";

    // 预加载并缓存缩放后的背景图，解决启动卡顿
    QPixmap originalBg(":/images/map.png");
    if (!originalBg.isNull()) {
        QPixmap scaledBg = originalBg.scaled(1920, 1080, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_pixmapCache[":/images/map.png"] = scaledBg;
    } else {
        qWarning() << "ResourceManager: Failed to preload background map.";
    }

    // 预加载所有音效
    m_soundEffectCache["shoot"] = new QSoundEffect(this);
    m_soundEffectCache["shoot"]->setSource(QUrl::fromLocalFile(":/sounds/shoot.wav"));

    m_soundEffectCache["enemy_shoot"] = new QSoundEffect(this);
    m_soundEffectCache["enemy_shoot"]->setSource(QUrl::fromLocalFile(":/sounds/push.wav"));


    m_soundEffectCache["hit"] = new QSoundEffect(this);
    m_soundEffectCache["hit"]->setSource(QUrl::fromLocalFile(":/sounds/hit.wav"));

    m_soundEffectCache["gameover"] = new QSoundEffect(this);
    m_soundEffectCache["gameover"]->setSource(QUrl::fromLocalFile(":/sounds/push.wav"));

    m_soundEffectCache["coin"] = new QSoundEffect(this);
    m_soundEffectCache["coin"]->setSource(QUrl::fromLocalFile(":/sounds/push.wav"));
    setSfxVolume(0.3f); // 设置一个默认音效音量
    qInfo() << "ResourceManager: Assets preloaded.";
}

// 获取图片的公共接口
QPixmap ResourceManager::getPixmap(const QString& path)
{
    // 如果缓存中已有，直接从内存返回，速度极快
    if (m_pixmapCache.contains(path)) {
        return m_pixmapCache.value(path);
    }

    // 如果缓存中没有，从磁盘加载
    QPixmap pixmap(path);
    if (pixmap.isNull()) {
        qWarning() << "ResourceManager: Failed to load pixmap from" << path;
        return QPixmap(); // 返回一个空的Pixmap
    }

    // 将新加载的图片存入缓存，以便下次使用
    m_pixmapCache.insert(path, pixmap);
    return pixmap;
}

// 播放音效的公共接口
void ResourceManager::playSound(const QString& name)
{
    if (m_soundEffectCache.contains(name)) {
        m_soundEffectCache.value(name)->play();
    } else {
        qWarning() << "ResourceManager: Sound not found:" << name;
    }
}

// 设置音量的公共接口
void ResourceManager::setSfxVolume(float volume)
{
    for(QSoundEffect *effect : std::as_const(m_soundEffectCache)) {
        effect->setVolume(volume);
    }
}

