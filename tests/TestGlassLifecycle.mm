#include "QtLiquidGlass/QtLiquidGlass.h"
#include "QtLiquidGlassCommon.h"

#include <QApplication>
#include <QWidget>

#import <AppKit/AppKit.h>

#include <iostream>

namespace {

int fail(const char* message) {
    std::cerr << "FAIL: " << message << '\n';
    return 1;
}

} // namespace

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QtLiquidGlass::Options options;
    QtLiquidGlass::configure(-1, options);
    QtLiquidGlass::setIntProperty(-1, "variant", 1);
    QtLiquidGlass::remove(-1);

    QWidget widget;
    widget.setWindowFlags(Qt::Window);
    widget.resize(160, 90);

    const int firstId = QtLiquidGlass::addGlassEffect(&widget);
    if (firstId < 0)
        return fail("the first effect was not registered");

    const int replacementId = QtLiquidGlass::addGlassEffect(&widget);
    if (replacementId < 0 || replacementId == firstId)
        return fail("replacing an effect did not produce a new ID");
    if (RemoveGlassEffectView(firstId))
        return fail("effect replacement left the previous registry entry active");

    QtLiquidGlass::configure(firstId, options);
    QtLiquidGlass::setIntProperty(firstId, "variant", 1);
    QtLiquidGlass::remove(firstId);

    if (!RemoveGlassEffectView(replacementId) || RemoveGlassEffectView(replacementId))
        return fail("explicit removal did not become a safe stale-ID operation");

    const int readdedId = QtLiquidGlass::addGlassEffect(&widget);
    if (readdedId < 0)
        return fail("re-adding after explicit removal failed");
    QtLiquidGlass::configure(readdedId, options);
    if (!RemoveGlassEffectView(readdedId))
        return fail("the re-added effect was not registered");

    QWidget* destroyedWidget = new QWidget;
    destroyedWidget->setWindowFlags(Qt::Window);
    const int destroyedWidgetId = QtLiquidGlass::addGlassEffect(destroyedWidget);
    delete destroyedWidget;
    if (RemoveGlassEffectView(destroyedWidgetId))
        return fail("QWidget destruction did not clear the registry");

    NSView* nativeHost = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 80, 40)];
    const int nativeId = AddGlassEffectView(nativeHost, false, 0, 1);
    if (nativeId < 0)
        return fail("the standalone native effect was not registered");
    [nativeHost release];
    if (RemoveGlassEffectView(nativeId))
        return fail("native host destruction did not clear the registry");

    NSView* explicitlyRemovedHost =
        [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 80, 40)];
    const int explicitlyRemovedId =
        AddGlassEffectView(explicitlyRemovedHost, false, 0, 1);
    if (!RemoveGlassEffectView(explicitlyRemovedId))
        return fail("explicit native removal did not find its registry entry");
    [explicitlyRemovedHost release];
    if (RemoveGlassEffectView(explicitlyRemovedId))
        return fail("explicit native removal left a registry entry");

    std::cout << "All glass lifecycle checks passed.\n";
    return 0;
}
