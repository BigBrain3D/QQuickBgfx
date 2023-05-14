#include "qquick_bgfx.h"

#include <QQuickWindow>
#include <QSGTextureProvider>

#ifdef _WIN32
#include <d3d11.h>
#include <wrl/client.h>

bgfx::Init QQuickBgfx::initD3D11Backend(void *windowHandler, void *context, const uint16_t width, const uint16_t height)
{
    if (!initialized())
    {
        bgfx::Init init;
        init.type = bgfx::RendererType::Direct3D11;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.resolution.width = width;
        init.resolution.height = height;
        init.platformData.context = reinterpret_cast<ID3D11Device *>(context);
        return init;
    }
    else return bgfx::Init();
}

QQuickBgfx::TextureHandles QQuickBgfx::CreateQSGD3D11Texture(QQuickWindow *window, int w, int h)
{
    D3D11_TEXTURE2D_DESC d3d11_texture_desc =
      CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_R8G8B8A8_UNORM, w, h, 1, 1,
                            D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, D3D11_USAGE_DEFAULT, 0, 1, 0, 0);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> d3d11_texture;
    auto dev = (ID3D11Device *)bgfx::getInternalData()->context;
    if (dev->CreateTexture2D(&d3d11_texture_desc, nullptr, &d3d11_texture) != S_OK)
        return {nullptr, reinterpret_cast<uintptr_t>(d3d11_texture.Get())};

    return {QNativeInterface::QSGD3D11Texture::fromNative(d3d11_texture.Get(), window, {w, h}),
            reinterpret_cast<uintptr_t>(d3d11_texture.Get())};
}
#else
#include <GL/glx.h>
#include <QOpenGLContext>
#include <qopenglfunctions.h>

bgfx::Init QQuickBgfx::initGLBackend(void* windowHandle, void* context, const uint16_t width, const uint16_t height) {
    if (!initialized())
    {
        bgfx::Init init;
        init.type = bgfx::RendererType::OpenGL;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.resolution.width = width;
        init.resolution.height = height;
        init.platformData.context = context;
        init.platformData.nwh = windowHandle;
        return init;
    }
    else return bgfx::Init();
}

QQuickBgfx::TextureHandles QQuickBgfx::CreateGLTexture(QQuickWindow *window, int w, int h, QOpenGLContext* context)
{
    context->makeCurrent(window);

    auto* funcs = context->functions();
    
    GLuint texture;

    funcs->glGenTextures(1, &texture);

    funcs->glBindTexture(GL_TEXTURE_2D, texture);

    funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    context->doneCurrent();

    return {QNativeInterface::QSGOpenGLTexture::fromNative(texture, window, {w, h}), static_cast<uintptr_t>(texture)};
}
#endif