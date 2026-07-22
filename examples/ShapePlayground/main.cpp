#include "ShapePlayground.h"

#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("QtLiquidGlass Morphing Player");

    ShapePlayground playground;
    playground.show();
    return app.exec();
}
