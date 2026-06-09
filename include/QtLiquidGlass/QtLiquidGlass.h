#ifndef QTLIQUIDGLASS_H
#define QTLIQUIDGLASS_H

#include <QWidget>
#include <QString>

namespace QtLiquidGlass {

enum class Material {
    Sidebar,
    Sheet,
    Hud,
    WindowBackground,
    Popover,
    Menu,
    FullscreenUI,
    ControlCenter,
    Widgets,
    Inspector,
    Titlebar,
    Tooltip,
    Frosted,
    ClearGlass,
    Chromatic
};

enum class BlendingMode {
    BehindWindow = 0,
    WithinWindow = 1
};

enum class AdaptiveAppearance {
    Light = 0,
    Dark = 1,
    Auto = 2
};

enum class InteractionState {
    Normal = 0,
    Hovered = 1
};

enum class TitlebarStyle {
    Preserve = 0,
    TransparentFullSize = 1
};

enum class WindowDragBehavior {
    Auto = 0,
    Preserve = 1,
    MovableByWindowBackground = 2,
    NotMovableByWindowBackground = 3
};

struct Options {
    double cornerRadius = 0.0;
    QString tintColor = "";    // "#RRGGBB" or "#AARRGGBB"
    bool opaque = false;
    TitlebarStyle titlebarStyle = TitlebarStyle::TransparentFullSize;
    WindowDragBehavior dragBehavior = WindowDragBehavior::Auto;
    BlendingMode blendingMode = BlendingMode::BehindWindow;
    AdaptiveAppearance appearance = AdaptiveAppearance::Auto;
    InteractionState interaction = InteractionState::Normal;
};

// Returns id >= 0, or -1 on failure.
int addGlassEffect(QWidget* widget,
                   Material material = Material::Sidebar,
                   const Options& opts = {});

// Modify corner radius, tint, opacity, etc.
void configure(int id, const Options& opts);

// Modify custom int properties (variant, scrimState, subduedState)
void setIntProperty(int id, const QString& key, long long value);

// Remove and cleanup
void remove(int id);

} // namespace QtLiquidGlass

#endif // QTLIQUIDGLASS_H
