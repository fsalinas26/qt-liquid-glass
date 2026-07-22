# QtLiquidGlass 0.4.0

QtLiquidGlass 0.4.0 hardens effect lifetime and native-state restoration while adding an experimental custom-shape API. Existing v0.3.0 source usage remains compatible.

## Added

- Experimental `QPainterPath` glass geometry through `QtLiquidGlass::Experimental`.
- Runtime capability checks for custom paths and clipping.
- Support for paths transformed with `QTransform`.
- Automatic effect invalidation when a Qt or native host is destroyed.
- Native lifecycle, restoration, configuration, fallback, and shape regression tests.
- A standalone morphing media player demonstrating animated custom paths.

## Changed

- `configure()` now applies `opaque`, `titlebarStyle`, and `dragBehavior` without replacing the native glass view.
- Repeated configuration no longer accumulates opaque backing views.
- Explicit removal restores captured window policy, background state, and container-layer state.
- A library-created frameless wrapper is dismantled during removal when that native hierarchy is encountered.
- The demo updates ordinary options in place and recreates glass only for material changes.
- CMake now enforces the documented Qt 6.2 minimum.

## Compatibility notes

- Existing public functions, enums, `Options` field order, and material mappings are unchanged.
- Material still requires recreating an effect because it is not part of `Options`.
- Calls must be made on Qt's GUI thread.
- Stale integer IDs are accepted as safe no-ops; experimental operations return `false`.
- `NSGlassEffectView` on the tested macOS 26 runtime does not implement `setBlendingMode:`. `BlendingMode` affects the `NSVisualEffectView` fallback on that runtime.
- Custom paths rely on the private `_setPath:` selector and are unavailable on the fallback backend. Applications should call `supportsCustomShapes()` first.
- Private AppKit material variants and experimental shape behavior can change between macOS releases.
