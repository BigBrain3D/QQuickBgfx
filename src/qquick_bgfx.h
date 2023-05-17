#pragma once
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>

#include <QOpenGLContext>
#include <QDebug>

class QSGTexture;
class QQuickWindow;

namespace QQuickBgfx {
    struct TextureHandles
    {
        QSGTexture *qsgTextue{nullptr};
        uintptr_t nativeTextureHandle{};
    };

    #if defined(QQ_ENABLE_METAL)
        bgfx::Init initMetalBackend(void *windowHandle, void *context, const uint16_t width, const uint16_t height);
        TextureHandles CreateQSGMetalTexture(QQuickWindow *window, int w, int h);
    #endif

    #if defined(QQ_ENABLE_DIRECTX)
        bgfx::Init initD3D11Backend(void *windowHandle, void *context, const uint16_t width, const uint16_t height);
        TextureHandles CreateQSGD3D11Texture(QQuickWindow *window, int w, int h);
    #endif

    #if defined(QQ_ENABLE_OPENGL)
        bgfx::Init initGLBackend(void* windowHandle, void* context, const uint16_t width, const uint16_t height);
        TextureHandles CreateGLTexture(QQuickWindow *window, int w, int h, QOpenGLContext* context);
    #endif

    inline bgfx::Init initBackend(bgfx::RendererType::Enum graphicsApi, void *windowHandle, void *context,
                                const uint16_t width, const uint16_t height)
    {
        switch (graphicsApi)
        {
            case bgfx::RendererType::Metal:
    #if defined(QQ_ENABLE_METAL)
                qInfo() << "Initializing Metal.";

                return initMetalBackend(windowHandle, context, width, height);
    #else
                BX_ASSERT(false, "Metal is not enabled. Enable it by defining QQ_ENABLE_METAL.");
    #endif
                break;
            case bgfx::RendererType::Direct3D11:
    #if defined(QQ_ENABLE_DIRECTX)
                return QQuickBgfx::initD3D11Backend(windowHandle, context, width, height);
    #else
                BX_ASSERT(false, "DirectX is not enabled. Enable it by defining QQ_ENABLE_DIRECTX.");
    #endif
                break;
            case bgfx::RendererType::OpenGL:
    #if defined(QQ_ENABLE_OPENGL)
                return QQuickBgfx::initGLBackend(windowHandle, context, width, height);
    #else
                BX_ASSERT(false, "OpenGL is not enabled. Enable it by defining QQ_ENABLE_OPENGL.");
    #endif
                break;
            default:
                BX_ASSERT(false, "Invalid or not implemented Graphics API.");
                break;
        }
        return {};
    }

    inline TextureHandles CreateQSGTexture(QQuickWindow *window, int w, int h, QOpenGLContext* context = nullptr)
    {
        switch (bgfx::getRendererType())
        {
            case bgfx::RendererType::Metal:
    #if defined(QQ_ENABLE_METAL)
                return CreateQSGMetalTexture(window, w, h);
    #else
                BX_ASSERT(false, "Metal is not enabled. Enable it by defining QQ_ENABLE_METAL.");
    #endif
                break;
            case bgfx::RendererType::Direct3D11:
    #if defined(QQ_ENABLE_DIRECTX)
                return CreateQSGD3D11Texture(window, w, h);
    #else
                BX_ASSERT(false, "Direct3D is not enabled. Enable it by defining QQ_ENABLE_DIRECTX.");
    #endif
                break;
            case bgfx::RendererType::OpenGL:
    #if defined(QQ_ENABLE_OPENGL)
                return CreateGLTexture(window, w, h, context);
    #else
                BX_ASSERT(false, "OPENGL is not enabled. Enable it by defining QQ_ENABLE_OPENGL.");
    #endif
                break;
            default:
                BX_ASSERT(false, "Invalid or not implemented Graphics Api");
                break;
        }
        return {};
    }

    inline bool initialized()
    {
        return bgfx::getInternalData()->context;
    }

    inline void frame()
    {
        if (initialized())
        {
            bgfx::frame();
        }
    }
}    // namespace QQuickBgfx
