#include "QtLiquidGlass/QtLiquidGlass.h"
#include "QtLiquidGlassCommon.h"

#include <QApplication>
#include <QWidget>

#import <AppKit/AppKit.h>

#include <cmath>
#include <iostream>

namespace {

int fail(const char* message) {
    std::cerr << "FAIL: " << message << '\n';
    return 1;
}

bool closeEnough(CGFloat lhs, CGFloat rhs) {
    return std::abs(lhs - rhs) < 0.01;
}

bool isGlassView(NSView* view) {
    Class glassClass = NSClassFromString(@"NSGlassEffectView");
    if (glassClass && [view isKindOfClass:glassClass]) return true;
    return [view isKindOfClass:[NSVisualEffectView class]];
}

NSView* findGlassView(NSView* container) {
    for (NSView* child in container.subviews) {
        if (isGlassView(child)) return child;
    }
    return nil;
}

NSUInteger countGlassViews(NSView* container) {
    NSUInteger count = 0;
    for (NSView* child in container.subviews) {
        if (isGlassView(child)) ++count;
    }
    return count;
}

NSUInteger countCustomBoxes(NSView* container) {
    NSUInteger count = 0;
    for (NSView* child in container.subviews) {
        if ([child isKindOfClass:[NSBox class]] &&
            [(NSBox*)child boxType] == NSBoxCustom) {
            ++count;
        }
    }
    return count;
}

} // namespace

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QWidget widget;
    widget.setWindowFlags(Qt::Window);
    widget.setAttribute(Qt::WA_NativeWindow);
    widget.setAttribute(Qt::WA_TranslucentBackground);
    widget.resize(180, 100);

    NSView* host = reinterpret_cast<NSView*>(widget.winId());
    NSWindow* window = host.window;
    NSView* container = host.superview;
    if (!window || !container) return fail("Qt did not create the expected native hierarchy");

    window.styleMask &= ~NSWindowStyleMaskFullSizeContentView;
    window.titlebarAppearsTransparent = NO;
    window.movableByWindowBackground = NO;
    NSColor* originalBackground = [NSColor colorWithRed:0.15 green:0.25 blue:0.35 alpha:1.0];
    window.backgroundColor = originalBackground;
    const BOOL originalWindowOpaque = window.opaque;

    container.wantsLayer = YES;
    container.layer.cornerRadius = 3.5;
    container.layer.masksToBounds = NO;

    const NSUInteger initialGlassCount = countGlassViews(container);
    const NSUInteger initialBoxCount = countCustomBoxes(container);

    QtLiquidGlass::Options options;
    options.cornerRadius = 14.0;
    options.titlebarStyle = QtLiquidGlass::TitlebarStyle::TransparentFullSize;
    options.dragBehavior = QtLiquidGlass::WindowDragBehavior::Auto;

    const int effectId = QtLiquidGlass::addGlassEffect(&widget,
                                                        QtLiquidGlass::Material::Sidebar,
                                                        options);
    if (effectId < 0) return fail("addGlassEffect returned an invalid ID");

    container = host.superview;
    NSView* originalGlass = findGlassView(container);
    if (!originalGlass || countGlassViews(container) != initialGlassCount + 1)
        return fail("glass insertion produced an incorrect view count");
    if (!(window.styleMask & NSWindowStyleMaskFullSizeContentView) ||
        !window.titlebarAppearsTransparent ||
        !window.movableByWindowBackground || window.opaque) {
        return fail("initial window options were not applied");
    }
    if (!closeEnough(container.layer.cornerRadius, 14.0) ||
        !container.layer.masksToBounds) {
        return fail("initial container rounding was not applied");
    }

    options.cornerRadius = 9.0;
    options.opaque = true;
    options.titlebarStyle = QtLiquidGlass::TitlebarStyle::Preserve;
    options.dragBehavior = QtLiquidGlass::WindowDragBehavior::MovableByWindowBackground;
    QtLiquidGlass::configure(effectId, options);
    QtLiquidGlass::configure(effectId, options);

    if (window.styleMask & NSWindowStyleMaskFullSizeContentView)
        return fail("Preserve did not restore the original full-size-content policy");
    if (window.titlebarAppearsTransparent)
        return fail("Preserve did not restore the original titlebar policy");
    if (!window.movableByWindowBackground)
        return fail("the explicit movable policy was not applied");
    if (countCustomBoxes(container) != initialBoxCount + 1)
        return fail("repeated configuration accumulated backing views");
    if (findGlassView(container) != originalGlass)
        return fail("configuration replaced the native glass view");

    options.cornerRadius = 6.0;
    options.opaque = false;
    options.titlebarStyle = QtLiquidGlass::TitlebarStyle::TransparentFullSize;
    options.dragBehavior = QtLiquidGlass::WindowDragBehavior::NotMovableByWindowBackground;
    QtLiquidGlass::configure(effectId, options);

    if (countCustomBoxes(container) != initialBoxCount)
        return fail("disabling opaque mode did not remove its backing view");
    if (!window.titlebarAppearsTransparent || window.movableByWindowBackground)
        return fail("updated titlebar or drag options were not applied");

    QtLiquidGlass::remove(effectId);
    if (countGlassViews(container) != initialGlassCount ||
        countCustomBoxes(container) != initialBoxCount) {
        return fail("removal left native effect views behind");
    }
    if (window.styleMask & NSWindowStyleMaskFullSizeContentView)
        return fail("removal did not restore the full-size-content policy");
    if (window.titlebarAppearsTransparent)
        return fail("removal did not restore the titlebar transparency policy");
    if (window.movableByWindowBackground)
        return fail("removal did not restore the window drag policy");
    if (window.opaque != originalWindowOpaque)
        return fail("removal did not restore window opacity");
    if (![window.backgroundColor isEqual:originalBackground])
        return fail("removal did not restore the window background color");
    if (!container.wantsLayer ||
        !closeEnough(container.layer.cornerRadius, 3.5) ||
        container.layer.masksToBounds) {
        return fail("removal did not restore the container layer");
    }

    QWidget parent;
    parent.setWindowFlags(Qt::Window);
    QWidget child(&parent);
    child.setAttribute(Qt::WA_NativeWindow);
    child.resize(80, 40);
    parent.resize(160, 90);
    parent.winId();

    NSView* childHost = reinterpret_cast<NSView*>(child.winId());
    const NSUInteger childInitialGlassCount = countGlassViews(childHost);
    const int childId = QtLiquidGlass::addGlassEffect(&child);
    if (childId < 0 || countGlassViews(childHost) != childInitialGlassCount + 1)
        return fail("child-widget glass insertion failed");
    QtLiquidGlass::remove(childId);
    if (countGlassViews(childHost) != childInitialGlassCount)
        return fail("child-widget removal left a glass view behind");

    NSWindow* nativeWindow = [[NSWindow alloc]
        initWithContentRect:NSMakeRect(0, 0, 120, 70)
                  styleMask:NSWindowStyleMaskTitled
                    backing:NSBackingStoreBuffered
                      defer:NO];
    NSView* nativeRoot = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 120, 70)];
    nativeWindow.contentView = nativeRoot;
    nativeWindow.opaque = YES;
    NSColor* nativeBackground =
        [NSColor colorWithRed:0.4 green:0.3 blue:0.2 alpha:1.0];
    nativeWindow.backgroundColor = nativeBackground;

    const int nativeId = AddGlassEffectView(nativeRoot, false, 1, 2);
    if (nativeId < 0 || nativeWindow.opaque)
        return fail("native root setup did not apply transparent window state");
    RemoveGlassEffectView(nativeId);
    if (!nativeWindow.opaque || ![nativeWindow.backgroundColor isEqual:nativeBackground])
        return fail("native root removal did not restore opacity and background color");

    [nativeRoot release];
    [nativeWindow release];

    std::cout << "All glass configuration checks passed.\n";
    return 0;
}
