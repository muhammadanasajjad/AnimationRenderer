#include <QApplication>
#include <QSurfaceFormat>

#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(4, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(0);
    format.setSwapInterval(1);

    QSurfaceFormat::setDefaultFormat(format);

    QApplication application(argc, argv);

    MainWindow window;
    window.show();

    return application.exec();
}
