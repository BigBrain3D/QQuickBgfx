#pragma once
#include "qquickbgfxitem/qquickbgfxitem.h"

#include <bgfx/platform.h>

#include <QObject>

#include <vector>

class QQmlEngine;
class QQuickWindow;

namespace QQuickBgfx {
class QBgfx : public QObject
{
    Q_OBJECT
public:
    QBgfx() = delete;
    QBgfx(QQuickWindow *, const QList<QQuickBgfxItem *>);

    ~QBgfx();

private slots:

    void renderFrame();
    void shutdown();
    void init();
    void synchronize();
    void requestSwap();

signals:
    void render(const std::vector<QQuickBgfxItem *> &, QQuickWindow* windowPtr);
    void initialized(bgfx::Init &, QQuickWindow* windowPtr);
    void sync(const std::vector<QQuickBgfxItem*>& bgfxItems, QQuickWindow* windowPtr);
    void swap(const std::vector<QQuickBgfxItem*>& bgfxItems, QQuickWindow* windowPtr);

private:
    std::vector<QQuickBgfxItem *> m_bgfxItems;
    QQuickWindow *m_window{nullptr};
    bgfx::Init m_bgfxInit;
};
}    // namespace QQuickBgfx
