#include <qbgfx.h>

#include "qquick_bgfx.h"
#include "qquickbgfxitem/qquickbgfxitem.h"

#include <bgfx/bgfx.h>

#include <QList>
#include <QQuickWindow>
//#include <QGuiApplication>
#include <stdexcept>

using namespace QQuickBgfx;

QBgfx::QBgfx(QQuickWindow *w, const QList<QQuickBgfxItem *> items): m_window(w)
{
    //Qt::DirectConnection needs to be specified in order to call the slot from the signal thread
    connect(m_window, &QQuickWindow::beforeFrameBegin, this, &QBgfx::init, Qt::DirectConnection);
    connect(m_window, &QQuickWindow::beforeRenderPassRecording, this, &QBgfx::renderFrame, Qt::DirectConnection);
    connect(m_window, &QQuickWindow::beforeSynchronizing, this, &QBgfx::synchronize, Qt::DirectConnection);

    //Free standing function instead will always be called from the signal thread
    connect(m_window, &QQuickWindow::afterRenderPassRecording, this, &QBgfx::requestSwap, Qt::DirectConnection);

    //    connect(QGuiApplication::instance(), &QGuiApplication::aboutToQuit, this, &Renderer::shutdown, Qt::QueuedConnection);

    m_bgfxItems.reserve(m_bgfxItems.size());
    m_bgfxItems.insert(m_bgfxItems.end(), items.begin(), items.end());
}
void QBgfx::synchronize()
{
    emit sync(m_bgfxItems, static_cast<QObject*>(m_window->findChildren<QQuickItem*>("main")[0]));
}

void QBgfx::requestSwap()
{
    emit swap(m_bgfxItems, m_window);
}

QBgfx::~QBgfx()
{
    //    shutdown();
}

#if defined(QQ_ENABLE_OPENGL)
    #include <QOpenGLContext>
#endif

void QBgfx::init()
{
    QSGRendererInterface *rif = m_window->rendererInterface();
    const auto dpr = m_window->effectiveDevicePixelRatio();
    auto winHandle = reinterpret_cast<void *>(m_window->winId());

    void* context = nullptr;

    #if defined(QQ_ENABLE_METAL) || defined(QQ_ENABLE_DIRECTX)
        context = static_cast<void *>(rif->getResource(m_window, QSGRendererInterface::DeviceResource));
    #elif defined(QQ_ENABLE_OPENGL)
        auto qtcontext = reinterpret_cast<QOpenGLContext*>(rif->getResource(m_window, QSGRendererInterface::OpenGLContextResource));

        #if defined(_WIN32)
        auto ninterface = qtcontext->nativeInterface<QNativeInterface::QWGLContext>()->nativeContext();
        #elif defined(__linux__)
        auto ninterface = qtcontext->nativeInterface<QNativeInterface::QGLXContext>()->nativeContext();
        #elif defined(__APPLE__)
        auto ninterface = qtcontext->nativeInterface<QNativeInterface::QCocoaGLContext>()->nativeContext();
        #endif

        context = reinterpret_cast<void *>(ninterface);
    #endif

    bgfx::RendererType::Enum gaphicsApi{bgfx::RendererType::Count};
    switch (rif->graphicsApi())
    {
        case QSGRendererInterface::MetalRhi:
            #if defined(QQ_ENABLE_METAL)
                gaphicsApi = bgfx::RendererType::Metal;
            #else
                BX_ASSERT(false, "METAL not enabled.");
            #endif
            break;
        case QSGRendererInterface::Direct3D11:
            #if defined(QQ_ENABLE_DIRECTX)
                gaphicsApi = bgfx::RendererType::Direct3D11;
            #else
                BX_ASSERT(false, "DirectX not enabled.");
            #endif
            break;
        case QSGRendererInterface::OpenGLRhi:
            #if defined(QQ_ENABLE_OPENGL)
                gaphicsApi = bgfx::RendererType::OpenGL;
            #else
                BX_ASSERT(false, "OpenGL not enabled.");
            #endif
        default:
            break;
    }
    m_bgfxInit =
      QQuickBgfx::initBackend(gaphicsApi, winHandle, context, m_window->width() * dpr, m_window->height() * dpr);

    emit initialized(m_bgfxInit, m_window);
}

void QBgfx::renderFrame()
{
    if (!QQuickBgfx::initialized())
        return;

    m_window->beginExternalCommands();
    emit render(m_bgfxItems, m_window);
    m_window->endExternalCommands();
}

void QBgfx::shutdown()
{
    if (QQuickBgfx::initialized())
    {
        bgfx::shutdown();
    }
    m_bgfxItems.clear();
}
