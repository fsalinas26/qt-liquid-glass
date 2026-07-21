#include "QtLiquidGlass/QtLiquidGlass.h"

#include <QApplication>
#include <QPainterPath>
#include <QTransform>
#include <QWidget>

#import <AppKit/AppKit.h>
#import <objc/message.h>
#import <objc/runtime.h>

#include <cmath>
#include <iostream>

namespace {

NSView* findGlassView(QWidget& widget) {
    NSView* host = reinterpret_cast<NSView*>(widget.winId());
    NSView* container = host.superview ? host.superview : host;
    Class glassClass = NSClassFromString(@"NSGlassEffectView");
    for (NSView* child in container.subviews) {
        if (glassClass && [child isKindOfClass:glassClass]) return child;
    }
    return nil;
}

bool closeEnough(CGFloat lhs, CGFloat rhs) {
    return std::abs(lhs - rhs) < 0.01;
}

int fail(const char* message) {
    std::cerr << "FAIL: " << message << '\n';
    return 1;
}

} // namespace

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QPainterPath invalidPath;
    invalidPath.addRect(0.0, 0.0, 10.0, 10.0);
    if (QtLiquidGlass::Experimental::supportsCustomShapes(-1) ||
        QtLiquidGlass::Experimental::setShape(-1, invalidPath) ||
        QtLiquidGlass::Experimental::clearShape(-1) ||
        QtLiquidGlass::Experimental::setClipsToBounds(-1, true)) {
        return fail("experimental operations should reject an invalid effect ID");
    }

    QWidget widget;
    widget.setWindowFlags(Qt::Window);
    widget.resize(160, 90);

    const int effectId = QtLiquidGlass::addGlassEffect(&widget);
    if (effectId < 0) return fail("addGlassEffect returned an invalid ID");

    if (!QtLiquidGlass::Experimental::supportsCustomShapes(effectId)) {
        std::cout << "Custom shapes are unavailable on this AppKit runtime; skipped.\n";
        QtLiquidGlass::remove(effectId);
        return 0;
    }

    QPainterPath shape;
    shape.moveTo(10.0, 5.0);
    shape.lineTo(110.0, 5.0);
    shape.cubicTo(120.0, 5.0, 120.0, 45.0, 110.0, 45.0);
    shape.lineTo(10.0, 45.0);
    shape.closeSubpath();

    if (!QtLiquidGlass::Experimental::setShape(effectId, shape))
        return fail("setShape rejected a supported glass view");

    NSView* glass = findGlassView(widget);
    if (!glass) return fail("could not find the injected NSGlassEffectView");

    SEL pathSelector = sel_registerName("_path");
    CGPathRef nativePath = ((CGPathRef (*)(id, SEL))objc_msgSend)(glass, pathSelector);
    if (!nativePath) return fail("native glass path was not installed");

    const CGRect box = CGPathGetBoundingBox(nativePath);
    const CGFloat expectedMinY = NSHeight(glass.bounds) - 45.0;
    if (!closeEnough(CGRectGetMinX(box), 10.0) ||
        !closeEnough(CGRectGetMaxX(box), 120.0) ||
        !closeEnough(CGRectGetMinY(box), expectedMinY) ||
        !closeEnough(CGRectGetMaxY(box), NSHeight(glass.bounds) - 5.0)) {
        return fail("QPainterPath was not converted into expected AppKit coordinates");
    }

    const QPainterPath translated = QTransform::fromTranslate(7.0, 3.0).map(shape);
    if (!QtLiquidGlass::Experimental::setShape(effectId, translated))
        return fail("setShape rejected a transformed QPainterPath");
    nativePath = ((CGPathRef (*)(id, SEL))objc_msgSend)(glass, pathSelector);
    const CGRect translatedBox = CGPathGetBoundingBox(nativePath);
    if (!closeEnough(CGRectGetMinX(translatedBox), 17.0) ||
        !closeEnough(CGRectGetMaxY(translatedBox), NSHeight(glass.bounds) - 8.0)) {
        return fail("QTransform changes were not preserved in the native glass path");
    }

    if (!QtLiquidGlass::Experimental::setClipsToBounds(effectId, true))
        return fail("setClipsToBounds rejected a supported glass view");
    if (!glass.clipsToBounds) return fail("native clipsToBounds was not enabled");

    if (!QtLiquidGlass::Experimental::clearShape(effectId))
        return fail("clearShape rejected a supported glass view");
    nativePath = ((CGPathRef (*)(id, SEL))objc_msgSend)(glass, pathSelector);
    if (nativePath) return fail("clearShape did not clear the custom native path");

    QtLiquidGlass::remove(effectId);
    std::cout << "All custom glass shape checks passed.\n";
    return 0;
}
