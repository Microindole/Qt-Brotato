#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include <QPixmap>
#include <QSoundEffect>
#include <QMap>
#include <QString>

// 资源管理器，使用单例模式，确保全局只有一个实例
class ResourceManager : public QObject
{
    Q_OBJECT
public:
    // 获取单例实例的静态方法
    static ResourceManager& instance();

    // 获取图片资源（自动处理加载和缓存）
    QPixmap getPixmap(const QString& path);

    // 播放音效
    void playSound(const QString& name);

    // 设置所有音效的音量
    void setSfxVolume(float volume);

private:
    // 私有化构造函数和析构函数，以实现单例模式
    explicit ResourceManager(QObject *parent = nullptr);
    ~ResourceManager();

    // 禁用拷贝构造和赋值操作
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // 用于预加载资源的私有函数
    void preloadAssets();

    // 缓存容器
    QMap<QString, QPixmap> m_pixmapCache;
    QMap<QString, QSoundEffect*> m_soundEffectCache;
};

#endif // RESOURCEMANAGER_H
