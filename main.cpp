#include <src/qbgfx.h>
#include <qquickbgfxitem/qquickbgfxitem.h>

#include <QGuiApplication>
#include <QQuickView>

#include <bgfx/bgfx.h>
#include <qquick_bgfx.h>
#include <bx/bounds.h>
#include <bx/math.h>
#include <debugdraw/debugdraw.h>

//init_example initialize bgfx from a gien bgfx::Init
void init_example(const bgfx::Init& init)
{
    if (!QQuickBgfx::initialized())
    {
        bgfx::renderFrame();
        bgfx::init(init);
        bgfx::setDebug(BGFX_DEBUG_TEXT);
        ddInit();
    }
}

//render_example runs the rendering code
void render_example(const std::vector<QQuickBgfxItem*>& bgfxItems)
{
    printf("%d\n", bgfxItems.size());

    for(const auto item : bgfxItems)
    {
        if (item->viewId() < 256)
        {
            float r{0.0f};
            float g{0.0f};
            float b{0.0f};

            auto c = item->backgroundColor();
            c.setHslF(c.hueF(), c.saturationF(), c.lightnessF() * std::clamp(item->mousePosition()[1] / (float)item->height(), 0.0f, 1.0f));
            c.getRgbF(&r, &g, &b);

            const uint32_t color = uint8_t(r * 255) << 24 | uint8_t(g * 255) << 16 | uint8_t(b * 255) << 8 | 255;

            bgfx::setViewClear(item->viewId(), BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, color, 1.0f, 0);
            
            bgfx::touch(item->viewId());

            const auto w = item->dprWidth();
            const auto h = item->dprHeight();
            static float time{0.0f};
            time += 0.003f;

            const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
			const bx::Vec3 eye = { 
                std::clamp(item->mousePosition()[0] / (float)item->width()-0.5f, -0.5f, 0.5f) * 15.0f, 
                0.0f, 
                std::clamp(item->mousePosition()[1] / (float)item->height()-0.5f, -0.5f, 0.5f) * 15.0f 
            };

            float view[16];
            bx::mtxLookAt(view, eye, at, { 0.F, 1.F, 0.F });

            float proj[16];
            bx::mtxProj(proj, 60.0f, float(w)/float(h), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            
            bgfx::setViewTransform(item->viewId(), view, proj);
            bgfx::setViewRect(item->viewId(), 0, 0, uint16_t(w), uint16_t(h) );

            float mtx[16];
            bx::mtxRotateXY(mtx, time, time);
            
            DebugDrawEncoder dde;
            dde.begin(item->viewId());
            dde.drawAxis(0.0f, 0.0f, 0.0f);
            dde.setTransform(&mtx);
            dde.draw(bx::Aabb{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}});
            dde.end();
        }
    }
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

#if defined(QQ_ENABLE_OPENGL)
    QQuickWIndow::setGraphicsApi(QSGRendererInterface::OpenGLRhi);
#elif defined(QQ_ENABLE_METAL)
    QQuickWindow::setGraphicsApi(QSGRendererInterface::MetalRhi);
#elif defined(QQ_ENABLE_DIRECTX)
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Direct3D11Rhi);
#endif

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///BgfxQQuickItemExample/main.qml"));
    view.show();

    const auto qbgfx = QQuickBgfx::QBgfx(static_cast<QQuickWindow*>(&view), view.rootObject()->findChildren<QQuickBgfxItem*>());
    
    //Connection to initialized signal allows to decouple the bgfx initialization from the qquick_bgfx::QBgfx wrapper
    QObject::connect(&qbgfx, &QQuickBgfx::QBgfx::initialized, init_example);
    //Connection to render signal allows to decouple the rendering code from the qquick_bgfx::QBgfx wrapper
    QObject::connect(&qbgfx, &QQuickBgfx::QBgfx::render, render_example);
    
    return app.exec();
}
