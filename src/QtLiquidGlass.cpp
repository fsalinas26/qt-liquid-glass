#include "QtLiquidGlass/QtLiquidGlass.h"
#include "QtLiquidGlassCommon.h"

#include <QWidget>
#include <QWindow>
#include <QColor>

namespace QtLiquidGlass {

int addGlassEffect(QWidget* widget, Material material, const Options& opts) {
#ifdef PLATFORM_OSX
    if (!widget) return -1;
    
    // Ensure the widget has a native window handle
    if (!widget->testAttribute(Qt::WA_NativeWindow)) {
        widget->setAttribute(Qt::WA_NativeWindow);
    }
    
    widget->setAttribute(Qt::WA_TranslucentBackground);
    
    // Get the winId (NSView*)
    WId wid = widget->winId();
    void* nativeViewPtr = reinterpret_cast<void*>(wid);
    
    int id = AddGlassEffectView(nativeViewPtr,
                                opts.opaque,
                                static_cast<int>(opts.titlebarStyle),
                                static_cast<int>(opts.dragBehavior));
    
    if (id >= 0) {
        long long variantVal = 0;  // NSGlassEffectView private variant
        long long materialVal = 0; // NSVisualEffectView fallback material

        switch (material) {
            case Material::Sidebar:
                variantVal = 16; // 'sidebar'
                materialVal = 7; // NSVisualEffectMaterialSidebar
                break;
            case Material::Sheet:
                variantVal = 0; // 'regular' (standard sheet look)
                materialVal = 11; // NSVisualEffectMaterialSheet
                break;
            case Material::Hud:
                variantVal = 2; // 'dock' (dark, satiny)
                materialVal = 13; // NSVisualEffectMaterialHUDWindow
                break;
            case Material::WindowBackground:
                variantVal = 1; // 'clear' (subtle)
                materialVal = 12; // NSVisualEffectMaterialWindowBackground
                break;
            case Material::Popover:
                variantVal = 23; // 'cartouchePopover' (modern popover)
                materialVal = 6; // NSVisualEffectMaterialPopover
                break;
            case Material::Menu:
                variantVal = 9; // 'notificationCenter' (closest to menu)
                materialVal = 5; // NSVisualEffectMaterialMenu
                break;
            case Material::FullscreenUI:
                variantVal = 6; // 'avplayer' (deep blur)
                materialVal = 15; // NSVisualEffectMaterialFullScreenUI
                break;
            case Material::ControlCenter:
                variantVal = 8; // 'controlCenter'
                materialVal = 5; // Menu is reasonable fallback
                break;
            case Material::Widgets:
                variantVal = 4; // 'widgets'
                materialVal = 18; // NSVisualEffectMaterialContentBackground
                break;
            case Material::Inspector:
                variantVal = 18; // 'inspector'
                materialVal = 7; // Sidebar
                break;
            case Material::Titlebar:
                variantVal = 17; // 'abuttedSidebar' (sidebar meets titlebar)
                materialVal = 3; // Titlebar
                break;
            case Material::Tooltip:
                variantVal = 20; // 'loupe'
                materialVal = 17; // ToolTip
                break;
            case Material::Frosted:
                variantVal = 11; // Discovered via explorer
                materialVal = 1; // Light
                break;
            case Material::ClearGlass:
                variantVal = 13; // Discovered via explorer
                materialVal = 0; // AppearanceBased
                break;
            case Material::Chromatic:
                variantVal = 19; // Discovered via explorer
                materialVal = 1; // Light
                break;
        }

        // Set variant/material before configure — variant changes can rebuild
        // internal sublayers, which would lose a previously-set corner radius
        SetGlassViewVariant(id, (int)variantVal);
        SetGlassViewMaterial(id, (int)materialVal);
        configure(id, opts);
    }
    
    return id;
#else
    Q_UNUSED(widget);
    Q_UNUSED(material);
    Q_UNUSED(opts);
    return -1;
#endif
}

void configure(int id, const Options& opts) {
#ifdef PLATFORM_OSX
    bool hasTint = false;
    double r=0, g=0, b=0, a=0;
    if (!opts.tintColor.isEmpty()) {
        QColor c(opts.tintColor);
        if (c.isValid()) {
            hasTint = true;
            r = c.redF();
            g = c.greenF();
            b = c.blueF();
            a = c.alphaF();
        }
    }
    
    ConfigureGlassView(id, opts.cornerRadius, hasTint, r, g, b, a);
    SetGlassViewBlendingMode(id, static_cast<int>(opts.blendingMode));
    SetGlassViewAdaptiveAppearance(id, static_cast<int>(opts.appearance));
    SetGlassViewInteractionState(id, static_cast<int>(opts.interaction));
#else
    Q_UNUSED(id);
    Q_UNUSED(opts);
#endif
}

void setIntProperty(int id, const QString& key, long long value) {
#ifdef PLATFORM_OSX
    if (key == "variant") {
        SetGlassViewVariant(id, (int)value);
    } else if (key == "material") {
        SetGlassViewMaterial(id, (int)value);
    } else if (key == "scrim" || key == "scrimState") {
        SetGlassViewScrim(id, (int)value);
    } else if (key == "subdued" || key == "subduedState") {
        SetGlassViewSubdued(id, (int)value);
    } else if (key == "contentLensing") {
        SetGlassViewContentLensing(id, (int)value);
    } else if (key == "adaptiveAppearance" || key == "appearance") {
        SetGlassViewAdaptiveAppearance(id, (int)value);
    } else if (key == "interactionState" || key == "interaction") {
        SetGlassViewInteractionState(id, (int)value);
    } else if (key == "blendingMode") {
        SetGlassViewBlendingMode(id, (int)value);
    }
#else
    Q_UNUSED(id);
    Q_UNUSED(key);
    Q_UNUSED(value);
#endif
}

void remove(int id) {
#ifdef PLATFORM_OSX
    RemoveGlassEffectView(id);
#else
    Q_UNUSED(id);
#endif
}

} // namespace QtLiquidGlass
