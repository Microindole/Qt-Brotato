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

    // 定义所有需要预加载的地图图片路径
    const QStringList mapPaths = {
        ":/images/map.png",
        ":/images/map2_cave.png",
        ":/images/map3_desert.png",
        ":/images/map4_lava.png"
    };

    // 循环遍历并预加载所有地图
    for (const QString& path : mapPaths) {
        QPixmap originalMap(path);
        if (!originalMap.isNull()) {
            // 将原始大小的图片存入缓存
            m_pixmapCache[path] = originalMap;
        } else {
            qWarning() << "ResourceManager: Failed to preload map:" << path;
        }
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

